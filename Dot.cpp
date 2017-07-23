#include "Dot.h"
#include <SDL2/SDL.h>
#include "functions.h"

Dot::Dot(){
  tileX=1;
  tileY=1;
  //
  R=130;
  G=50;
  B=20;
  //
  inx=0;
  iny=0;
}
void Dot::startOff(int tX, int tY,int offX, int offY){
  tileX=tX;
  tileY=tY;
  //
  inx=offX*8;
  iny=offY*6;
  //
  sourceX=offX;
  sourceY=offY;
}

void Dot::wander(int speed){
  for(int i=0;i<speed;i++){
    inx+=rand()%3-1;
    iny+=rand()%3-1;
  }
  if(inx>8) inx=8;
  if(inx<-8)  inx=-8;
  if(iny>6) iny=6;
  if(iny<-6)  iny=-6;
}

void Dot::attract(int speed){
  for(int i=0;i<speed;i++){
    int oX=inx-2*sourceX;
    int oY=iny-2*sourceY;
    if(oX != 0) inx+=-oX/abs(oX);
    if(oY != 0) iny+=-oY/abs(oY);
  }
}

void Dot::update(){
  wander(2);
  attract(1);
}

void Dot::draw(SDL_Renderer *rend){
  DrawRect(tileX*16+8+inx-1,tileY*12+6+iny-1,2,2,250,10+rand()%70,10,rend);
}
