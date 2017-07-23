#ifndef MAGICDOTS_H
#define MAGICDOTS_H

#include <SDL2/SDL.h>

class Dot{
public:
  Dot();
  void startOff(int tX, int tY,int offX, int offY);
  void wander(int speed);
  void attract(int speed);
  void update();
  void draw(SDL_Renderer*);
private:
  int inx;
  int iny;
  int tileX;
  int tileY;
  int R;
  int G;
  int B;
  int sourceX;
  int sourceY;
};

#endif
