#include "ObjectFiller.hpp"


EdgeBucket::EdgeBucket(){}
EdgeBucket::EdgeBucket(int _yMax, float _xMin, float _inverseGradient){
    yMax = _yMax;
    xMin = _xMin;
    inverseGradient = _inverseGradient;
}

EdgeTableTuple::EdgeTableTuple(){
}

ObjectFiller::ObjectFiller()
{
    initEdgeTable();
}

void ObjectFiller::insertionSort(EdgeTableTuple &ett)
{
    int i, j;
    EdgeBucket temp;

    for (i = 1; i < ett.buckets.size(); i++)
    {
        temp.yMax = ett.buckets[i].yMax;
        temp.xMin = ett.buckets[i].xMin;
        temp.inverseGradient = ett.buckets[i].inverseGradient;
        j = i - 1;

        while ((temp.xMin < ett.buckets[j].xMin) && (j >= 0))
        {
            ett.buckets[j + 1].yMax = ett.buckets[j].yMax;
            ett.buckets[j + 1].xMin = ett.buckets[j].xMin;
            ett.buckets[j + 1].inverseGradient = ett.buckets[j].inverseGradient;
            j = j - 1;
        }
        ett.buckets[j + 1].yMax = temp.yMax;
        ett.buckets[j + 1].xMin = temp.xMin;
        ett.buckets[j + 1].inverseGradient = temp.inverseGradient;
    }
}

void ObjectFiller::initEdgeTable()
{
    // for (int i = 0; i < maxHeight; i++)
    // {
    //     EdgeTable[i].countEdgeBucket = 0;
    // }

    // ActiveEdgeTuple.countEdgeBucket = 0;

    EdgeTable.clear();
    ActiveEdgeTuple = EdgeTableTuple();
}

void ObjectFiller::insertLinesToEdgeTable(const Object &object){
    int x1, x2, y1, y2;
    for(const Line &line : object.getRefLines()){
        x1 = (int)line.getStartPixel().getX();
        x2 = (int)line.getEndPixel().getX();
        y1 = (int)line.getStartPixel().getY();
        y2 = (int)line.getEndPixel().getY();

        storeEdgeInTable(x1, y1, x2, y2);
    }
}

void ObjectFiller::storeEdgeInTable(int x1, int y1, int x2, int y2)
{
    float gradient, inverseGradient;
    int yMax, xMin, yMin;

    // is Vertical Line
    if (x2 == x1){
        inverseGradient = 0.000000;
    }
    else{
        gradient = ((float)(y2 - y1)) / ((float)(x2 - x1));

        // horizontal lines are not stored in edge table (no inverseGradient)
        if (y2 == y1)
            return;

        inverseGradient = (float)1.0 / gradient;
    }

    if (y1 > y2){
        yMax = y1;
        yMin = y2;
        xMin = x2;
    }
    else{
        yMax = y2;
        yMin = y1;
        xMin = x1;
    }

    if(yMin < 0) std::cerr<<"anjing eh "<<yMin<<std::endl;
    storeEdgeInTuple(EdgeTable[yMin], yMax, xMin, inverseGradient);
}

void ObjectFiller::storeEdgeInTuple(EdgeTableTuple &receiver, int yMax, int xMin, float inverseGradient){
    // both used for edgetable and active edge table..
    // The edge tuple sorted in increasing yMax and x of the lower end.
    receiver.buckets.push_back(EdgeBucket(yMax, xMin, inverseGradient));

    // sort the buckets
    insertionSort(receiver);
}

void ObjectFiller::removeEdgeByYmax(EdgeTableTuple &Tuple, int currentY){
    for (int i = 0; i < Tuple.buckets.size(); i++){
        if (Tuple.buckets[i].yMax == currentY){
            vector<EdgeBucket> &tmp = Tuple.buckets;
            tmp.erase(tmp.begin() + i);
            i--;
        }
    }
}

void ObjectFiller::updateXMin(EdgeTableTuple &Tuple){
    for (int i = 0; i < Tuple.buckets.size(); i++){
        // (Tuple->buckets[i]).xMin = (Tuple->buckets[i]).xMin + (Tuple->buckets[i]).inverseGradient;
        Tuple.buckets[i].xMin += Tuple.buckets[i].inverseGradient;
    }
}

