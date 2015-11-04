#include "blur_gaussian.h"

#define M_PI 3.14159265358979323846
#define M_E 2.71828182845904523536

BlurGaussian::BlurGaussian(PNM* img) :
    Convolution(img)
{
}

BlurGaussian::BlurGaussian(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* BlurGaussian::transform()
{
    emit message("Blurring...");

    int size = getParameter("size").toInt();
    radius = (size/2)+1;
    sigma = getParameter("sigma").toDouble();

    return convolute(getMask(size, Normalize), RepeatEdge);
}

math::matrix<float> BlurGaussian::getMask(int size, Mode)
{
    math::matrix<float> mask(size, size);

    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++)
        {
            mask(i, j) = getGauss(i / radius, j / radius, sigma);
        }
    }

    return mask;
}

float BlurGaussian::getGauss(int x, int y, float sigma)
{    
    return 1 / (2 * M_PI * sigma * sigma) * pow(M_E, -((x*x) + (y*y)) / (2*sigma*sigma));
}

