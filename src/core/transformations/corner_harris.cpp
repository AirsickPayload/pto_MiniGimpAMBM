#include "corner_harris.h"

#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"

CornerHarris::CornerHarris(PNM* img) :
    Convolution(img)
{
}

CornerHarris::CornerHarris(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* CornerHarris::transform()
{
    int    threshold    = getParameter("threshold").toInt();
    double sigma        = getParameter("sigma").toDouble(),
           sigma_weight = getParameter("sigma_weight").toDouble(),
           k_param      = getParameter("k").toDouble();

    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    math::matrix<double> imgX(width, height),
                         imgY(width, height),
                         imgXy(width, height),
                         candidates(width, height),
                         cornersuppr(width, height);

    PNM* grayImage = ConversionGrayscale(image).transform();

    BlurGaussian blurGauss(grayImage);
    blurGauss.setParameter("size", 3);
    blurGauss.setParameter("sigma", 3.6);
    grayImage = blurGauss.transform();

    EdgeSobel sobel(grayImage);

    math::matrix<float>* Gx = sobel.rawHorizontalDetection();
    math::matrix<float>* Gy = sobel.rawVerticalDetection();

    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            double dGx_point = (*Gx)(x, y),
                   dGy_point= (*Gy)(x, y);
            imgX(x, y) = dGx_point * dGx_point;
            imgY(x, y) = dGy_point * dGy_point;
            imgXy(x, y) = dGx_point * dGy_point;
        }
    }

    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1){
                candidates(x, y) = 0;
                cornersuppr(x, y) = 0;
            }
            else{
                double sx = 0, sy = 0, sxy = 0;
                for (int k = -1; k < 2; k++){
                    for (int j = -1; j < 2; j++){
                        double g_val = BlurGaussian::getGauss(k, j, sigma);
                        sx += imgX(x + k, y + j)*g_val;
                        sy += imgY(x + k, y + j)*g_val;
                        sxy += imgXy(x + k, y + j)*g_val;
                    }
                }
                sx /= sigma_weight;
                sy /= sigma_weight;
                sxy /= sigma_weight;
                math::matrix<double> H(2, 2);
                H(0, 0) = sx; H(1, 1) = sy; H(0, 1) = sxy; H(1, 0) = sxy;
                double r = H(0, 0)*H(1, 1) - H(0, 1)*H(1, 0) - k_param*pow(H(0, 0) + H(1, 1), 2);
                candidates(x, y) = (r > threshold) ? r : 0;
            }
        }
    }

    bool search = true;

    while (search){
        search = false;
        for (int x = 1; x < width - 1; x++){
            for (int y = 1; y < height - 1; y++){
                double val = candidates(x, y);
                if (val > candidates(x - 1, y - 1) && val > candidates(x - 1, y)
                                                   && val > candidates(x - 1, y + 1)
                                                   && val > candidates(x, y - 1)
                                                   && val > candidates(x, y + 1)
                                                   && val > candidates(x + 1, y - 1)
                                                   && val > candidates(x + 1, y)
                                                   && val > candidates(x + 1, y + 1)){
                    cornersuppr(x, y) = val;
                }
                else{
                    if (val > 0){
                        search = true;
                    }
                    cornersuppr(x, y) = 0;
                }
            }
        }
        candidates = cornersuppr;
    }

    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            if (candidates(x, y) == 0){
                newImage->setPixel(x, y, 0);
            }
            else{
                newImage->setPixel(x, y, 1);
            }
        }
    }

    return newImage;
}
