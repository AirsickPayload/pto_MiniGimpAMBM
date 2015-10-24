#include "histogram_stretching.h"

#include "../histogram.h"

HistogramStretching::HistogramStretching(PNM* img) :
    Transformation(img)
{
}

HistogramStretching::HistogramStretching(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HistogramStretching::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    if (image->format() == QImage::Format_Indexed8)
        {
            QHash<int, int>* channel = image->getHistogram()->get(Histogram::LChannel);

            QList<int> list = channel->keys();

            double min = 255;
            double max = 0;

            for(int temp: list)
            {
                if(temp > max)
                    max = temp;
                if (temp < min)
                    min = temp;
            }

            for (int x=0; x<width; x++)
                for (int y=0; y<height; y++)
                {
                    QRgb pixel = image->pixel(x,y);

                    double val = qGray(pixel);
                    val = (255/(max - min)) * (val - min);

                    newImage->setPixel(x,y,val);
                }
        }
        else
        {
            QHash<int,int>* channelR = image->getHistogram()->get(Histogram::RChannel);
            QHash<int,int>* channelG = image->getHistogram()->get(Histogram::GChannel);
            QHash<int,int>* channelB = image->getHistogram()->get(Histogram::BChannel);

            QList<int> listR = channelR->keys();
            QList<int> listG = channelG->keys();
            QList<int> listB = channelB->keys();

            double minR = 255;
            double maxR = 0;
            double minG = 255;
            double maxG = 0;
            double minB = 255;
            double maxB = 0;

            for(int temp: listR)
            {
                if(temp > maxR)
                    maxR = temp;
                if(temp < minR)
                    minR = temp;
            }

            for(int temp : listG)
            {
                if(temp > maxG)
                    maxG = temp;
                if(temp < minG)
                    minG = temp;
            }

            for(int temp: listB)
            {
                if(temp > maxB)
                    maxB = temp;
                if(temp < minB)
                    minB = temp;
            }

            for (int x=0; x<width; x++)
                for (int y=0; y<height; y++)
                {
                    QRgb pixel = image->pixel(x,y);

                    double r = qRed(pixel);
                    double g = qGreen(pixel);
                    double b = qBlue(pixel);

                    r = (255.0/(maxR - minR)) * (r - minR);
                    g = (255.0/(maxG - minG)) * (g - minG);
                    b = (255.0/(maxB - minB)) * (b - minB);

                    newImage->setPixel(x,y, qRgb(r,g,b));
                }
        }

    return newImage;
}


