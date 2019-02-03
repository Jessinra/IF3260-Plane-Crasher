#include "objectFiller.hpp"

ObjectFiller::ObjectFiller() {
}

void ObjectFiller::insertionSort(EdgeTableTuple *ett) {
    int i, j;
    EdgeBucket temp;

    for (i = 1; i < ett->countEdgeBucket; i++) {
        temp.yMax = ett->buckets[i].yMax;
        temp.xMin = ett->buckets[i].xMin;
        temp.inverseGradient = ett->buckets[i].inverseGradient;
        j = i - 1;

        while ((temp.xMin < ett->buckets[j].xMin) && (j >= 0)) {
            ett->buckets[j + 1].yMax = ett->buckets[j].yMax;
            ett->buckets[j + 1].xMin = ett->buckets[j].xMin;
            ett->buckets[j + 1].inverseGradient = ett->buckets[j].inverseGradient;
            j = j - 1;
        }
        ett->buckets[j + 1].yMax = temp.yMax;
        ett->buckets[j + 1].xMin = temp.xMin;
        ett->buckets[j + 1].inverseGradient = temp.inverseGradient;
    }
}

void ObjectFiller::initEdgeTable() {
    for (int i = 0; i < maxHeight; i++) {
        EdgeTable[i].countEdgeBucket = 0;
    }

    ActiveEdgeTuple.countEdgeBucket = 0;
}

void ObjectFiller::insertLinesToEdgeTable(Object *object) {
    int x1, x2, y1, y2 for (Line &line : object->getLines()) {
        x1 = (int)line->getStartPixel().getX();
        x2 = (int)line->getEndPixel().getX();
        y1 = (int)line->getStartPixel().getY();
        y2 = (int)line->getEndPixel().getY();

        storeEdgeInTable(x1, y1, x2, y2);
    }
}

void ObjectFiller::storeEdgeInTuple(EdgeTableTuple *receiver, int yMax, int xMin, float inverseGradient) {
    // both used for edgetable and active edge table..
    // The edge tuple sorted in increasing yMax and x of the lower end.
    (receiver->buckets[(receiver)->countEdgeBucket]).yMax = yMax;
    (receiver->buckets[(receiver)->countEdgeBucket]).xMin = (float)xMin;
    (receiver->buckets[(receiver)->countEdgeBucket]).inverseGradient = inverseGradient;

    // sort the buckets
    insertionSort(receiver);

    (receiver->countEdgeBucket)++;
}

void ObjectFiller::storeEdgeInTable(int x1, int y1, int x2, int y2) {
    float gradient, inverseGradient;
    int yMax, xMin, yMin;

    // is Vertical Line
    if (x2 == x1) {
        inverseGradient = 0.000000;

    } else {
        gradient = ((float)(y2 - y1)) / ((float)(x2 - x1));

        // horizontal lines are not stored in edge table (no inverseGradient)
        if (y2 == y1)
            return;

        inverseGradient = (float)1.0 / gradient;
    }

    if (y1 > y2) {
        yMax = y1;
        yMin = y2;
        xMin = x2;

    } else {
        yMax = y2;
        yMin = y1;
        xMin = x1;
    }

    storeEdgeInTuple(&EdgeTable[yMin], yMax, xMin, inverseGradient);
}

void ObjectFiller::removeEdgeByYmax(EdgeTableTuple *Tuple, int currentY) {
    for (int i = 0; i < Tuple->countEdgeBucket; i++) {
        if (Tuple->buckets[i].yMax == currentY) {
            for (int j = i; j < Tuple->countEdgeBucket - 1; j++) {
                Tuple->buckets[j].yMax = Tuple->buckets[j + 1].yMax;
                Tuple->buckets[j].xMin = Tuple->buckets[j + 1].xMin;
                Tuple->buckets[j].inverseGradient = Tuple->buckets[j + 1].inverseGradient;
            }

            Tuple->countEdgeBucket--;
            i--;
        }
    }
}

void ObjectFiller::updateXMin(EdgeTableTuple *Tuple) {
    for (int i = 0; i < Tuple->countEdgeBucket; i++) {
        (Tuple->buckets[i]).xMin = (Tuple->buckets[i]).xMin + (Tuple->buckets[i]).inverseGradient;
    }
}