vector<Line> ObjectFiller::getObjectFillerLines(const Object &object)
{
    /* Follow the following rules: 
	1. Horizontal edges: Do not include in edge table 
	2. Horizontal edges: Drawn either on the bottom or on the top. 
	3. Vertices: If local max or min, then count twice, else count 
		once. 
	4. Either vertices at local minima or at local maxima are drawn.*/

    int x1, yMax1, x2, yMax2, FillFlag = 0, coordCount;
    vector<Line> lineToDraw;

    // Initialize EdgeTable
    initEdgeTable();
    EdgeTable.resize(object.getHeight());
    insertLinesToEdgeTable(object);

    // Get object color
    const int sampleColor = object.getSingleColor();

    // Initialize drawing position
    int positionY = object.getRefPos().getY();
    int positionX = object.getRefPos().getX();

    // Start from yMin 0 Repeat until last yMin:
    for (int i = 0; i < object.getHeight(); i++) //4. Increment y by 1 (next scan line)
    {
        // 1. Move from EdgeTuple bucket y to the
        // ActiveEdgeTuple those edges whose ymin = y (entering edges)
        for (int j = 0; j < EdgeTable[i].buckets.size(); j++)
        {
            storeEdgeInTuple(ActiveEdgeTuple,
                             EdgeTable[i].buckets[j].yMax,
                             EdgeTable[i].buckets[j].xMin,
                             EdgeTable[i].buckets[j].inverseGradient);
        }

        // 2. Remove from ActiveEdgeTuple those edges for
        // which y=yMax (not involved in next scan line)
        removeEdgeByYmax(ActiveEdgeTuple, i);

        //sort ActiveEdgeTuple (remember: EdgeTuple is presorted)
        insertionSort(ActiveEdgeTuple);

        //3. Fill lines on scan line y by using pairs of x-coords from ActiveEdgeTuple
        FillFlag = 0;
        coordCount = 0;
        x1 = -1;
        x2 = -1;
        yMax1 = 0;
        yMax2 = 0;

        for (int k = 0; k < ActiveEdgeTuple.buckets.size(); k++)
        {
            if (coordCount % 2 == 0)
            {
                x1 = (int)(ActiveEdgeTuple.buckets[k].xMin);
                yMax1 = ActiveEdgeTuple.buckets[k].yMax;

                if (x1 == x2)
                {
                    /* three cases can arrive- 
					1. lines are towards top of the intersection 
					2. lines are towards bottom 
					3. one line is towards top and other is towards bottom 
				    */
                    if (((x1 == yMax1) && (x2 != yMax2)) || ((x1 != yMax1) && (x2 == yMax2)))
                    {
                        x2 = x1;
                        yMax2 = yMax1;
                    }

                    else
                    {
                        coordCount++;
                    }
                }

                else
                {
                    coordCount++;
                }
            }
            else
            {
                x2 = (int)ActiveEdgeTuple.buckets[k].xMin;
                yMax2 = ActiveEdgeTuple.buckets[k].yMax;

                FillFlag = 0;

                // checking for intersection...
                if (x1 == x2)
                {
                    /*three cases can arive- 
					1. lines are towards top of the intersection 
					2. lines are towards bottom 
					3. one line is towards top and other is towards bottom 
				    */

                    if (((x1 == yMax1) && (x2 != yMax2)) || ((x1 != yMax1) && (x2 == yMax2)))
                    {
                        x1 = x2;
                        yMax1 = yMax2;
                    }
                    else
                    {
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else
                {
                    coordCount++;
                    FillFlag = 1;
                }

                if (FillFlag)
                {
                    //  draw horizontal line of solid line
                    Pixel pixelStart = Pixel(x1, i, sampleColor);
                    Pixel pixelEnd = Pixel(x2, i, sampleColor);

                    lineToDraw.push_back(Line(pixelStart, pixelEnd));
                }
            }
        }

        // 5. For each nonvertical edge remaining in ActiveEdgeTuple, update x for new y
        updateXMin(ActiveEdgeTuple);
    }
    return lineToDraw;
}