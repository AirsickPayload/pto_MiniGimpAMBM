#include "hough.h"

#include "conversion_grayscale.h"
#include "edge_laplacian.h"

Hough::Hough(PNM* img) :
    Transformation(img)
{
}

Hough::Hough(PNM* img, ImageViewer* super) :
    Transformation(img, super)
{
}

PNM* Hough::transform()
{
    int width  = image->width(), height = image->height();
    int thetaDensity = getParameter("theta_density").toInt();
    int rhoMax       = 0;
    int thetaSize    = 0;
    int v            = 0;
    double theta     = 0;
    double rho       = 0;
    double max       = 0;

    ConversionGrayscale* grayscale = new ConversionGrayscale(image);
    image = grayscale->transform();
    delete grayscale;

    if(getParameter("skip_edge_detection").toBool() != true) {
        EdgeLaplacian* edgeLaplacian = new EdgeLaplacian(image);
        edgeLaplacian->setParameter("size", 3);
        image = edgeLaplacian->transform();
        delete edgeLaplacian;
    }

    rhoMax = sqrt((width*width) + (height*height));
    thetaSize = thetaDensity * 180;

    PNM* newImage = new PNM(thetaSize, rhoMax * 2 + 1, QImage::Format_Indexed8);
    math::matrix<float> hough(thetaSize, rhoMax * 2 + 1);

    for (int i = 0; i < thetaSize; i++) {
        for (int j = 0; j < rhoMax * 2 + 1; j++) {
            hough(i, j) = 0;
        }
    }

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            QRgb pixel = image->pixel(i, j);
            v = qGray(pixel);

            if (v > 0) {
                for (int k = 0; k < thetaSize; k++) {
                    theta = (k*M_PI)/(180.0 * thetaDensity);
                    rho = round(i*cos(theta) + j*sin(theta));
                    hough(k, rho + rhoMax)++;
                }
            }
        }
    }

    for (int i = 0; i < thetaSize; i++) {
        for (int j = 0; j < rhoMax * 2 + 1; j++) {
            if (hough(i, j) > max)
                max = hough(i, j);
        }
    }

    for (int x = 0; x < thetaSize; x++) {
        for (int y = 0; y < rhoMax * 2 + 1; y++) {
            newImage->setPixel(x, y, round(255*hough(x, y)/max));
        }
    }
    return newImage;
}
