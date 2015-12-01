#include "edge_laplacian.h"

EdgeLaplacian::EdgeLaplacian(PNM* img) :
    Convolution(img)
{
}

EdgeLaplacian::EdgeLaplacian(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

math::matrix<float> EdgeLaplacian::getMask(int, Mode)
{
    int size = getParameter("size").toInt();
    math::matrix<float> mask(size, size);

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            mask(i, j) = -1;
        }
    }

    int mid = floor(size / 2);
    mask(mid, mid) = pow(size, 2) - 1;

    return mask;
}

