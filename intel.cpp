#include "intel.h"
#include "Units.h"

LinkTile* PathFrom(int** Dist,int cursX, int cursY){
	LinkTile* destination = new LinkTile(cursX,cursY);
	LinkTile* step = destination;
	// Work your way back from destination to source
	int stepX = cursX;
	int stepY = cursY;
	for(int i=Dist[cursX][cursY];i>0;i--){
		int backX = stepX;
		int backY = stepY;
		// Search out shortest backstep
		if(Dist[stepX+1][stepY]==i-1){
			backX = stepX+1;
			backY = stepY;
		}
		if(Dist[stepX-1][stepY]==i-1){
			backX = stepX-1;
			backY = stepY;
		}
		if(Dist[stepX][stepY-1]==i-1){
			backX = stepX;
			backY = stepY-1;
		}
		if(Dist[stepX][stepY+1]==i-1){
			backX = stepX;
			backY = stepY+1;
		}
		// Add previous step to chain
		LinkTile* prev = new LinkTile(backX,backY);
		prev->next = step;
		// Make previous step the current step
		step = prev;
		stepX = backX;
		stepY = backY;
	}
	// Bake and return
	return(step);
}

void checkClosestTouch(int** Dist,int targX,int targY,int &backX,int &backY,int &curDist){
  if(Dist[targX][targY]<curDist){
    backX = targX;
    backY = targY;
    curDist = Dist[backX][backY];
  }
}
