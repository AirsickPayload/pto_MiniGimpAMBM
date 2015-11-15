#include "noise_median.h"

NoiseMedian::NoiseMedian(PNM* img) :
    Convolution(img)
{
}

NoiseMedian::NoiseMedian(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseMedian::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    if(image->format() == QImage::Format_Indexed8){
        for(int x = 0; x < width; x++){
            for(int y  = 0; y < height; y++){
                newImage -> setPixel(x, y, getMedian(x, y, LChannel));
            }
        }
    } else if(image->format() == QImage::Format_RGB32){
            for(int x = 0; x < width; x++){
                for(int y = 0; y < height; y++){
                    newImage -> setPixel(x, y, qRgb(getMedian(x, y, RChannel),
                                                    getMedian(x, y, GChannel),
                                                    getMedian(x, y, BChannel)));
                }
            }
    }
    return newImage;
}

int NoiseMedian::getMedian(int x, int y, Channel channel)
{
    int radius = getParameter("radius").toInt();
    int size = radius * 2  + 1;
    math::matrix<float> okno = getWindow(x, y, size, channel, RepeatEdge);
    float* pomTabObl = new float[size * size];
    int l = 0;

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            pomTabObl[l] = okno(i, j);
            l++;
        }
    }

    //Sortowanie przed pobraniem mediany
    std::sort(pomTabObl, pomTabObl + size * size);

    return (int)pomTabObl[(int)(floor(size * size) / 2)];
}
