#include "edge_canny.h"
#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"

math::matrix<float> m_m;
math::matrix<float> m_o;
int kierunek[361];

EdgeCanny::EdgeCanny(PNM* img) :
    Convolution(img)
{
}

EdgeCanny::EdgeCanny(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* EdgeCanny::transform()
{
    int width  = image->width(),
        height = image->height();

    int upper_thresh = getParameter("upper_threshold").toInt(),
        lower_thresh = getParameter("lower_threshold").toInt();

    PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);

    ConversionGrayscale* grayscale = new ConversionGrayscale(image);
    image = grayscale->transform();
    delete grayscale;

    BlurGaussian* gaussianBlur = new BlurGaussian(image);
    gaussianBlur->setParameter("size", 3);
    gaussianBlur->setParameter("sigma", 1.6);
    image = gaussianBlur->transform();
    delete gaussianBlur;

    EdgeSobel* sobel = new EdgeSobel(image);
    math::matrix<float>* G_x = sobel->rawHorizontalDetection(),
                       * G_y = sobel->rawVerticalDetection();
    delete sobel;

    m_o = math::matrix<float>(image->width(), image->height());
    m_m = math::matrix<float>(image->width(), image->height());

     // inicjalizacja tablicy 'podglądu' dla możliwych kierunków gradientów pikseli
    for (int i = 0; i < 361; i++){
        if ((i >= 0 && i < 23) || (i >= 158 && i < 203) || (i >= 338 && i < 361)) {
            kierunek[i] = 0;
        } else if ((i >= 23 && i < 68) || (i >= 203 && i < 248)) {
            kierunek[i] = 1;
        } else if ((i >= 68 && i < 113) || (i >= 248 && i < 293)) {
            kierunek[i] = 2;
        } else if ((i >= 113 && i < 158) || (i >= 293 && i < 338)) {
            kierunek[i] = 3;
        }
    }

    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            newImage->setPixel(x, y, PIXEL_VAL_MIN);
            m_m(x, y) = sqrt(pow((*G_x)(x, y), 2) + pow((*G_y)(x, y), 2));
            if ((*G_x)(x, y) != 0)
               m_o(x,y) = atan(1) * 180 / 3.14;
            else
               m_o(x,y) = 0;
        }
    }

    float ne1, ne2;
    std::list<QPoint> initialPoints;
    for (int i = 1; i < width - 1; i++){
        for (int j = 1; j < height - 1; j++){
            int pozycja = m_o(i, j);
            float m2 = m_m(i, j);
            int pktINT1 = 1;
            QPoint pktQ1;
            if (pktINT1 == 1){
                if (kierunek[pozycja] == 0){
                    pktQ1 = QPoint(0, -1);
                }else if (kierunek[pozycja] == 1){
                    pktQ1 = QPoint(1, -1);
                }else if (kierunek[pozycja] == 2){
                    pktQ1 = QPoint(-1, 0);
                }else if (kierunek[pozycja] == 3){
                    pktQ1 = QPoint(-1, -1);
                }
            }else{
                if (kierunek[pozycja] == 0){
                    pktQ1 = QPoint(0, 1);
                }else if (kierunek[pozycja] == 1){
                    pktQ1 = QPoint(-1, 1);
                }else if (kierunek[pozycja] == 2){
                    pktQ1 = QPoint(1, 0);
                }else if (kierunek[pozycja] == 3){
                    pktQ1 = QPoint(1, 1);
                }
            }
            QPoint pktQ2;
            int pktINT2 = 2;
            if (pktINT2 == 1){
                if (kierunek[pozycja] == 0){
                    pktQ2 = QPoint(0, -1);
                }else if (kierunek[pozycja] == 1){
                    pktQ2 = QPoint(1, -1);
                }else if (kierunek[pozycja] == 2){
                    pktQ2 = QPoint(-1, 0);
                }else if (kierunek[pozycja] == 3){
                    pktQ2 = QPoint(-1, -1);
                }
            }else{
                if (kierunek[pozycja] == 0){
                    pktQ2 = QPoint(0, 1);
                }else if (kierunek[pozycja] == 1){
                    pktQ2 = QPoint(-1, 1);
                }else if (kierunek[pozycja] == 2){
                    pktQ2 = QPoint(1, 0);
                }else if (kierunek[pozycja] == 3){
                    pktQ2 = QPoint(1, 1);
                }
            }
            ne1 = m_m(i + pktQ1.x(), j + pktQ1.y());
            ne2 = m_m(i + pktQ2.x(), j + pktQ2.y());

            if (m2 > ne1 && m2 > ne2 && m2 > upper_thresh){
                newImage->setPixel(i, j, PIXEL_VAL_MAX);
                initialPoints.push_back(QPoint(i, j));
            }
            }
        }

        // histereza
        while (!initialPoints.empty()){
            QPoint point = initialPoints.back();
            initialPoints.pop_back();
            int pozycja = m_o(point.x(), point.y());
            QPoint tneipoin[7] = {
                QPoint(-1, 0),
                QPoint(-1, 1),
                QPoint(0, -1),
                QPoint(0, 1),
                QPoint(1, -1),
                QPoint(1, 0),
                QPoint(1, 1),
            };
            for (int i = 1; i <= 2; i++){
                QPoint gDir;
                if (i == 1){
                    if (kierunek[pozycja] == 0){
                        gDir = QPoint(0, -1);
                    }else if (kierunek[pozycja] == 1){
                        gDir = QPoint(1, -1);
                    }else if (kierunek[pozycja] == 2){
                        gDir = QPoint(-1, 0);
                    }else if (kierunek[pozycja] == 3){
                        gDir = QPoint(-1, -1);
                    }
                }else{
                    if (kierunek[pozycja] == 0){
                        gDir = QPoint(0, 1);
                    }
                    else if (kierunek[pozycja] == 1){
                        gDir = QPoint(-1, 1);
                    }
                    else if (kierunek[pozycja] == 2){
                        gDir = QPoint(1, 0);
                    }
                    else if (kierunek[pozycja] == 3){
                        gDir = QPoint(1, 1);
                    }
                }
                QPoint currPoint = point;
                while (true){
                    QPoint newPoint = QPoint(currPoint.x() + gDir.x(), currPoint.y() + gDir.y());
                    if (newPoint.x() == width - 1 || newPoint.x() == 0 || newPoint.y() == height - 1 || newPoint.y() == 0) break;
                    if (newImage->pixel(newPoint.x(), newPoint.y()) == PIXEL_VAL_MAX) break;

                    if (m_m(newPoint.x(), newPoint.y()) < lower_thresh) break;

                    if (kierunek[(int)m_o(newPoint.x(), newPoint.y())] != kierunek[pozycja]) break;
                    bool maxM = true;
                    double mag = m_m(newPoint.x(), newPoint.y());
                    for (int i = 0; i < 7; i++){
                        QPoint tnei(newPoint.x() + tneipoin[i].x(), newPoint.y() + tneipoin[i].y());
                        if (tnei.x() == currPoint.x() && tnei.y() == currPoint.y()) continue;
                        if (kierunek[(int)m_o(tnei.x(), tnei.y())] != kierunek[pozycja]) continue;
                        if (m_m(tnei.x(), tnei.y()) >= mag){
                            maxM = false;
                            break;
                        }
                    }
                    if (!maxM) break;
                    newImage->setPixel(newPoint.x(), newPoint.y(), PIXEL_VAL_MAX);
                    currPoint = newPoint;
                }
            }
        }

    return newImage;
}
