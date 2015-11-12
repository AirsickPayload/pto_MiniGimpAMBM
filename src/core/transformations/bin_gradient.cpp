#include "bin_gradient.h"

BinarizationGradient::BinarizationGradient(PNM* img) :
    Transformation(img)
{
}

BinarizationGradient::BinarizationGradient(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationGradient::transform()
{
    int width = image->width();
    int height = image->height();
    int licznik = 0, mianownik = 0, Gx = 0, Gy = 0, G = 0, T = 0;

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            Mode mode = Transformation::RepeatEdge;
            QRgb pixel = qGray(getPixel(x, y, mode));

            Gx = qGray(getPixel(x + 1, y, mode)) - qGray(getPixel(x - 1, y, mode));
            Gy = qGray(getPixel(x, y + 1, mode)) - qGray(getPixel(x, y - 1, mode));

            if (Gx > Gy) G = Gx;
            else G = Gy;
            licznik += pixel * G;
            mianownik += G;
        }
    }

    T = licznik / mianownik;

    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            Mode mode = Transformation::RepeatEdge;
            QRgb pixel = getPixel(x, y, mode);
            int val = qGray(pixel);

            if (val < T)
                newImage->setPixel(x, y, 0);
            else
                newImage->setPixel(x, y, 1);
        }
    }


    return newImage;
}



