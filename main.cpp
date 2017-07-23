#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include "Units.h"
#include "tileRender.h"
#include "intel.h"

#define MAPW 40
#define MAPH 40

using namespace std;

SDL_Event Tempo;

SDL_Texture* LoadImageD(string, SDL_Renderer*);
bool rollout(int**, int**, Unit* **,int,int,int);

SDL_Texture* LoadImageD(string imagePath, SDL_Renderer* ren){
  SDL_Surface *bmp = SDL_LoadBMP(imagePath.c_str());
  Uint32 BgColor = SDL_MapRGB(bmp->format,255,255,255);
  SDL_SetColorKey(bmp,SDL_TRUE,BgColor);
  SDL_Texture* loaded = SDL_CreateTextureFromSurface(ren, bmp);
  SDL_FreeSurface(bmp);
  return(loaded);
}

bool rollout(int** Dist,int** Map, Unit* ** UMap,int X,int Y,int Move)
{
  printf("started rollout \n");
  LinkTile curFrontierHead(513,513);
  LinkTile nexFrontierHead(513,513);
  LinkTile* curFrontierTail;
  LinkTile* nexFrontierTail;

  Dist[X][Y] = 0;
  nexFrontierHead.next = new LinkTile(X,Y);
  for(int steps=1;steps<= Move;steps++){
    printf("Step %d ... ",steps);
    curFrontierHead.next = nexFrontierHead.next;
    nexFrontierHead.next = NULL;
    curFrontierTail = &curFrontierHead;
    nexFrontierTail = &nexFrontierHead;
    while(curFrontierTail->next != NULL){
      curFrontierTail = curFrontierTail->next;
      int hx = curFrontierTail->X;
      int hy = curFrontierTail->Y;
      if(Dist[hx+1][hy]>steps && Map[hx+1][hy] == 0 && UMap[hx+1][hy] == NULL){
        Dist[hx+1][hy]=steps;
        nexFrontierTail->next = new LinkTile(hx+1,hy);
        nexFrontierTail = nexFrontierTail->next;
      }
      if(Dist[hx-1][hy]>steps && Map[hx-1][hy] == 0 && UMap[hx-1][hy] == NULL){
        Dist[hx-1][hy]=steps;
        nexFrontierTail->next = new LinkTile(hx-1,hy);
        nexFrontierTail = nexFrontierTail->next;
      }
      if(Dist[hx][hy-1]>steps && Map[hx][hy-1] == 0 && UMap[hx][hy-1] == NULL){
        Dist[hx][hy-1]=steps;
        nexFrontierTail->next = new LinkTile(hx,hy-1);
        nexFrontierTail = nexFrontierTail->next;
      }
      if(Dist[hx][hy+1]>steps && Map[hx][hy+1] == 0 && UMap[hx][hy+1] == NULL){
        Dist[hx][hy+1]=steps;
        nexFrontierTail->next = new LinkTile(hx,hy+1);
        nexFrontierTail = nexFrontierTail->next;
      }
    }
    // Destroy curFrontier chain
    printf("finished + cleaned \n");
    delete curFrontierHead.next;
  }
  delete nexFrontierHead.next;
  curFrontierHead.next = NULL;
  nexFrontierHead.next = NULL;
  printf("cleaningup \n");
  return(1);
}

