#ifndef UNITS_H
#define UNITS_H

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>

class LinkTile{
public:
  LinkTile(int, int);
  int X;
  int Y;
  LinkTile* next;

  ~LinkTile();
private:
};

class Item{
public:
  int Type;
  int Mt;
  int Hit;
  int Crit;
  int Uses;
  int Worth;

  Item();
};

class OverworldSpritesheet{
public:
  void drawUp(int X, int Y, int frame);
  void drawDown(int X, int Y, int frame);
  void drawLeft(int X, int Y, int frame);
  void drawRight(int X, int Y, int frame);
  void drawIdle(int X, int Y, int frame);
  void drawSelc(int X, int Y, int frame);

  OverworldSpritesheet(SDL_Renderer*,std::string);
  ~OverworldSpritesheet();
private:
  void draw(SDL_Texture*,int X, int Y, int frame);

  SDL_Texture* upTex;
  SDL_Texture* downTex;
  SDL_Texture* leftTex;
  SDL_Texture* idleTex;
  SDL_Texture* selcTex;

  SDL_Renderer* rend;
};

class Unit{
public:
  Unit();
  int X;
  int Y;
  int Mov;
  int HP;

  void AssignPath(LinkTile*);
  bool PathAdvance();
  bool isHero;
  LinkTile* traj_head;
  Unit* target;

  Item* equip;

  int DefensePower();
  int AttackPower();
  int WeaponPower();

  int Accuracy();
  int Evasion();
  void Attack();

  OverworldSpritesheet* sprite;

  void drawIdle();
  void drawSelc();
  int frame;
  int tick;
private:
  //SDL_Texture* Image;
  LinkTile* traj_curr;
  int Level;

  int Str;
  int Skl;
  int Spd;
  int Lck;
  int Def;
  int Res;
  int Con;
};

#endif
