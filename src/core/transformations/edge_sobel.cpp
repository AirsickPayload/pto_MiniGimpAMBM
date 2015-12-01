#include "edge_sobel.h"

EdgeSobel::EdgeSobel(PNM* img, ImageViewer* iv) :
    EdgeGradient(img, iv)
{
    prepareMatrices();
}

EdgeSobel::EdgeSobel(PNM* img) :
    EdgeGradient(img)
{
    prepareMatrices();
}

void EdgeSobel::prepareMatrices()
{
    math::matrix<float> gXmac(3, 3);
    math::matrix<float> gYmac(3, 3);

    for (int i = 0; i < 3; i++){
        gXmac(i, 1) = 0;
    }

    gXmac(0, 0) = -1;
    gXmac(0, 2) = 1;
    gXmac(1, 0) = -2;
    gXmac(1, 2) = 2;
    gXmac(2, 0) = -1;
    gXmac(2, 2) = 1;

    for (int i = 0; i < 3; i++){
        gYmac(1, i) = 0;
    }

    gYmac(0, 0) = -1;
    gYmac(0, 1) = -2;
    gYmac(0, 2) = -1;
    gYmac(2, 0) = 1;
    gYmac(2, 1) = 2;
    gYmac(2, 2) = 1;

    g_x = gXmac;
    g_y = gYmac;
}

math::matrix<float>* EdgeSobel::rawHorizontalDetection()
{
    math::matrix<float>* x_gradient = new math::matrix<float>(this->image->width(), this->image->height());

    int width = image->width();
    int height = image->height();

    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            math::matrix<float> okno = getWindow(x, y, 3, LChannel, RepeatEdge);
            (*x_gradient)(x, y) = sum(join(g_x, okno));
        }
    }

    return x_gradient;
}

math::matrix<float>* EdgeSobel::rawVerticalDetection()
{
    math::matrix<float>* y_gradient = new math::matrix<float>(this->image->width(), this->image->height());

    int width = image->width();
    int height = image->height();

    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            math::matrix<float> okno = getWindow(x, y, 3, LChannel, RepeatEdge);
            (*y_gradient)(x, y) = sum(join(g_y, okno));
        }
    }

    return y_gradient;
}
