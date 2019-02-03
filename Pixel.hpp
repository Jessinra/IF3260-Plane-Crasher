#ifndef POINT_HPP
#define POINT_HPP

#include <vector>
#define PI 3.14159265
using namespace std;

class Pixel
{
  private:
    float x;
    float y;
    unsigned int col;

  public:
    Pixel();
    Pixel(float, float);
    Pixel(float, float, unsigned int);

    float getX() const;
    float getY() const;
    unsigned int getColor() const;

    void setX(float);
    void setY(float);
    void setPos(float, float);
    void setColor(unsigned int);
    Pixel rotation(float, float, float);
    Pixel dilated(float, float, float);
};

#endif