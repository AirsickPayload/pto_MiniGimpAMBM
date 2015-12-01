#include "edge_gradient.h"

EdgeGradient::EdgeGradient(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

EdgeGradient::EdgeGradient(PNM* img) :
    Convolution(img)
{
}

PNM* EdgeGradient::verticalDetection()
{
    return convolute(g_y, RepeatEdge);
}

PNM* EdgeGradient::horizontalDetection()
{
    return convolute(g_x, RepeatEdge);
}

PNM* EdgeGradient::transform()
{
    PNM* newImage = new PNM(image->width(), image->height(), image->format());

    int width = image->width();
    int height = image->height();

    PNM* ImageX = horizontalDetection();
    PNM* ImageY = verticalDetection();

    if (image->format() == QImage::Format_Indexed8){
        for (int x=0; x<width; x++){
            for (int y=0; y<height; y++){
                QRgb pixelX = ImageX -> pixel(x, y);
                QRgb pixelY = ImageY -> pixel(x, y);

                int warX = qGray(pixelX);
                int warY = qGray(pixelY);

                newImage->setPixel(x, y, sqrt(pow(warX, 2) + pow(warY, 2)));
            }
        }
    }else if(image->format() == QImage::Format_RGB32){
        for(int x = 0; x < width; x++){
            for(int y = 0; y < height; y++){
                QRgb pixelX = ImageX -> pixel(x, y);
                QRgb pixelY = ImageY -> pixel(x, y);

                int RX = qRed(pixelX);
                int RY = qRed(pixelY);

                int GX = qGreen(pixelX);
                int GY = qGreen(pixelY);

                int BX = qBlue(pixelX);
                int BY = qBlue(pixelY);

                int nR = sqrt(pow(RX, 2) + pow(RY, 2));
                int nG = sqrt(pow(GX, 2) + pow(GY,2));
                int nB = sqrt(pow(BX, 2) + pow(BY, 2));

                newImage -> setPixel(x, y, qRgb(nR, nG, nB));
            }
        }
    }

    return newImage;
}

