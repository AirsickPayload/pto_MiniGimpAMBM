#include "morphological_operator.h"

MorphologicalOperator::MorphologicalOperator(PNM* img) :
    Transformation(img)
{
}

MorphologicalOperator::MorphologicalOperator(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

// abstract
const int MorphologicalOperator::morph(math::matrix<float>, math::matrix<bool>)
{
    return 0;
}

math::matrix<bool> MorphologicalOperator::getSE(int size, SE shape)
{
    switch (shape)
    {
    case Square:    return seSquare(size);
    case Cross:     return seCross(size);
    case XCross:    return seXCross(size);
    case VLine:     return seVLine(size);
    case HLine:     return seHLine(size);
    default:        return seSquare(size);
    }
}


math::matrix<bool> MorphologicalOperator::seSquare(int size)
{
    math::matrix<bool> ret(size, size);

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            ret(i, j) = true;

    return ret;
}

math::matrix<bool> MorphologicalOperator::seCross(int size)
{
    math::matrix<bool> ret(size, size);

    int center = size / 2;

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (i == center || j == center) ret(i, j) = true;
            else ret(i, j) = false;

    return ret;
}

math::matrix<bool> MorphologicalOperator::seXCross(int size)
{
    math::matrix<bool> ret(size, size);

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (i == j || i + j == size) ret(i, j) = true;
            else ret(i, j) = false;

    return ret;
}

math::matrix<bool> MorphologicalOperator::seVLine(int size)
{
    math::matrix<bool> ret(size, size);

    int center = size / 2;

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (j == center) ret(i, j) = true;
            else ret(i, j) = false;

    return ret;
}

math::matrix<bool> MorphologicalOperator::seHLine(int size)
{
    math::matrix<bool> ret(size, size);

    int center = size / 2;

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (i == center) ret(i, j) = true;
            else ret(i, j) = false;

    return ret;
}

PNM* MorphologicalOperator::transform()
{  
    int size  = getParameter("size").toInt();
    SE  shape = (MorphologicalOperator::SE) getParameter("shape").toInt();

    PNM* newImage = new PNM(image->width(), image->height(), QImage::Format_RGB32);


    int width = image->width();
    int height = image->height();

    math::matrix<bool> elementStrukturyzujacy = getSE(size, shape);


    for(int x = 0; x < width; x++)
        for(int y = 0; y < height; y++)
        {
            math::matrix<double> windowR = getWindow(x, y, size, RChannel, RepeatEdge);
            math::matrix<double> windowG = getWindow(x, y, size, GChannel, RepeatEdge);
            math::matrix<double> windowB = getWindow(x, y, size, BChannel, RepeatEdge);

            newImage->setPixel(x, y, qRgb(morph(windowR, elementStrukturyzujacy), morph(windowG, elementStrukturyzujacy), morph(windowB, elementStrukturyzujacy)));
        }

    return newImage;
}
