#include "convolution.h"
#include <iostream>

/** Overloaded constructor */
Convolution::Convolution(PNM* img) :
    Transformation(img)
{
}

Convolution::Convolution(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

/** Returns a convoluted form of the image */
PNM* Convolution::transform()
{
    return convolute(getMask(3, Normalize), RepeatEdge);
}

/** Returns a sizeXsize matrix with the center point equal 1.0 */
math::matrix<float> Convolution::getMask(int size, Mode mode = Normalize)
{
    math::matrix<float> mask(size, size);

    for(int i = 0; i < size; i++){
       for(int j = 0; j < size; j++){
           mask(i,j) = 0.0;
       }
    }
    mask(floor(size / 2), floor(size / 2)) = 1.0;

    return mask;
}

/** Does the convolution process for all pixels using the given mask. */
PNM* Convolution::convolute(math::matrix<float> mask, Mode mode = RepeatEdge)
{
    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    float weight = sum(mask);

    if(image->format() == QImage::Format_Indexed8){
        for(int x = 0; x < width; x++){
            for(int y = 0; y < height; y++){
                math::matrix<float> okno = getWindow(x, y, mask.colno(), LChannel, mode);
                math::matrix<float> akumulator = join(okno, mask);
                float suma = sum(akumulator);

                if(weight > 0){
                    suma /= weight;
                }

                if(suma > PIXEL_VAL_MAX){
                    suma = PIXEL_VAL_MAX;
                }
                if(suma < PIXEL_VAL_MIN){
                    suma = PIXEL_VAL_MIN;
                }

                newImage->setPixel(x, y, suma);
            }
        }
    }else{
        for(int x = 0; x < width; x++){
            for(int y = 0; y < height; y++){
                math::matrix<float> oknoR = getWindow(x, y, mask.colno(), RChannel, mode);
                math::matrix<float> oknoG = getWindow(x, y, mask.colno(), GChannel, mode);
                math::matrix<float> oknoB = getWindow(x, y, mask.colno(), BChannel, mode);

                math::matrix<float> akumulatorR = join(oknoR, mask);
                math::matrix<float> akumulatorG = join(oknoG, mask);
                math::matrix<float> akumulatorB = join(oknoB, mask);

                float sumaR = sum(akumulatorR);
                float sumaG = sum(akumulatorG);
                float sumaB = sum(akumulatorB);

                if(weight > 0){
                    sumaR /= weight;
                    sumaG /= weight;
                    sumaB /= weight;
                }

                if(sumaR > PIXEL_VAL_MAX){
                    sumaR = PIXEL_VAL_MAX;
                }
                if(sumaR < PIXEL_VAL_MIN){
                    sumaR = PIXEL_VAL_MIN;
                }

                if(sumaG > PIXEL_VAL_MAX){
                    sumaG = PIXEL_VAL_MAX;
                }
                if(sumaG < PIXEL_VAL_MIN){
                    sumaG = PIXEL_VAL_MIN;
                }

                if(sumaB > PIXEL_VAL_MAX){
                    sumaB = PIXEL_VAL_MAX;
                }
                if(sumaB < PIXEL_VAL_MIN){
                    sumaB = PIXEL_VAL_MIN;
                }

                newImage->setPixel(x, y, qRgb(sumaR, sumaG, sumaB));
            }
        }
    }

    return newImage;
}

/** Joins to matrices by multiplying the A[i,j] with B[i,j].
  * Warning! Both Matrices must be squares with the same size!
  */
const math::matrix<float> Convolution::join(math::matrix<float> A, math::matrix<float> B)
{
    int size = A.rowno();
    math::matrix<float> C(size, size);

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            C(i, j) = A(i, j) * B(i, j);
        }
    }

    return C;
}

/** Sums all of the matrixes elements */
const float Convolution::sum(const math::matrix<float> A)
{
    float sum = 0.0;
    int size = A.rowno();

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            sum += A(i,j);
        }
    }

    return sum;
}


/** Returns reflected version of a matrix */
const math::matrix<float> Convolution::reflection(const math::matrix<float> A)
{
    int size = A.rowno();
    math::matrix<float> C(size, size);

    int srodek = floor(size/2);
    float tmp = 0;
    if( (srodek % 2) != 0){
        for(int w = 0; w <= srodek; w++){
            for(int k = 0; k <= srodek; k++){
                tmp = C[w][k];
                C[w][k] = C[2*srodek - w][2*srodek - k];
                C[2*srodek - w][2*srodek - k] = tmp;
            }
        }
    }else{
        for(int w = 0; w < srodek; w++){
            for(int k = 0; k < srodek; k++){
                tmp = C[w][k];
                C[w][k] = C[(2*srodek - 1) - w][(2*srodek - 1) - k];
                C[(2*srodek - 1) - w][(2*srodek - 1) - k] = tmp;
            }
        }
    }
    return C;
}
