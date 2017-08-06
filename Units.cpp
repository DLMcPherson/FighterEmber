#include "Units.h"
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>

using namespace std;

Unit::Unit(){
  X=rand()%38+1;
  Y=rand()%38+1;

  Level = 1;
  HP = 20;
  Str = 6;
  Skl = 6;
  Spd = 7;
  Lck = 2;
  Def = 5;
  Res = 1;
  Con = 9;
  Mov = 7;
  team = 1;
  targetable = 0;

  tintR = 255;
  tintG = 255;
  tintB = 255;

  isHero = 0;
  traj_head = NULL;
  traj_curr = NULL;

  equip = new Item();
  target = NULL;

  frame = 0;
  tick = 0;
}

void Unit::animTick(){
  tick++; tick = tick%20;
  if(tick%10 == 8 || tick%10==9){
    frame++;
    frame = frame%4;
  }
}

void Unit::drawMove(){
  animTick();
  sprite->drawDown(X,Y,frame);
}

void Unit::drawIdle(){
  animTick();
  sprite->drawIdle(X,Y,frame);
}

void Unit::drawSelc(){
  animTick();
  sprite->drawSelc(X,Y,frame);
}

bool Unit::PathAdvance(){
  if(traj_curr->next != NULL){
    traj_curr = traj_curr->next;
    X = traj_curr->X;
    Y = traj_curr->Y;
    return(0);
  }
  else{
    delete traj_head;
    traj_curr = NULL;
    traj_head = NULL;
    return(1);
  }
}

void Unit::AssignPath(LinkTile* _head){
  traj_head = _head;
  traj_curr = _head;
}

void Unit::Attack(){
  int hitChance = Accuracy() - target->Evasion();
  if(hitChance<0) hitChance = 0;
  if(hitChance>100) hitChance = 100;
  printf("Hit Chance = %d,   ",hitChance);
  if(rand()%100<hitChance){
    int damage = AttackPower() + WeaponPower() - target->DefensePower();
    if(damage<0) damage=0;
    target->HP -= damage;
    printf("Attack: %d HP Left \n",target->HP);
  }
  else{
    printf("Dodged!! \n");
  }
  // Cleanup
  target = NULL;
  return;
}

int Unit::Accuracy(){
  return(equip->Hit + Skl*2 + Lck/2);
}

int Unit::Evasion(){
  // Note that Spd here should go down if character carrying heavy item
  // Add Terrain Bonus later
  return(Spd*2 + Lck);
}

int Unit::AttackPower(){
  return(Str);
}

int Unit::DefensePower(){
  // Add terrain bonus eventually
  return(Def);
}

int Unit::WeaponPower(){
  // Add weapon triangle eventually
  return(equip->Mt);
}

// ============= OverworldSpritesheet Class ========
void OverworldSpritesheet::drawUp(int X, int Y, int frame){
  draw(upTex,X,Y,frame);
}
void OverworldSpritesheet::drawDown(int X, int Y, int frame){
  draw(downTex,X,Y,frame);
}
void OverworldSpritesheet::drawLeft(int X, int Y, int frame){
  draw(leftTex,X,Y,frame);
}
void OverworldSpritesheet::drawRight(int X, int Y, int frame){
  draw(leftTex,X,Y,frame);
}
void OverworldSpritesheet::drawIdle(int X, int Y, int frame){
  draw(idleTex,X,Y,frame);
}
void OverworldSpritesheet::drawSelc(int X, int Y, int frame){
  draw(selcTex,X,Y,frame);
}

void OverworldSpritesheet::draw(SDL_Texture* tex, int X, int Y, int frame){
  SDL_Rect Offset;
  Offset.x=X*20-14;
  Offset.y=Y*20-14;
  Offset.w=48;
  Offset.h=48;
  SDL_Rect Clip;
  Clip.x=frame*48;
  Clip.y=0;
  Clip.w=48;
  Clip.h=48;
  SDL_SetTextureColorMod(tex,255,255,255);
  SDL_RenderCopy(rend, tex, &Clip, &Offset);
  return;
}

OverworldSpritesheet::OverworldSpritesheet(SDL_Renderer* ren,string name){
  string imagePath;
  string path = "res/map_anim/";
  rend = ren;

  imagePath = path + name + "_map_up.png";
  SDL_Surface* surf = IMG_Load(imagePath.c_str());
  upTex = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  imagePath = path + name + "_map_down.png";
  surf = IMG_Load(imagePath.c_str());
  downTex = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  imagePath = path + name + "_map_side.png";
  surf = IMG_Load(imagePath.c_str());
  leftTex = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  imagePath = path + name + "_map_idle.png";
  surf = IMG_Load(imagePath.c_str());
  idleTex = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  imagePath = path + name + "_map_selected.png";
  surf = IMG_Load(imagePath.c_str());
  selcTex = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
}

OverworldSpritesheet::~OverworldSpritesheet(){
  SDL_DestroyTexture(upTex);
  SDL_DestroyTexture(downTex);
  SDL_DestroyTexture(leftTex);
  SDL_DestroyTexture(idleTex);
  SDL_DestroyTexture(selcTex);
}

// ============= Tile Linked List Class ==========

LinkTile::LinkTile(int _X, int _Y){
  X = _X;
  Y = _Y;
  next = NULL;
}

LinkTile::~LinkTile(){
  if(next != NULL){
    delete next;
  }
}

// ============= Item Class ===========

Item::Item(){
  Type = 0;
  Mt = 5;
  Hit=90;
  Crit=0;
  Uses=23;
  Worth=460;
}
