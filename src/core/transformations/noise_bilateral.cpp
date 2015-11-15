#include "noise_bilateral.h"

NoiseBilateral::NoiseBilateral(PNM* img) :
    Convolution(img)
{
}

NoiseBilateral::NoiseBilateral(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseBilateral::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    sigma_d = getParameter("sigma_d").toInt();
    sigma_r = getParameter("sigma_r").toInt();
    radius = sigma_d;

    if(image->format() == QImage::Format_Indexed8){
        for(int x = 0; x < width; x++){
            for(int y  = 0; y < height; y++){
                newImage -> setPixel(x, y, calcVal(x, y, LChannel));
            }
        }
    } else if(image->format() == QImage::Format_RGB32){
        for(int x = 0; x < width; x++){
            for(int y = 0; y < height; y++){
                newImage -> setPixel(x, y, qRgb(calcVal(x, y, RChannel),
                                                calcVal(x, y, GChannel),
                                                calcVal(x, y, BChannel)));
            }
        }
    }

    return newImage;
}

int NoiseBilateral::calcVal(int x, int y, Channel channel)
{
    int size = radius;
    math::matrix<float> okno = getWindow(x, y, size, channel, RepeatEdge);
    float sumaL = 0, sumaM = 0, colorSpatC = 0;

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            colorSpatC = colorCloseness(okno(i, j), okno(size/2, size/2)) * spatialCloseness(QPoint(i, j), QPoint(radius/2, radius/2));
            sumaL += okno(i, j) * colorSpatC;
            sumaM += colorSpatC;
        }
    }
    return (int)(sumaL / sumaM);
}

float NoiseBilateral::colorCloseness(int val1, int val2)
{
    return exp(pow((val1-val2), 2) / (2 * pow(sigma_r, 2) * (-1)));
}

float NoiseBilateral::spatialCloseness(QPoint point1, QPoint point2)
{
    return exp((pow(point1.x()-point2.x(), 2) + pow(point1.y()-point2.y(), 2)) / (2 * pow(sigma_d, 2) * (-1)));
}
