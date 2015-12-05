#include "map_normal.h"

#include "edge_sobel.h"
#include "map_height.h"

MapNormal::MapNormal(PNM* img) :
    Convolution(img)
{
}

MapNormal::MapNormal(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* MapNormal::transform()
{
    int width  = image->width(),
        height = image->height();

    double strength = getParameter("strength").toDouble();

    PNM* newImage = new PNM(width, height, image->format());

    MapHeight* map = new MapHeight(image);

    image = map->transform();

    delete map;

    EdgeSobel* sobel = new EdgeSobel(image);

    math::matrix<float>* G_x = sobel->rawHorizontalDetection();
    math::matrix<float>* G_y = sobel->rawVerticalDetection();

    delete sobel;

    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++)
        {
            double dX = (*G_x)(i, j) / PIXEL_VAL_MAX;
            double dY = (*G_y)(i, j) / PIXEL_VAL_MAX;
            double dZ = 1 / strength;
            double dlWektora = sqrt(dX*dX + dY*dY + dZ*dZ);
            dX = dX / dlWektora;
            dY = dY / dlWektora;
            dZ = dZ / dlWektora;

            dX = (dX + 1.0)*(255/strength);
            dY = (dY + 1.0)*(255/strength);
            dZ = (dZ + 1.0)*(255/strength);

            newImage->setPixel(i, j, qRgb(dX, dY, dZ));
        }
    return newImage;
}
