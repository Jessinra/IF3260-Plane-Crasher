#include <iostream>
#include "master.hpp"
#include "Color.hpp"

using namespace std;

class Runner : public Master
{
  public:
    Runner(int h = 700, int w = 1000) : Master(h, w) {}
    void start()
    {
        int mul = 50;
        Pixel p1 = Pixel(000, 200, 0xFFFFFF);
        Pixel p2 = Pixel(100, 100, 0xFFFFFF);
        Pixel p3 = Pixel(200, 100, 0xFFFFFF);
        Pixel p4 = Pixel(400, 000, 0xFFFFFF);
        Pixel p5 = Pixel(600, 000, 0xFFFFFF);
        Pixel p6 = Pixel(400, 100, 0xFFFFFF);
        Pixel p7 = Pixel(700, 100, 0xFFFFFF);
        Pixel p8 = Pixel(800, 000, 0xFFFFFF);
        Pixel p9 = Pixel(900, 000, 0xFFFFFF);
        Pixel p10 = Pixel(800, 200, 0xFFFFFF);
        Pixel p11 = Pixel(400, 200, 0xFFFFFF);
        Pixel p12 = Pixel(600, 300, 0xFFFFFF);
        Pixel p13 = Pixel(400, 300, 0xFFFFFF);
        Pixel p14 = Pixel(200, 200, 0xFFFFFF);

        Line l1 = Line(p1, p2);
        Line l2 = Line(p2, p3);
        Line l3 = Line(p3, p4);
        Line l4 = Line(p4, p5);
        Line l5 = Line(p5, p6);
        Line l6 = Line(p6, p7);
        Line l7 = Line(p7, p8);
        Line l8 = Line(p8, p9);
        Line l9 = Line(p9, p10);
        Line l10 = Line(p10, p11);
        Line l11 = Line(p11, p12);
        Line l12 = Line(p12, p13);
        Line l13 = Line(p13, p14);
        Line l14 = Line(p14, p1);
        // for(const Pixel &x : l9.getRefPixelsVector()){
        //     std::cerr<<x.getX()<<"   "<<x.getY()<<" "<<x.getColor()<<endl;
        // }
        clearWindow();
        drawLine(0, 0, l1);
        drawLine(0, 0, l2);
        drawLine(0, 0, l3);
        drawLine(0, 0, l4);
        drawLine(0, 0, l5);
        drawLine(0, 0, l6);
        drawLine(0, 0, l7);
        drawLine(0, 0, l8);
        drawLine(0, 0, l9);
        drawLine(0, 0, l10);
        drawLine(0, 0, l11);
        drawLine(0, 0, l12);
        drawLine(0, 0, l13);
        drawLine(0, 0, l14);
    }
};

int main()
{
    Runner run;
    run.start();
    return 0;
}