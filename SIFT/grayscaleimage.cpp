#include "grayscaleimage.h"
#include "utility.hpp"

#include <QImage>
#include <QColor>

#include <cstdlib>
#include <iostream>
using namespace std;

const GrayScalePixel GrayScaleImage::INVALID_VALUE = -1.0;
const GrayScalePixel GrayScaleImage::MAX_VALUE = 1.0;
const GrayScalePixel GrayScaleImage::MIN_VALUE = 0.0;

GrayScaleImage::GrayScaleImage():
        _width(0),
        _height(0),
        _data(0)
{
}

GrayScaleImage::GrayScaleImage(size_t w, size_t h):
        _width(w),
        _height(h),
        _data(new GrayScalePixel[w * h])
{
}

GrayScaleImage::GrayScaleImage(size_t w, size_t h, GrayScalePixel value):
        _width(w),
        _height(h),
        _data(new GrayScalePixel[w * h])
{
    for (size_t i=0;i<_width*_height;i++)
    {
        size_t offset = i;
        _data[offset] = value;
    }
}

GrayScaleImage::GrayScaleImage(GrayScalePixel* rawData, size_t w, size_t h, bool isRGBA):
        _width(w),
        _height(h),
        _data(new GrayScalePixel[w * h])
{
    if (isRGBA)
    {
        int size = w * h;
        for (int i=0;i<size;i++)
        {
            _data[i] = rawData[i * 4];
        }
    }
    else
    {
        memcpy(_data, rawData, sizeof(GrayScalePixel)*_width*_height);
    }
}


GrayScaleImage::GrayScaleImage(const GrayScaleImage& a):
        _width(a._width),
        _height(a._height),
        _data(new GrayScalePixel[_width * _height])
{
    memcpy(_data, a._data, sizeof(GrayScalePixel)*_width*_height);
}

GrayScaleImage& GrayScaleImage::operator=(const GrayScaleImage& img)
{
    if ( &img == this)
    {
        return (*this);
    }
    else
    {
        if (_data != 0)
            delete[] _data;

        _width = img.width();
        _height = img.height();
        _data = new GrayScalePixel[_width * _height];
        memcpy(_data, img.rawData(), sizeof(GrayScalePixel)*_width*_height);
        return *this;
    }
}

GrayScaleImage::GrayScaleImage(const std::string& filename):
        _width(0),
        _height(0),
        _data(0)
{
    loadImage(filename);
}

GrayScaleImage::~GrayScaleImage()
{
}

bool GrayScaleImage::loadImage(const string& filename)
{
    if(filename.size() <= 0)
	return false;
    
    QImage img(filename.c_str());
    _width = img.width();
    _height = img.height();
    _data = new GrayScalePixel[_width * _height];
    for (size_t y=0;y<_height;y++)
    {
        size_t rowOffset = y * _width;
        for (size_t x=0;x<_width;x++)
        {
            QRgb p = img.pixel(x, y);
            int r, g, b, a;
            Utils::interpretPixel(p, r, g, b, a);
            size_t pixelIdx = (x + rowOffset);
            GrayScalePixel value = Utils::convertToGrayScaleValue(r, g, b);
            _data[pixelIdx] = value / 255.0;
        }
    }
    return true;
}

bool GrayScaleImage::saveImage(const string& filename, bool needShift)
{
    if(filename.size() <= 0)
	return false;

    const double boostFactor = 2.0, shiftValue = 0.5;
    
    QImage img(_width,_height, QImage::Format_ARGB32);
    for (size_t y=0;y<_height;y++)
    {
        size_t rowOffset = y * _width;
        for (size_t x=0;x<_width;x++)
        {
            size_t pixelIdx = (x + rowOffset);
            int r, g, b, a;
            GrayScalePixel value = _data[pixelIdx];
            if(needShift)
                value = value * boostFactor + shiftValue;
            r = value * 255.0;
            g = value * 255.0;
            b = value * 255.0;
            a = 255.0;
            QRgb p = qRgba(r, g, b, a);
            img.setPixel(x, y, p);
        }
    }
    return img.save(filename.c_str());    
}

GrayScalePixel GrayScaleImage::getPixel(size_t x, size_t y)
{
    size_t yIdx = y % _height, xIdx = x % _width;

    size_t offset = yIdx * _width + xIdx;
    return _data[offset];
}

void GrayScaleImage::getNeighbor(size_t x, size_t y, size_t size, GrayScalePixel* pixels)
{
    if ( (size % 2) == 0 )
    {
	int halfSize = size / 2;
        for (int k=-halfSize;k<halfSize;k++)
        {
            int pixelY = y + k;
            for (int l=-halfSize;l<=halfSize;l++)
            {
                int idx = (k + halfSize) * size + (l + halfSize);
                int pixelX = x + l;
                pixels[idx] = getPixel(pixelX, pixelY);
            }
        }
    }
    else
    {
        int halfSize = (size -1) / 2;
        for (int k=-halfSize;k<=halfSize;k++)
        {
            int pixelY = y + k;
            for (int l=-halfSize;l<=halfSize;l++)
            {
                int idx = (k + halfSize) * size + (l + halfSize);
                int pixelX = x + l;
                pixels[idx] = getPixel(pixelX, pixelY);
            }
        }
    }
}

void GrayScaleImage::setPixel(size_t x, size_t y, const GrayScalePixel& value)
{
    size_t offset = y * _width + x;
    _data[offset] = value;
}
