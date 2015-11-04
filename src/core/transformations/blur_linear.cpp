#include "blur_linear.h"

BlurLinear::BlurLinear(PNM* img) :
    Convolution(img)
{
}

BlurLinear::BlurLinear(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* BlurLinear::transform()
{
    int maskSize = getParameter("size").toInt();
    QList<QVariant> tmpMask = getParameter("mask").toList();

    bool normalize = getParameter("normalize").toBool();

    math::matrix<float> mask(maskSize, maskSize);

    for(int i = 0; i < maskSize; i++){
        for(int j = 0; j < maskSize; j++){
            mask(i, j) = 1;
        }
    }
    mask(0, 0) = -3;
    mask(0, maskSize - 1) = 5;
    mask(maskSize - 1, 0) = -3;
    mask(maskSize - 1, maskSize - 1) = 5;

    mask = Convolution::reflection(mask);

    if(normalize){
        float sum = Convolution::sum(mask);

        for(int i = 0; i < maskSize; i++){
            for(int j = 0; j < maskSize; j++){
                mask(i, j) /= sum;
            }
        }
    }

    return convolute(mask, RepeatEdge);
}
