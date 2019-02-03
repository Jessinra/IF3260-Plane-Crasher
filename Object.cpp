
#include "Object.hpp"

Object::Object()
{
}

Object::Object(float x, float y, std::string filename)
{
    setPos(x, y);

    // initialize lines
    ifstream inFile;
    inFile.open(filename);

    if (!inFile)
    {
        cout << "\nError opening file.\n";
        exit(1);
    }

    int nline;
    int xStart, yStart, xEnd, yEnd;
    int xMin, xMax, yMin, yMax;
    unsigned int colorStart, colorEnd;

    inFile >> nline;
    for (int i = 0; i < nline; ++i)
    {
        inFile >> dec >> xStart;
        inFile >> yStart;
        inFile >> hex >> colorStart;
        inFile >> dec >> xEnd;
        inFile >> yEnd;
        inFile >> hex >> colorEnd;

        if (i == 0)
        {
            xMin = min(xStart, xEnd);
            xMax = max(xStart, xEnd);
            yMin = min(yStart, yEnd);
            yMax = max(yStart, yEnd);
        }
        else
        {
            xMin = min(yMin, min(xStart, xEnd));
            xMax = max(xMax, max(xStart, xEnd));
            yMin = min(yMin, min(yStart, yEnd));
            yMax = max(yMax, max(yStart, yEnd));
        }

        Pixel startpx = Pixel(xStart, yStart, colorStart);
        Pixel endpx = Pixel(xEnd, yEnd, colorEnd);
        Line line = Line(startpx, endpx);
        lines.push_back(line);
    }

    height = yMax + 1;
    width = xMax + 1;
    inFile.close();
}

bool Object::outOfWindow(int h, int w) const
{
    return (this->position.getX() >= w || this->position.getY() >= h || this->position.getX() <= -width || this->position.getY() <= -height);
}

void Object::reverseHorizontal(){
    for(Line &line : lines){
        line.setStartPixel(Pixel(width - line.getStartPixel().getX() - 1, line.getStartPixel().getY(), line.getStartPixel().getColor()));
        line.setEndPixel(Pixel(width - line.getEndPixel().getX() - 1, line.getEndPixel().getY(), line.getEndPixel().getColor()));
    }
}

void Object::setPos(Pixel position)
{
    this->position = position;
}

void Object::setPos(float x, float y)
{
    this->position = Pixel(x, y);
}

int Object::getSingleColor() const
{
    return this->getLines()[0].getStartPixel().getColor();
}

vector<Line> Object::getLines() const
{
    return lines;
}

int Object::getWidth() const
{
    return width;
}

int Object::getHeight() const
{
    return height;
}

Pixel Object::getPos() const
{
    return this->position;
}

const vector<Line> &Object::getRefLines() const
{
    return lines;
}

const Pixel &Object::getRefPos() const
{
    return this->position;
}