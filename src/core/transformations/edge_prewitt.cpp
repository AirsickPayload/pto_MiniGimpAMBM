#include "edge_prewitt.h"

EdgePrewitt::EdgePrewitt(PNM*img) :
    EdgeGradient(img)
{
    prepareMatrices();
}

EdgePrewitt::EdgePrewitt(PNM*img, ImageViewer* iv) :
    EdgeGradient(img, iv)
{
    prepareMatrices();
}

void EdgePrewitt::prepareMatrices()
{
    math::matrix<float> gXmac(3, 3);
    math::matrix<float> gYmac(3, 3);

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if (j == 0){
                gXmac(i, j) = -1;
            }
            else if (j == 1){
                gXmac(i, j) = 0;
            }
            else if (j == 2){
                gXmac(i, j) = 1;
            }
        }
    }

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if (i == 0){
                gYmac(i, j) = -1;
            }
            else if (i == 1){
                gYmac(i, j) = 0;
            }
            else if (i == 2){
                gYmac(i, j) = 1;
            }
        }
    }
    g_x = gXmac;
    g_y = gYmac;
}

