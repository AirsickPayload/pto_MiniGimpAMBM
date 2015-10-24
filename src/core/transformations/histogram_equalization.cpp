#include "histogram_equalization.h"

#include "../histogram.h"

HistogramEqualization::HistogramEqualization(PNM* img) :
    Transformation(img)
{
}

HistogramEqualization::HistogramEqualization(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

float* countDistribuant(float distribuant[], int pixelCount[], int maxPixels) {
    for(int i=0; i<256; i++) {
        for(int j=0; j<i; j++) {
            distribuant[i] += pixelCount[j];
        }
        distribuant[i] /= maxPixels;
    }
    return distribuant;
}

PNM* HistogramEqualization::transform()
{
    int width  = image->width();
    int height = image->height();
    int maxPixels = width*height;
    int* pixelCount = new int[256];
    float* distribuant = new float[256];
    float* distribuantR = new float[256];
    float* distribuantG = new float[256];
    float* distribuantB = new float[256];

    PNM* newImage = new PNM(width, height, image->format());

    for (int i=0; i < 256; i++) {
        pixelCount[i]=0;
        distribuant[i]=0;
        distribuantR[i]=0;
        distribuantG[i]=0;
        distribuantB[i]=0;
    }

    if (image->format() == QImage::Format_Indexed8)
    {
        QHash<int, int>* tmp = image->getHistogram()->get(Histogram::LChannel);
        for(QHash<int, int>::const_iterator i = tmp->begin(); i != tmp->end(); i++)
        {
            pixelCount[i.key()] = i.value();
        }
        distribuant = countDistribuant(distribuant, pixelCount, maxPixels);

        for(int x=0; x < width; x++) {
            for(int y=0; y < height; y++) {
                QRgb pixel = image->pixel(x, y);

                newImage->setPixel(x,y, distribuant[qGray(pixel)]*255);
            }
        }
    }

    else if (image->format() == QImage::Format_RGB32)// RGB32
    {
        QHash<int, int> *tmpR = image->getHistogram()->get(Histogram::RChannel);
        for(QHash<int, int>::const_iterator k = tmpR->begin(); k != tmpR->end(); k++)
        {
            pixelCount[k.key()] = k.value();
        }

        distribuantR = countDistribuant(distribuantR, pixelCount, maxPixels);
        for (int i=0; i < 256; i++) pixelCount[i]=0;

        QHash<int, int> *tmpG = image->getHistogram()->get(Histogram::GChannel);
        for(QHash<int, int>::const_iterator k = tmpG->begin(); k != tmpG->end(); k++)
        {
            pixelCount[k.key()] = k.value();
        }

        distribuantG = countDistribuant(distribuantG, pixelCount, maxPixels);
        for (int i=0; i < 256; i++) pixelCount[i]=0;

        QHash<int, int> *tmpB = image->getHistogram()->get(Histogram::BChannel);
        for(QHash<int, int>::const_iterator k = tmpB->begin(); k != tmpB->end(); k++)
        {
            pixelCount[k.key()] = k.value();
        }

        distribuantB = countDistribuant(distribuantB, pixelCount, maxPixels);

        for(int x=0; x<width; x++) {
            for(int y=0; y<height; y++) {
                QRgb pixel = image->pixel(x, y);
                QColor newpixel = QColor(distribuantR[qRed(pixel)]*255, distribuantG[qGreen(pixel)]*255, distribuantB[qBlue(pixel)]*255);
                newImage->setPixel(x,y,newpixel.rgb());
            }

        }
    }
    return newImage;
}

