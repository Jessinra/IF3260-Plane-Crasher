
#include <math.h>
#include <stdio.h>

#include "Pixel.hpp"
#include "Line.hpp"
#include "Object.hpp"
#include "Master.hpp"

/* changable */
#define maxHeight 1080
#define maxWidth 1920
#define maxVertices 10000

// Start from lower left corner
class EdgeBucket {
   public:
    int yMax;    //max y-coordinate of edge
    float xMin;  //x-coordinate of lowest edge point updated only in aet
    float inverseGradient;
};

class EdgeTableTuple {
    // the array will give the scanline number
    // The edge table (ET) with edges entries sorted
    // in increasing y and x of the lower end
   public:
    int countEdgeBucket;              //no. of edgebuckets
    EdgeBucket buckets[maxVertices];  // max vertices (??)
};

class ObjectFiller {
   private:
    EdgeTableTuple EdgeTable[maxHeight];
    EdgeTableTuple ActiveEdgeTuple;

    /* Function to sort an array using insertion sort*/
    void insertionSort(EdgeTableTuple *ett);

    void initEdgeTable();
    void insertLinesToEdgeTable();
    void storeEdgeInTuple(EdgeTableTuple *receiver, int yMax, int xMin, float inverseGradient);
    void storeEdgeInTable(int x1, int y1, int x2, int y2);
    void removeEdgeByYmax(EdgeTableTuple *Tuple, int currentY);
    void updateXMin(EdgeTableTuple *Tuple);

   public:
    ObjectFiller();
    void fillObject(Object *);

    // void printTuple(EdgeTableTuple *tup)
    // void printTable()
};