void ObjectFiller::fillObject(Object *object) {
    /* Follow the following rules: 
	1. Horizontal edges: Do not include in edge table 
	2. Horizontal edges: Drawn either on the bottom or on the top. 
	3. Vertices: If local max or min, then count twice, else count 
		once. 
	4. Either vertices at local minima or at local maxima are drawn.*/

    int x1, yMax1, x2, yMax2, FillFlag = 0, coordCount;
    Pixel *pixelStart, *pixelEnd;
    Line *filledLine;

    // Initialize EdgeTable
    insertLinesToEdgeTable(object);

    // Initialize drawing position
    int positionY = object->getPos().getY();
    int positionX = object->getPos().getX();

    // Start from yMin 0 Repeat until last yMin:
    for (int i = 0; i < maxHeight; i++)  //4. Increment y by 1 (next scan line)
    {
        // 1. Move from EdgeTuple bucket y to the
        // ActiveEdgeTuple those edges whose ymin = y (entering edges)
        for (int j = 0; j < EdgeTable[i].countEdgeBucket; j++) {
            storeEdgeInTuple(&ActiveEdgeTuple,
                             EdgeTable[i].buckets[j].yMax,
                             EdgeTable[i].buckets[j].xMin,
                             EdgeTable[i].buckets[j].inverseGradient);
        }

        // 2. Remove from ActiveEdgeTuple those edges for
        // which y=yMax (not involved in next scan line)
        removeEdgeByYmax(&ActiveEdgeTuple, i);

        //sort ActiveEdgeTuple (remember: EdgeTuple is presorted)
        insertionSort(&ActiveEdgeTuple);

        //3. Fill lines on scan line y by using pairs of x-coords from ActiveEdgeTuple
        FillFlag = 0;
        coordCount = 0;
        x1 = 0;
        x2 = 0;
        yMax1 = 0;
        yMax2 = 0;

        for (int k = 0; k < ActiveEdgeTuple.countEdgeBucket; k++) {
            if (coordCount % 2 == 0) {
                x1 = (int)(ActiveEdgeTuple.buckets[k].xMin);
                yMax1 = ActiveEdgeTuple.buckets[k].yMax;

                if (x1 == x2) {
                    /* three cases can arrive- 
					1. lines are towards top of the intersection 
					2. lines are towards bottom 
					3. one line is towards top and other is towards bottom 
				    */
                    if (((x1 == yMax1) && (x2 != yMax2)) || ((x1 != yMax1) && (x2 == yMax2))) {
                        x2 = x1;
                        yMax2 = yMax1;
                    }

                    else {
                        coordCount++;
                    }
                }

                else {
                    coordCount++;
                }

            } else {
                x2 = (int)ActiveEdgeTuple.buckets[k].xMin;
                yMax2 = ActiveEdgeTuple.buckets[k].yMax;

                FillFlag = 0;

                // checking for intersection...
                if (x1 == x2) {
                    /*three cases can arive- 
					1. lines are towards top of the intersection 
					2. lines are towards bottom 
					3. one line is towards top and other is towards bottom 
				    */

                    if (((x1 == yMax1) && (x2 != yMax2)) || ((x1 != yMax1) && (x2 == yMax2))) {
                        x1 = x2;
                        yMax1 = yMax2;

                    } else {
                        coordCount++;
                        FillFlag = 1;
                    }

                } else {
                    coordCount++;
                    FillFlag = 1;
                }

                if (FillFlag) {
                    //  draw horizontal line of solid line
                    // TODO: IMPLEMENT COLOR

                    pixelStart = new Pixel(x1, i);
                    pixelEnd = new Pixel(x2, i);
                    filledLine = new Line(pixelStart, pixelEnd);

                    drawLine(positionX, positionY, &filledLine);
                }
            }
        }

        // 5. For each nonvertical edge remaining in ActiveEdgeTuple, update x for new y
        updateXMin(&ActiveEdgeTuple);
    }
}

    /*
    // void ObjectFiller::printTuple(EdgeTableTuple *tup)
    // {
    // 	int j;

    // 	if (tup->countEdgeBucket)
    // 		printf("\nCount %d-----\n",tup->countEdgeBucket);

    // 		for (j=0; j<tup->countEdgeBucket; j++)
    // 		{
    // 			printf(" %d+%.2f+%.2f",
    // 			tup->buckets[j].yMax, tup->buckets[j].xMin,tup->buckets[j].inverseGradient);
    // 		}
    // }

    // void ObjectFiller::printTable()
    // {
    // 	int i,j;

    // 	for (i=0; i<maxHt; i++)
    // 	{
    // 		if (EdgeTable[i].countEdgeBucket)
    // 			printf("\nScanline %d", i);

    // 		printTuple(&EdgeTable[i]);
    // 	}
    // }
    */