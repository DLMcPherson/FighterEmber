#include <iostream>
#include <SDL2/SDL.h>
#include "tileRender.h"
#include <math.h>

using namespace std;

int TilDy(int A)
{
	return((int)floor(A/12));
}

int TilUy(int A)
{
	return((int)ceil(A/12));
}

int TilDx(int A)
{
	return((int)floor(A/16));
}

int TilUx(int A)
{
	return((int)ceil(A/16));
}

void DrawRect(int X, int Y, int Width, int Height, int R, int G, int B,SDL_Renderer* rend){
  SDL_Rect Recto;
  Recto.x=X;
  Recto.y=Y;
  Recto.w=Width;
  Recto.h=Height;
  SDL_SetRenderDrawColor(rend,R,G,B, 255);
  SDL_RenderFillRect(rend,&Recto);
  return;
}

void FillTile(int TileX, int TileY, int R, int G, int B, SDL_Renderer* rend){
  DrawRect(TileX*20,TileY*20,20,20,R,G,B,rend);
}
