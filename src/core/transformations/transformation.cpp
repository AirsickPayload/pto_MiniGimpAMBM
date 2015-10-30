#include "transformation.h"

Transformation::Transformation(PNM* image) :
    QThread(0)
{
    parameters = new QHash<QString, QVariant>;
    this->image = image;
    this->supervisor = 0;
}

Transformation::Transformation(PNM* image, ImageViewer* iv) :
    QThread(0)
{
    parameters = new QHash<QString, QVariant>;
    this->image = image;
    this->supervisor = iv;

    if (iv)
    {
        connect(this, SIGNAL(started()), iv, SLOT(transformationStarted()));
        connect(this, SIGNAL(finished()), iv, SLOT(transformationFinished()));
        connect(this, SIGNAL(message(QString)), iv, SLOT(toolMessage(QString)));
        connect(this, SIGNAL(progress(int)), iv, SLOT(toolProgress(int)));
        connect(this, SIGNAL(resultReady(PNM*)), iv, SLOT(updateImage(PNM*)));
        connect(this, SIGNAL(finished()), iv, SLOT(toolFinished()));
    }
}

Transformation::~Transformation()
{
    delete parameters;
    if (supervisor)
    {
        disconnect(this, SIGNAL(started()));
        disconnect(this, SIGNAL(message(QString)));
        disconnect(this, SIGNAL(progress(int)));
        disconnect(this, SIGNAL(resultReady(PNM*)));
        disconnect(this, SIGNAL(finished()));
    }
}

/** Sets a parameter param of the transformation
* with the value.
* If the parameter was already set it is overridden
* Returns the pointer to the Transformation object so it can be used as a factory.
*/
Transformation* Transformation::setParameter(QString param, QVariant value)
{
    parameters->insert(param, value);
    return this;
}

/** Returns the value of the given param.
* If the param isn't set returns a default QVariant()
* which is an invalid variant
* @see QVariant::QVariant()
*/
QVariant Transformation::getParameter(QString param)
{
    return parameters->value(param, QVariant());
}

void Transformation::run()
{
    PNM* image = this->transform();
    emit resultReady(image);
}

/**
* /virtual, abstract!/
* Virtual method that do some transformations (based on parameter Hash)
* in the Transformation class it returns a pointer to
* the loaded image or a new null PNM image
*/
PNM* Transformation::transform()
{
    if (image)
        return image;
    else
        return new PNM();
}

/** Returns a pointer to the image stored in the class */
PNM* Transformation::getImage()
{
    return image;
}

/** Returns a pixel value at given coordinates using different modes */
QRgb Transformation::getPixel(int x, int y, Mode mode)
{
    switch (mode) {
    case CyclicEdge:    return getPixelCyclic(x,y);
    case NullEdge:      return getPixelNull(x,y);
    case RepeatEdge:    return getPixelRepeat(x,y);
    default:            return image->pixel(x,y);
    }

}

/** Returns a pixel using the Cyclic mode:
 *  pixel(x,y) = pixel(x%width, y%width);
 */
QRgb Transformation::getPixelCyclic(int x, int y)
{
    return image->pixel(x % image->width(),y % image->height());
}

/**
  * Returns a given pixel
  * If the pixel is out of image boundaries Black is returned;
  */
QRgb Transformation::getPixelNull(int x, int y)
{
    if(x >= image->width() || x < 0 || y >= image->height() || y < 0){
        return qRgb(0,0,0);
    }
    return image->pixel(x,y);
}

/**
  * Returns given pixel.
  * If the pixel is out of image boundaries
  * the nearest edge pixel is given
  */
QRgb Transformation::getPixelRepeat(int x, int y)
{
    if(x >= image->width()){
        x = image->width() - 1;
    }else if(x < 0){
        x = 0;
    }

    if(y >= image->height()){
        y = image->height() - 1;
    }else if(y < 0){
        y = 0;
    }

    return image->pixel(x,y);
}

/** Returns a size x size part of the image centered around (x,y) */
math::matrix<float> Transformation::getWindow(int x, int y, int size,
                                              Channel channel,
                                              Mode mode = RepeatEdge)
{
    math::matrix<float> window(size,size);
    int sasiedztwo = floor(size/2);

    int  m_X = 0, m_Y = 0;

    //nieparzysta macierz
    if( (size % 2) != 0){
      //zaczynamy od "lewej" az do "prawej" strony sasiedztwa
        for(int w = -sasiedztwo; w <= sasiedztwo; w++){
            for(int k = -sasiedztwo; k <= sasiedztwo; k++){
                 switch(channel){
                    case RChannel:
                     window(m_X, m_Y) = qRed(getPixel(x + w, y + k, mode));
                     break;
                    case GChannel:
                     window(m_X, m_Y) = qGreen(getPixel(x + w, y + k, mode));
                     break;
                    case BChannel:
                     window(m_X, m_Y) = qBlue(getPixel(x + w, y + k, mode));
                     break;
                    case LChannel:
                     window(m_X, m_Y) = qGray(getPixel(x + w, y + k, mode));
                     break;
                 }
                 m_Y += 1;
            }
            m_X += 1; m_Y = 0;
        }
    }else{
        for(int w = -sasiedztwo; w < sasiedztwo; w++){
            for(int k = -sasiedztwo; k < sasiedztwo; k++){
                 switch(channel){
                    case RChannel:
                     window(m_X, m_Y) = qRed(getPixel(x + w, y + k, mode));
                     break;
                    case GChannel:
                     window(m_X, m_Y) = qGreen(getPixel(x + w, y + k, mode));
                     break;
                    case BChannel:
                     window(m_X, m_Y) = qBlue(getPixel(x + w, y + k, mode));
                     break;
                    case LChannel:
                     window(m_X, m_Y) = qGray(getPixel(x + w, y + k, mode));
                     break;
                 }
                 m_Y += 1;
            }
            m_X += 1; m_Y = 0;
    }
   }
    return window;
}

ImageViewer* Transformation::getSupervisor()
{
    return supervisor;
}
