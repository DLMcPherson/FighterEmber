#ifndef TILERENDER_H
#define TILERENDER_H

#include <SDL2/SDL.h>

int TilDx(int);
int TilUx(int);
int TilDy(int);
int TilUy(int);
void DrawRect(int X, int Y, int Width, int Height, int R, int G, int B,SDL_Renderer* rend);
void FillTile(int TileX, int TileY, int R, int G, int B, SDL_Renderer* rend);

#endif
