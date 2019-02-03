#ifndef OBJECT_FILLER_HPP
#define OBJECT_FILLER_HPP

#include <math.h>
#include <stdio.h>

#include "Line.hpp"
#include "Object.hpp"
#include "Pixel.hpp"

/* changable */
#define maxHeight 1080
#define maxVertices 10000

// Start from lower left corner
class EdgeBucket
{
public:
  int yMax;   // max y-coordinate of edge
  float xMin; // x-coordinate of lowest edge point updated only in aet
  float inverseGradient;
};

class EdgeTableTuple
{
  // the array will give the scanline number
  // The edge table (ET) with edges entries sorted
  // in increasing y and x of the lower end
public:
  int countEdgeBucket;             // no. of edgebuckets
  EdgeBucket buckets[maxVertices]; // max vertices (??)
};

class ObjectFiller
{
private:
  EdgeTableTuple EdgeTable[maxHeight];
  EdgeTableTuple ActiveEdgeTuple;

  /* Function to sort an array using insertion sort*/
  void insertionSort(EdgeTableTuple *ett);

  void initEdgeTable();
  void insertLinesToEdgeTable(Object *object);
  void storeEdgeInTable(int x1, int y1, int x2, int y2);
  void storeEdgeInTuple(EdgeTableTuple *receiver, int yMax, int xMin, float inverseGradient);
  void removeEdgeByYmax(EdgeTableTuple *Tuple, int currentY);
  void updateXMin(EdgeTableTuple *Tuple);

public:
  ObjectFiller();
  vector<Line> getObjectFillerLines(Object *);
};

#endif