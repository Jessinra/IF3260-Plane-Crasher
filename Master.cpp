
#include "Master.hpp"

int Master::framejump = 1;

Master::Master(int height, int width)
{
    setDimension(height, width);
    setFbfd();
    setVinfo();
    setFinfo();
    setScreensize();
    setFrameBufferPointer();
    setXMultiplier();
    setXAdder();
    setYMultiplier();
    setYAdder();

    displayVinfo();
}

void Master::setDimension(int height, int width)
{
    this->xstart = 0;
    this->ystart = 0;
    this->yend = height;
    this->xend = width;
}

void Master::setFbfd()
{
    // Open the file for reading and writing

    int fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd)
    {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }

    printf("The framebuffer device was opened successfully.\n");
    this->fbfd = fbfd;
}

void Master::setVinfo()
{
    // Get fixed screen information
    if (ioctl(this->fbfd, FBIOGET_FSCREENINFO, &this->finfo))
    {
        printf("Error reading fixed information.\n");
        exit(2);
    }
}

void Master::setFinfo()
{
    // Get variable screen information
    if (ioctl(this->fbfd, FBIOGET_VSCREENINFO, &this->vinfo))
    {
        printf("Error reading variable information.\n");
        exit(3);
    }
}

void Master::setScreensize()
{
    // Figure out the size of the screen in bytes
    this->screensize = this->vinfo.xres * this->vinfo.yres * this->vinfo.bits_per_pixel / 8;
}

