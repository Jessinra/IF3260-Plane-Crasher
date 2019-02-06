
#include "MoveableObject.hpp"
using namespace std;

MoveableObject::MoveableObject(float x, float y, const std::string &filename) : Object(x, y, filename)
{
    this->dx = 0;
    this->dy = 0;
    this->speed = 1;
}

MoveableObject::MoveableObject(float x, float y, float dx, float dy, float speed, const std::string &filename) : Object(x, y, filename)
{
    float distance = sqrt(dx * dx + dy * dy);
    this->dx = 0;
    this->dy = 0;

    if (distance > 0)
    {
        this->dx = dx / distance;
        this->dy = dy / distance;
    }

    this->speed = speed;
}

MoveableObject::MoveableObject(const Object &obj) : Object(obj)
{
    this->dx = 0;
    this->dy = 0;
    this->speed = 1;
}

MoveableObject::MoveableObject(float dx, float dy, float speed, const Object &obj) : Object(obj)
{
    float distance = sqrt(dx * dx + dy * dy);
    this->dx = 0;
    this->dy = 0;

    if (distance > 0)
    {
        this->dx = dx / distance;
        this->dy = dy / distance;
    }

    this->speed = speed;
}

void MoveableObject::setVector(float dx, float dy)
{
    float distance = sqrt(dx * dx + dy * dy);
    if (distance > 0)
    {
        this->dx = dx / distance;
        this->dy = dy / distance;
    }
}

void MoveableObject::move()
{
    this->position.setX(this->position.getX() + this->dx * this->speed);
    this->position.setY(this->position.getY() + this->dy * this->speed);
}

void MoveableObject::selfRotation(float x, float y, float theta)
{
    float yMin, yMax, xMin, xMax;
    bool alreadyloop = false;

    x -= this->position.getX();
    y -= this->position.getY();

    for (Line &line : lines)
    {
        line.setStartPixel(line.getStartPixel().rotation(x, y, theta));
        line.setEndPixel(line.getEndPixel().rotation(x, y, theta));

        if (alreadyloop)
        {
            xMin = min(xMin, min(line.getStartPixel().getX(), line.getEndPixel().getX()));
            xMax = max(xMax, max(line.getStartPixel().getX(), line.getEndPixel().getX()));
            yMin = min(yMin, min(line.getStartPixel().getY(), line.getEndPixel().getY()));
            yMax = max(yMax, max(line.getStartPixel().getY(), line.getEndPixel().getY()));
        }
        else
        {
            xMin = min(line.getStartPixel().getX(), line.getEndPixel().getX());
            xMax = max(line.getStartPixel().getX(), line.getEndPixel().getX());
            yMin = min(line.getStartPixel().getY(), line.getEndPixel().getY());
            yMax = max(line.getStartPixel().getY(), line.getEndPixel().getY());
        }
        alreadyloop = true;
    }

    for (Line &line : lines)
    {
        line.setStartPixel(Pixel(line.getStartPixel().getX() - xMin, line.getStartPixel().getY() - yMin, line.getStartPixel().getColor()));
        line.setEndPixel(Pixel(line.getEndPixel().getX() - xMin, line.getEndPixel().getY() - yMin, line.getEndPixel().getColor()));
    }

    xMax -= xMin;
    yMax -= yMin;

    x += this->position.getX();
    y += this->position.getY();

    this->position.setPos(this->position.getX() + xMin, this->position.getY() + yMin);

    width = xMax + 1;
    height = yMax + 1;
}

void MoveableObject::selfDilated(float x, float y, float k)
{
    float yMin, yMax, xMin, xMax;
    bool alreadyloop = false;

    for (Line &line : lines)
    {
        line.setStartPixel(line.getStartPixel().dilated(x, y, k));
        line.setEndPixel(line.getEndPixel().dilated(x, y, k));

        if (alreadyloop)
        {
            xMin = min(xMin, min(line.getStartPixel().getX(), line.getEndPixel().getX()));
            xMax = max(xMax, max(line.getStartPixel().getX(), line.getEndPixel().getX()));
            yMin = min(yMin, min(line.getStartPixel().getY(), line.getEndPixel().getY()));
            yMax = max(yMax, max(line.getStartPixel().getY(), line.getEndPixel().getY()));
        }
        else
        {
            xMin = min(line.getStartPixel().getX(), line.getEndPixel().getX());
            xMax = max(line.getStartPixel().getX(), line.getEndPixel().getX());
            yMin = min(line.getStartPixel().getY(), line.getEndPixel().getY());
            yMax = max(line.getStartPixel().getY(), line.getEndPixel().getY());
        }
        alreadyloop = true;
    }

    for (Line &line : lines)
    {
        line.setStartPixel(Pixel(line.getStartPixel().getX() - xMin, line.getStartPixel().getY() - yMin, line.getStartPixel().getColor()));
        line.setEndPixel(Pixel(line.getEndPixel().getX() - xMin, line.getEndPixel().getY() - yMin, line.getEndPixel().getColor()));
    }

    xMax -= xMin;
    yMax -= yMin;

    this->position.setPos(this->position.getX() + xMin, this->position.getY() + yMin);

    width = xMax + 1;
    height = yMax + 1;
}

float MoveableObject::getDx() const
{
    return this->dx;
}

float MoveableObject::getDy() const
{
    return this->dy;
}

float MoveableObject::getSpeed() const
{
    return this->speed;
}

void MoveableObject::setSpeed(float speed)
{
    this->speed = speed;
}