#ifndef INTEL_H
#define INTEL_H

#include "Units.h"

LinkTile* PathFrom(int** Dist,int cursX, int cursY);

void checkClosestTouch(int** Dist,int targX,int targY,int &backX,int &backY,int &curDist);

#endif
