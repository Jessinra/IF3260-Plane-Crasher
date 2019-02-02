#ifndef MASTER_HPP
#define MASTER_HPP

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "Line.hpp"
#include "Object.hpp"
#include "Pixel.hpp"

class Master {
   protected:
    /* unchangeable */
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    int fbfd;
    int screensize;
    char *fbp;
    int xmultiplier, ymultiplier;
    int xadder, yadder;

    /* changeable */
    int xstart, xend;
    int ystart, yend;
    static int framejump;

    /* some unused attribute by end of refactoring #2 */
    // int height; Not used
    // int width; Not used
    // int offsetcycle; Not used

    void setDimension(int, int);
    void setFbfd();
    void setVinfo();
    void setFinfo();
    void setScreensize();
    void setFrameBufferPointer();
    void setXMultiplier();
    void setXAdder();
    void setYMultiplier();
    void setYAdder();

   public:
    Master(int, int);

    void displayVinfo();

    bool isInsideWindow(int, int);
    void assignColor(int, int, unsigned int);
    void copyColor(int, int, int, int);
    void clearWindow();
    void clearWindow(unsigned int);
    void moveWindowUp();

    void draw(int, int, int **, int, int);
    void draw(int, int, const vector<vector<int> > &);

    void drawPixel(int, int, const Pixel &);
    void drawPixels(int xStart, int yStart, Pixel *pixels);
    void drawLine(int, int, const Line &);
    void drawObject(const Object &);
};

#endif