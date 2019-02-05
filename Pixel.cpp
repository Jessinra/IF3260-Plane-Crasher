#include "Pixel.hpp"
#include <cmath>
#include <iostream>
#include "Color.hpp"

Pixel::Pixel()
{
    this->x = 0;
    this->y = 0;
    col = Color::BLACK;
}

Pixel::Pixel(float x, float y)
{
    this->x = x;
    this->y = y;
    col = Color::WHITE;
}

Pixel::Pixel(float x, float y, unsigned int color)
{
    this->x = x;
    this->y = y;
    col = color;
}

float Pixel::getX() const
{
    return this->x;
}

float Pixel::getY() const
{
    return this->y;
}

unsigned int Pixel::getColor() const
{
    return this->col;
}

void Pixel::setX(float x)
{
    this->x = x;
}

void Pixel::setY(float y)
{
    this->y = y;
}

void Pixel::setPos(float x, float y)
{
    this->x = x;
    this->y = y;
}

void Pixel::setColor(unsigned int color)
{
    this->col = color;
}

Pixel Pixel::rotation(float pointX, float pointY, float theta)
{
    int resultX = this->x;
    int resultY = this->y;

    resultX -= pointX;
    resultY -= pointY;

    float sinTheta = sin(theta * PI / 180);
    float cosTheta = cos(theta * PI / 180);

    float xnew = resultX * cosTheta - resultY * sinTheta;
    float ynew = resultX * sinTheta + resultY * cosTheta;

    resultX = xnew + pointX;
    resultY = ynew + pointY;

    return Pixel(resultX, resultY, col);
}

Pixel Pixel::dilated(float pointX, float pointY, float k)
{
    int resultX = k * (x - pointX) + pointX;
    int resultY = k * (y - pointY) + pointY;

    return Pixel(resultX, resultY, col);
}