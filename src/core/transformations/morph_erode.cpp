#include "morph_erode.h"

MorphErode::MorphErode(PNM* img) :
    MorphologicalOperator(img)
{
}

MorphErode::MorphErode(PNM* img, ImageViewer* iv) :
    MorphologicalOperator(img, iv)
{
}

const int MorphErode::morph(math::matrix<float> window, math::matrix<bool> se)
{
    float max=0.0;

    for (int i = 0; i < window.rowno(); i++) {
        for (int j = 0; j < window.rowno(); j++) {
            if (se(i, j) == true && window(i, j) > max)
                max = window(i, j);
        }
    }

    return max;
}
