#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Color.hpp"
#include "Line.hpp"
#include "Pixel.hpp"

class Object
{
    // Suatu object tersusun atas garix matrix of pixel

  protected:
    Pixel position;
    vector<Line> lines;
    int width;
    int height;

  public:
    Object();
    Object(float, float, std::string);

    bool outOfWindow(int, int) const;
    void reverseHorizontal();

    void setPos(Pixel); // change setPos to void
    void setPos(float, float);

    int getSingleColor() const;
    vector<Line> getLines() const;
    Pixel getPos() const;
    int getWidth() const;
    int getHeight() const;
    const vector<Line> &getRefLines() const;
    const Pixel &getRefPos() const;
};

#endif