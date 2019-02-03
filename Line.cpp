#include "Line.hpp"

Line::Line(Pixel startPixel, Pixel endPixel)
{
    setStartPixel(startPixel);
    setEndPixel(endPixel);
}

void Line::setStartPixel(Pixel startPixel)
{
    this->startPixel = startPixel;
}

void Line::setEndPixel(Pixel endPixel)
{
    this->endPixel = endPixel;
}

Pixel Line::getStartPixel() const
{
    return this->startPixel;
}

Pixel Line::getEndPixel() const
{
    return this->endPixel;
}

const Pixel & Line::getRefStartPixel() const{
    return startPixel;
}

const Pixel & Line::getRefEndPixel() const{
    return endPixel;
}