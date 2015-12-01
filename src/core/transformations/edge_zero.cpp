#include "edge_zero.h"

#include "edge_laplacian_of_gauss.h"

EdgeZeroCrossing::EdgeZeroCrossing(PNM* img) :
    Convolution(img)
{
}

EdgeZeroCrossing::EdgeZeroCrossing(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* EdgeZeroCrossing::transform()
{
    int width = image->width(),
        height = image->height();

    int    size  = getParameter("size").toInt();
    double sigma = getParameter("sigma").toDouble();
    int    t     = getParameter("threshold").toInt();
    int    v_0   = 128;

    PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);

    EdgeLaplaceOfGauss edge(image);
    edge.setParameter("size", size);
    edge.setParameter("sigma", sigma);
    Transformation laplace(edge.transform());

    math::matrix<float> okno(size, size);

    for(int x = 0 ; x < width; x++){
        for(int y = 0; y < height; y++){
            okno = laplace.getWindow(x, y, size, LChannel, RepeatEdge);
            float min = PIXEL_VAL_MAX;
            float max = PIXEL_VAL_MIN;

            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    if (okno(i, j) > max){
                        max = okno(i, j);
                    }
                    if (okno(i, j) < min){
                        min = okno(i, j);
                    }
                }
            }
            if (min < (v_0 - t) && max > (v_0 + t)){
                newImage->setPixel(x, y, okno(size / 2,  size / 2));
            }
            else{
                newImage->setPixel(x, y, 0);
            }
        }
    }

     return newImage;
}