void Master::setFrameBufferPointer()
{
    // Map the device to memory
    this->fbp = (char *)mmap(0, this->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, this->fbfd, 0);
    if ((long int)this->fbp == -1)
    {
        printf("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
}

void Master::setXAdder()
{
    this->xadder = (this->vinfo.xoffset) * this->xmultiplier;
}

void Master::setXMultiplier()
{
    this->xmultiplier = (this->vinfo.bits_per_pixel / 8);
}

void Master::setYAdder()
{
    this->yadder = this->vinfo.yoffset * this->finfo.line_length;
}

void Master::setYMultiplier()
{
    this->ymultiplier = this->finfo.line_length;
}

void Master::displayVinfo()
{
    printf("%dx%d, %dbpp\n", this->vinfo.xres, this->vinfo.yres, this->vinfo.bits_per_pixel);
}

bool Master::isInsideWindow(int x, int y)
{
    return x >= xstart && x < xend && y >= ystart && y < yend;
}

void Master::assignColor(int x, int y, unsigned int color)
{
    if (isInsideWindow(x, y))
    {
        int location = x * xmultiplier + xadder + y * ymultiplier + yadder;
        *((unsigned int *)(fbp + location)) = color;
    }
}

void Master::copyColor(int xTarget, int yTarget, int xSource, int ySource)
{
    if (isInsideWindow(xTarget, yTarget) && isInsideWindow(xSource, ySource))
    {
        int location1 = xTarget * xmultiplier + xadder + yTarget * ymultiplier + yadder;
        int location2 = xSource * xmultiplier + xadder + ySource * ymultiplier + yadder;
        *((unsigned int *)(fbp + location1)) = *((unsigned int *)fbp + location2);
    }
}
    
void Master::assignColorBuffer(vector<vector<unsigned int> > & buffer, int x, int y, unsigned int color){
    if (y < buffer.size() && x < buffer[0].size()){
        buffer[y][x] = color;
    }
}

void Master::clearWindow() { memset(fbp, 0, (yend * ymultiplier + yadder)); }

void Master::clearWindow(unsigned int color)
{
    for (int y = ystart; y < yend; y++)
    {
        for (int x = xstart; x < xend; x++)
        {
            assignColor(x, y, color);
        }
    }
}

void Master::moveWindowUp()
{
    for (int y = ystart; y < yend - framejump; y += framejump)
    {
        int location1 = xadder + y * ymultiplier + yadder;
        int location2 = location1 + framejump * ymultiplier;
        memcpy(fbp + location1, fbp + location2, framejump * (1 + vinfo.yoffset) * finfo.line_length);
    }
    memset(fbp + (yend - framejump) * ymultiplier + yadder, 0, framejump * (ymultiplier + yadder));
}

void Master::draw(int xStart, int yStart, int **img, int height, int width)
{
    for (int y = max(0, -yStart); y < height; y++)
    {
        if (y + yStart >= this->yend)
        {
            break;
        }

        for (int x = max(0, -xStart); x < width; x++)
        {
            if (x + xStart >= this->xend)
            {
                break;
            }

            assignColor(x + xStart, y + yStart, img[y][x]);
        }
    }
}

void Master::draw(int xStart, int yStart, const vector<vector<unsigned int>> &img)
{
    for (int y = max(0, -yStart); y < (int)img.size(); y++)
    {
        if (y + yStart >= this->yend)
        {
            break;
        }

        for (int x = max(0, -xStart); x < img[y].size(); x++)
        {
            if (x + xStart >= this->xend)
            {
                break;
            }

            assignColor(x + xStart, y + yStart, img[y][x]);
        }
    }
}

void Master::drawPixel(int xStart, int yStart, const Pixel &pixel)
{
    int x = xStart + pixel.getX();
    int y = yStart + pixel.getY();
    assignColor(x, y, pixel.getColor());
}

void Master::drawPixels(int xStart, int yStart, vector<Pixel> pixels)
{
    for (Pixel &p : pixels)
    {
        drawPixel(xStart, yStart, p);
    }
}

void Master::drawLine(int positionX, int positionY, const Line &line)
{
    // Bresenham's line algorithm with gradient coloring

    // Position section
    int xStart = line.getStartPixel().getX();
    int yStart = line.getStartPixel().getY();
    int xEnd = line.getEndPixel().getX();
    int yEnd = line.getEndPixel().getY();

    // Color section
    int colorStart = line.getStartPixel().getColor();
    int colorEnd = line.getEndPixel().getColor();

    // Setup Const
    const float deltaX = xEnd - xStart;
    const float deltaY = yEnd - yStart;

    const float deltaRed = ((colorEnd & 0xff0000) - (colorStart & 0xff0000)) >> 16;
    const float deltaGreen = ((colorEnd & 0xff00) - (colorStart & 0xff00)) >> 8;
    const float deltaBlue = ((colorEnd & 0xff) - (colorStart & 0xff));

    const float manhattanDist = fabs(deltaX) + fabs(deltaY) + 1;

    const float redStep = deltaRed / manhattanDist;
    const float greenStep = deltaGreen / manhattanDist;
    const float blueStep = deltaBlue / manhattanDist;

    const int xStep = deltaX >= 0 ? 1 : -1;
    const int yStep = deltaY >= 0 ? 1 : -1;

    float red = (colorStart & 0xff0000) >> 16;
    float green = (colorStart & 0xff00) >> 8;
    float blue = colorStart & 0xff;

    if (xStart == xEnd)
    {
        if (xStart + positionX >= this->xstart &&
            xStart + positionX < this->xend)
        {
            for (int y = yStart; y != yEnd + yStep; y += yStep)
            {
                unsigned int color = ((unsigned int)floor(red) << 16) + ((unsigned int)floor(green) << 8) + ((unsigned int)floor(blue));
                assignColor(positionX + xStart, positionY + y, color);

                red += redStep;
                green += greenStep;
                blue += blueStep;
            }
        }
        return;
    }

    int y = yStart;
    const float deltaErr = fabs(deltaY / deltaX);
    float error = 0;
    for (int x = xStart; x != xEnd + xStep;)
    {
        unsigned int color = ((unsigned int)floor(red) << 16) + ((unsigned int)floor(green) << 8) + ((unsigned int)floor(blue));
        assignColor(positionX + x, positionY + y, color);

        if (error >= 0.5)
        {
            y += yStep;
            error -= 1;
        }
        else
        {
            error += deltaErr;
            x += xStep;
            if (error >= 0.5)
            {
                y += yStep;
                error -= 1;
            }
        }

        red += redStep;
        green += greenStep;
        blue += blueStep;
    }
}


void Master::drawLine(vector<vector<unsigned int>> &buffer, const Line & line){
    // Bresenham's line algorithm with gradient coloring

    // Position section
    int xStart = line.getStartPixel().getX();
    int yStart = line.getStartPixel().getY();
    int xEnd = line.getEndPixel().getX();
    int yEnd = line.getEndPixel().getY();

    // Color section
    int colorStart = line.getStartPixel().getColor();
    int colorEnd = line.getEndPixel().getColor();

    // Setup Const
    const float deltaX = xEnd - xStart;
    const float deltaY = yEnd - yStart;

    const float deltaRed = ((colorEnd & 0xff0000) - (colorStart & 0xff0000)) >> 16;
    const float deltaGreen = ((colorEnd & 0xff00) - (colorStart & 0xff00)) >> 8;
    const float deltaBlue = ((colorEnd & 0xff) - (colorStart & 0xff));

    const float manhattanDist = fabs(deltaX) + fabs(deltaY) + 1;

    const float redStep = deltaRed / manhattanDist;
    const float greenStep = deltaGreen / manhattanDist;
    const float blueStep = deltaBlue / manhattanDist;

    const int xStep = deltaX >= 0 ? 1 : -1;
    const int yStep = deltaY >= 0 ? 1 : -1;

    float red = (colorStart & 0xff0000) >> 16;
    float green = (colorStart & 0xff00) >> 8;
    float blue = colorStart & 0xff;

    if (xStart == xEnd){
        if (xStart >= 0 && xStart < buffer.size()){
            for (int y = yStart; y != yEnd + yStep; y += yStep){
                unsigned int color = ((unsigned int)floor(red) << 16) + ((unsigned int)floor(green) << 8) + ((unsigned int)floor(blue));
                assignColorBuffer(buffer, xStart, y, color);

                red += redStep;
                green += greenStep;
                blue += blueStep;
            }
        }
        return;
    }

    if(deltaY <= deltaX){
        int y = yStart;
        const float deltaErr = fabs(deltaY / deltaX);
        float error = 0;
        for (int x = xStart; x != xEnd + xStep;x += xStep)
        {
            unsigned int color = ((unsigned int)floor(red) << 16) + ((unsigned int)floor(green) << 8) + ((unsigned int)floor(blue));
            assignColorBuffer(buffer, x, y, color);

            error += deltaErr;
            if (error >= 0.5){
                y += yStep;
                error -= 1;
            }

            red += redStep;
            green += greenStep;
            blue += blueStep;
        }
    }
    else{
        int x = xStart;
        const float deltaErr = fabs(deltaX / deltaY);
        float error = 0;
        for (int y = yStart; y != yEnd + yStep;y += yStep)
        {
            unsigned int color = ((unsigned int)floor(red) << 16) + ((unsigned int)floor(green) << 8) + ((unsigned int)floor(blue));
            assignColorBuffer(buffer, x, y, color);

            error += deltaErr;
            if (error >= 0.5){
                x += xStep;
                error -= 1;
            }

            red += redStep;
            green += greenStep;
            blue += blueStep;
        }
    }
}

void Master::drawObject(const Object &object)
{
    int positionX = object.getRefPos().getX();
    int positionY = object.getRefPos().getY();
    for (const Line &line : object.getRefLines())
    {
        drawLine(positionX, positionY, line);
    }
}

void Master::drawSolidObject(const Object &object)
{
    int positionX = object.getRefPos().getX();
    int positionY = object.getRefPos().getY();

    vector<Line> objectFillerLines = this->objectFiller.getObjectFillerLines(object);

    for (const Line &line : objectFillerLines)
    {
        drawLine(positionX, positionY, line);
    }
}

void Master::drawSolidObject2(const Object &obj){
    int h = obj.getHeight();
    int w = obj.getWidth();
    const unsigned int back = 0;
    unsigned int color = back;
    vector<vector<unsigned int> > vir(h, vector<unsigned int>(w, back));
    // draw line
    for(const Line &line : obj.getRefLines()){
        drawLine(vir, line);
    }

    // filling top to down
    for(int i=1;i<h-1;++i){
        color = back;
        for(int j=0;j<w;++j){
            if(vir[i][j] != back){
                /* Anomaly check start */
                // top check;
                bool top = false;
                for(int k=-1;k<=1;++k){
                    if(j+k >= 0 && j+k < w && vir[i-1][j+k] != back){
                        top = true;
                        break;
                    }
                }
                // botom check
                bool bot = false;
                for(int k=-1;k<=1;++k){
                    if(j+k >= 0 && j+k < w && vir[i+1][j+k] != back){
                        bot = true;
                        break;
                    }
                }
                /* Anomaly check end */
                if(top && bot){
                    color = color == back? vir[i][j] : back;
                }
            }
            else{
                vir[i][j] = color;
            }
        }
    }

    draw(obj.getRefPos().getX(), obj.getRefPos().getY(), vir);
}