int main(int argc, char **argv){

  // Seed random value generator
  srand(time(NULL));

  // Create map arrays and initialize to zero
  // int Map[MAPW][MAPH];
  // Unit* UMap[MAPW][MAPH];
  // int Dist[MAPW][MAPH];
  int** Map = (int**)malloc(MAPW * sizeof(int *));
  for (int i=0; i<MAPW; i++)
    Map[i] = (int *)malloc(MAPH * sizeof(int));

  int** Dist = (int**)malloc(MAPW * sizeof(int *));
  for (int i=0; i<MAPW; i++)
    Dist[i] = (int *)malloc(MAPH * sizeof(int));

  Unit*** UMap = (Unit* **)malloc(MAPW * sizeof(Unit* *));
  for (int i=0; i<MAPW; i++)
    UMap[i] = (Unit* *)malloc(MAPH * sizeof(Unit*));

  for(int mx=0;mx<MAPW;mx++){
    for(int my=0;my<MAPH;my++){
      Map[mx][my] = 0;
      UMap[mx][my] = NULL;
      Dist[mx][my] = 513;
    }
  }

  // Draw map bounds
	for(int j=0;j<MAPH;j++){
		Map[j][0]=1;
    Map[0][j]=1;
    Map[j][39]=1;
    Map[39][j]=1;
	}
  for(int j=10;j<20;j++){
    Map[10][j] = 1;
    Map[30][j] = 1;
    Map[10+j][30] = 1;
  }

  // Create keyboard object
  const Uint8* keys = SDL_GetKeyboardState(NULL);
  // Create screen
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		cout << "SDL_Init Error: " << SDL_GetError() << endl;
		return 1;
	}
  IMG_Init(IMG_INIT_PNG);
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &win, &ren);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	//SDL_RenderSetLogicalSize(ren, 640, 480);

  // Load Images
  OverworldSpritesheet thief(ren,"assassin");
  OverworldSpritesheet lyn(ren,"lyn");

  // Heroes array
  Unit Heroes[10];
  Unit Enmies[10];
  Unit* selection;
  bool selected=0;
  for(int i=0;i<10;i++){
    UMap[Heroes[i].X][Heroes[i].Y] = &(Heroes[i]);
    UMap[Enmies[i].X][Enmies[i].Y] = &(Enmies[i]);
  }
  for(int i=0;i<10;i++){
    Heroes[i].sprite = &lyn;
    Enmies[i].sprite = &thief;
  }

  // Offset shill variable
  SDL_Rect Offset;
  Offset.x=100;
  Offset.y=100;
  Offset.w=48;
  Offset.h=48;
  SDL_Rect Clip;
  Clip.x=0;
  Clip.y=0;
  Clip.w=48;
  Clip.h=48;
  // Player position initialization
  int cursX=5;
  int cursY=5;
  int clickTick = 0;

  // ================= MAIN LOOP =========
	bool Breaker=1;
  int Pause=0;
	while(Breaker){
		//First clear the renderer
    SDL_SetRenderDrawColor(ren,40,30,10,255);
		SDL_RenderClear(ren);

    // Draw the map
    for(int mx=0;mx<MAPW;mx++){
      for(int my=0;my<MAPH;my++){
        if(Map[mx][my]==0){
          FillTile(mx,my,20,150,60,ren);
          Offset.x=mx*20;
          Offset.y=my*20;
          if(Dist[mx][my]<500){
            int Coff = 100-Dist[mx][my]*10;
            FillTile(mx,my,20+Coff,150+Coff,60+Coff,ren);
          }
        }
        if(Map[mx][my]==1){
          FillTile(mx,my,100,40,20,ren);
        }
      }
    }

    // Manage the heroes
    for(int i=0;i<10;i++){
      // Animate progression through path
      if(Heroes[i].traj_head != NULL){
        if(Heroes[i].PathAdvance()){
          UMap[Heroes[i].X][Heroes[i].Y] = &(Heroes[i]);
          if(Heroes[i].target != NULL) Heroes[i].Attack();
        }
      }
      // Render
      FillTile(Heroes[i].X,Heroes[i].Y,30,60,150,ren);
      if(Heroes[i].HP<20)
        FillTile(Heroes[i].X,Heroes[i].Y,10,20,50,ren);
      if(selection == &(Heroes[i]))
        Heroes[i].drawSelc();
      else
        Heroes[i].drawIdle();
    }

    // Manage the enemies
    for(int i=0;i<10;i++){
      // Animate progression through path
      if(Enmies[i].traj_head != NULL){
        if(Enmies[i].PathAdvance()){
          UMap[Enmies[i].X][Enmies[i].Y] = &(Enmies[i]);
          if(Enmies[i].target != NULL) Enmies[i].Attack();
        }
      }
      // Render
      FillTile(Enmies[i].X,Enmies[i].Y,150,20,30,ren);
      if(Enmies[i].HP<20)
        FillTile(Enmies[i].X,Enmies[i].Y,50,5,10,ren);
      if(selection == &(Enmies[i]))
        Enmies[i].drawSelc();
      else
        Enmies[i].drawIdle();
    }

    // Draw the cursor
    Offset.x=cursX*20-14;
    Offset.y=cursY*20-14;
    FillTile(cursX,cursY,100,100,100,ren);
    //SDL_RenderCopy(ren, hero, &Clip, &Offset);

    //Update the screen
    SDL_RenderPresent(ren);
    //Take a quick break after all that hard work
    SDL_Delay(100);

		// Event loop
    // Accept user input
    while(SDL_PollEvent(&Tempo))
    {
      // ====== Pressing Keys
      if(Tempo.type==SDL_KEYDOWN)
      {
        switch(Tempo.key.keysym.sym)
        {
        case SDLK_ESCAPE:
        Breaker=0;
        break;
        case SDLK_f:
        // Move unit if applicable
        if(selection != NULL){
          if(UMap[cursX][cursY] == NULL && Dist[cursX][cursY] <= selection->Mov){
            LinkTile* path = PathFrom(Dist,cursX,cursY);
            selection->AssignPath(path);
            UMap[selection->X][selection->Y] = NULL;
            //clickTick = Dist[cursX][cursY];
          }
          // Attack and move
          if(UMap[cursX][cursY] != NULL && Dist[cursX][cursY] <= selection->Mov +1){
            Unit* attackTarget = UMap[cursX][cursY];
            // Search out closest tile to target
            int backX = cursX;
            int backY = cursY;
            int i = Dist[cursX][cursY];
            if(Dist[cursX+1][cursY]==i-1){
              backX = cursX+1;
              backY = cursY;
            }
            if(Dist[cursX-1][cursY]==i-1){
              backX = cursX-1;
              backY = cursY;
            }
            if(Dist[cursX][cursY-1]==i-1){
              backX = cursX;
              backY = cursY-1;
            }
            if(Dist[cursX][cursY+1]==i-1){
              backX = cursX;
              backY = cursY+1;
            }
            // Path to that target
            LinkTile* path = PathFrom(Dist,backX,backY);
            selection->AssignPath(path);
            UMap[selection->X][selection->Y] = NULL;
            // Queue up attack
            selection->target = attackTarget;
          }
          // Clean last selection
          int lo_x = selection->X-10; if(lo_x<0) lo_x=0;
          int hi_x = selection->X+10; if(hi_x>=MAPW) hi_x=MAPW-1;
          int lo_y = selection->Y-10; if(lo_y<0) lo_x=0;
          int hi_y = selection->Y+10; if(hi_y>=MAPH) hi_y=MAPH-1;
          for(int i=lo_x;i<=hi_x;i++){
            for(int j=lo_y;j<=hi_y;j++){
              Dist[i][j] = 513;
            }
          }
          selection = NULL;
        }
        else if(clickTick == 0){
          // Select new unit
          if(UMap[cursX][cursY] != NULL){
            // New selection
            selection = UMap[cursX][cursY];
            selected = 1;
            // Rollout destination possibilities
            rollout(Dist,Map,UMap,cursX,cursY,selection->Mov);
            printf("F Back \n");
          }
        }
        break;
        }
      }
	    // ====== Releasing Keys
			else if(Tempo.type==SDL_KEYDOWN)
	    {
        switch(Tempo.key.keysym.sym)
        {
	        case SDLK_ESCAPE:
	        Breaker=0;
	        break;
        }
			}
    }

		// Player movement
    if(keys[SDLK_ESCAPE])
      Breaker=1;
		if(keys[SDL_SCANCODE_RIGHT])
			cursX+=1;
	  if(keys[SDL_SCANCODE_LEFT])
			cursX-=1;
  	if(keys[SDL_SCANCODE_DOWN])
			cursY+=1;
	  if(keys[SDL_SCANCODE_UP])
			cursY-=1;
    if(keys[SDL_SCANCODE_1])
      Pause=(Pause+1)%2;

	}

  // Free Textures
  // Other SDL
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
  IMG_Quit();
	SDL_Quit();

}
