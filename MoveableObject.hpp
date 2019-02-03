#ifndef MOVEABLEOBJECT_HPP
#define MOVEABLEOBJECT_HPP

#include <iostream>

#include "Object.hpp"

class MoveableObject : public Object
{
  protected:
    float speed;
    float dx, dy;

  public:
    MoveableObject(float, float, const std::string &);
    MoveableObject(float, float, float, float, float, const std::string &);
    MoveableObject(const Object &);
    MoveableObject(float, float, float, const Object &);

    void move();
    void selfRotation(float, float, float);
    void selfDilated(float, float, float);

    void setVector(float, float);
    void setSpeed(float);

    float getDx() const;
    float getDy() const;
    float getSpeed() const;
};

#endif