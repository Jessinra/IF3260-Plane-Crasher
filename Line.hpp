#ifndef LINE_HPP
#define LINE_HPP

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "Pixel.hpp"
using namespace std;

class Line
{

  protected:
    Pixel startPixel;
    Pixel endPixel;

  public:
    Line(Pixel, Pixel);

    void setStartPixel(Pixel);
    void setEndPixel(Pixel);
    Pixel getStartPixel() const;
    Pixel getEndPixel() const;
    const Pixel &getRefStartPixel() const;
    const Pixel &getRefEndPixel() const;
};

#endif