#include "edge_roberts.h"

EdgeRoberts::EdgeRoberts(PNM* img) :
    EdgeGradient(img)
{
    prepareMatrices();
}

EdgeRoberts::EdgeRoberts(PNM* img, ImageViewer* iv) :
    EdgeGradient(img, iv)
{
    prepareMatrices();
}

void EdgeRoberts::prepareMatrices()
{
    math::matrix<float> gXmac(2, 2);
    math::matrix<float> gYmac(2, 2);

    gXmac(0, 0) = 1;
    gXmac(0, 1) = 0;
    gXmac(1, 0) = 0;
    gXmac(1, 1) = -1;

    gYmac(0, 0) = 0;
    gYmac(0, 1) = 1;
    gYmac(1, 0) = -1;
    gYmac(1, 1) = 0;

    g_x = gXmac;
    g_y = gYmac;
}
