#include "correction.h"

Correction::Correction(PNM* img) :
    Transformation(img)
{
}

Correction::Correction(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* Correction::transform()
{
    float shift  = getParameter("shift").toFloat();
    float factor = getParameter("factor").toFloat();
    float gamma  = getParameter("gamma").toFloat();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    // Inicjalizacja tablicy
    for(int i = 0; i<= PIXEL_VAL_MAX; i++){
        LUT[i] = i;
    }
    // Tablicowanie
    for(int i = 0; i<= PIXEL_VAL_MAX; i++){
        LUT[i] = std::pow(LUT[i], gamma) * factor + shift;
        LUT[i] = Correction::boundariesCheck(LUT[i]);
    }

    if (image->format() == QImage::Format_Indexed8)
    {
        for (int x=0; x<width; x++)
            for (int y=0; y<height; y++)
            {
                QRgb pixel = image->pixel(x,y);
                int v = qGray(pixel);
                newImage->setPixel(x, y, LUT[v]);
            }
    }
    else if (image->format() == QImage::Format_RGB32)
    {
        for (int x=0; x<width; x++)
            for (int y=0; y<height; y++)
            {
                QRgb pixel = image->pixel(x,y);

                int r = qRed(pixel);
                int g = qGreen(pixel);
                int b = qBlue(pixel);

                QColor newPixel = QColor(LUT[r], LUT[g], LUT[b]);
                newImage->setPixel(x, y, newPixel.rgb());
            }
    }

    return newImage;
}

const int Correction::boundariesCheck(int value){
    if(value < PIXEL_VAL_MIN ) { return PIXEL_VAL_MIN; }
    if(value > PIXEL_VAL_MAX) { return PIXEL_VAL_MAX; }
    return value;
}
