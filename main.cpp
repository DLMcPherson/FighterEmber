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
#define NUM_HEROES 4
#define NUM_ENMIES 4

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

void rollout_passOn(int** Dist,int** Map, Unit* ** UMap,int X,int Y, LinkTile* &nexFrontierTail,int steps){
  if(Dist[X][Y]>steps && Map[X][Y] == 0){
    if(UMap[X][Y] == NULL){
      Dist[X][Y]=steps;
      nexFrontierTail->next = new LinkTile(X,Y);
      nexFrontierTail = nexFrontierTail->next;
    }
    else{
      UMap[X][Y]->targetable = 1;
    }
  }

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
  for(int steps=1;steps<= Move+1;steps++){
    printf("Step %d ... ",steps);
    curFrontierHead.next = nexFrontierHead.next;
    nexFrontierHead.next = NULL;
    curFrontierTail = &curFrontierHead;
    nexFrontierTail = &nexFrontierHead;
    while(curFrontierTail->next != NULL){
      curFrontierTail = curFrontierTail->next;
      int hx = curFrontierTail->X;
      int hy = curFrontierTail->Y;
      rollout_passOn(Dist,Map,UMap,hx+1,hy,nexFrontierTail,steps);
      rollout_passOn(Dist,Map,UMap,hx-1,hy,nexFrontierTail,steps);
      rollout_passOn(Dist,Map,UMap,hx,hy-1,nexFrontierTail,steps);
      rollout_passOn(Dist,Map,UMap,hx,hy+1,nexFrontierTail,steps);
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
  Unit Heroes[NUM_HEROES];
  Unit Enmies[NUM_ENMIES];
  Unit* selection;
  bool selected=0;
  for(int i=0;i<NUM_HEROES;i++){
    Heroes[i].sprite = &lyn;
    Heroes[i].team = 1;
    Heroes[i].Y = 30+i%2;
    Heroes[i].X = 10+i;
    Enmies[i].sprite = &thief;
    Enmies[i].team = 2;
  }
  for(int i=0;i<NUM_HEROES;i++){
    UMap[Heroes[i].X][Heroes[i].Y] = &(Heroes[i]);
    UMap[Enmies[i].X][Enmies[i].Y] = &(Enmies[i]);
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
  int NumMoves = 0;
  int CurMove = 0;
  int LiveHeroes = NUM_HEROES;
  int LiveEnmies = NUM_ENMIES;
  bool PlayerTurn = 1;
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
    LiveHeroes=0;
    for(int i=0;i<NUM_HEROES;i++){
      // Animate progression through path
      Heroes[i].animTick();
      if(Heroes[i].traj_head != NULL){
        if(Heroes[i].PathAdvance()){
          NumMoves++;
          UMap[Heroes[i].X][Heroes[i].Y] = &(Heroes[i]);
          if(Heroes[i].target != NULL) Heroes[i].Attack();
        }
        //Heroes[i].sprite->drawDown(Heroes[i].X,Heroes[i].Y,Heroes[i].frame);
      }
      {
        // Render
        FillTile(Heroes[i].X,Heroes[i].Y,30,60,150,ren);
        if(Heroes[i].HP<=0)
          FillTile(Heroes[i].X,Heroes[i].Y,10,20,50,ren);
        if(selection == &(Heroes[i]))
          Heroes[i].sprite->drawSelc(Heroes[i].X,Heroes[i].Y,Heroes[i].frame);
        else
          Heroes[i].sprite->drawIdle(Heroes[i].X,Heroes[i].Y,Heroes[i].frame);
      }
      if(Heroes[i].HP>0){
        LiveHeroes++;
      }
    }

    // Manage the enemies
    LiveEnmies=0;
    for(int i=0;i<NUM_ENMIES;i++){
      // Animate progression through path
      if(Enmies[i].traj_head != NULL){
        if(Enmies[i].PathAdvance()){
          NumMoves++;
          UMap[Enmies[i].X][Enmies[i].Y] = &(Enmies[i]);
          if(Enmies[i].target != NULL) Enmies[i].Attack();
        }
      }
      // Render
      FillTile(Enmies[i].X,Enmies[i].Y,150,20,30,ren);
      if(Enmies[i].HP<=0)
        FillTile(Enmies[i].X,Enmies[i].Y,50,5,10,ren);
      if(selection == &(Enmies[i]))
        Enmies[i].drawSelc();
      else
        Enmies[i].drawIdle();
      // Tally how many enemies are left
      if(Enmies[i].HP>0){
        LiveEnmies++;
      }
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
          if(UMap[cursX][cursY] != NULL){
            Unit* attackTarget = UMap[cursX][cursY];
            if(attackTarget->targetable && attackTarget->team != selection->team){
              // Search out closest tile to target
              int backX = cursX;
              int backY = cursY;
              int curDist = 513;
              checkClosestTouch(Dist,cursX+1,cursY,backX,backY,curDist);
              checkClosestTouch(Dist,cursX-1,cursY,backX,backY,curDist);
              checkClosestTouch(Dist,cursX,cursY+1,backX,backY,curDist);
              checkClosestTouch(Dist,cursX,cursY-1,backX,backY,curDist);
              // Path to that target
              LinkTile* path = PathFrom(Dist,backX,backY);
              selection->AssignPath(path);
              UMap[selection->X][selection->Y] = NULL;
              // Queue up attack
              selection->target = attackTarget;
            }
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
          for(int i=0;i<NUM_HEROES;i++)
            Heroes[i].targetable = 0;
          for(int i=0;i<NUM_ENMIES;i++)
            Enmies[i].targetable = 0;
          selection = NULL;
        }
        else if(clickTick == 0){
          // Select new unit
          if(UMap[cursX][cursY] != NULL){
            if(UMap[cursX][cursY]->team == 1 && UMap[cursX][cursY]->HP>0){
              // New selection
              selection = UMap[cursX][cursY];
              selected = 1;
              // Rollout destination possibilities
              rollout(Dist,Map,UMap,cursX,cursY,selection->Mov);
              printf("F Back \n");
            }
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
    if(PlayerTurn){
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

    // Turn Switching
    if(NumMoves>=NUM_HEROES && PlayerTurn){
      PlayerTurn = 0;
      NumMoves = 0;
    }
    if(NumMoves>=NUM_ENMIES && PlayerTurn==0){
      PlayerTurn = 1;
      NumMoves = 0;
      CurMove = 0;
    }

    if(LiveEnmies==0){
      SDL_SetRenderDrawColor(ren,120,180,200,255);
      SDL_RenderClear(ren);
      printf("YOU WIN!!\n");
    }

    // Enemy AI
    if(CurMove == NumMoves && PlayerTurn==0){
      selection = &(Enmies[CurMove]);
      if(selection->HP>0){
        rollout(Dist,Map,UMap,selection->X,selection->Y,selection->Mov);
        int i = 0;
        for(i=0;i<NUM_HEROES;i++){
          if(Heroes[i].targetable){
            Unit* attackTarget = &(Heroes[i]);
            // Search out closest tile to target
            int atckX= attackTarget->X;
            int atckY= attackTarget->Y;
            int backX = atckX;
            int backY = atckY;
            int curDist = 513;
            checkClosestTouch(Dist,atckX+1,atckY,backX,backY,curDist);
            checkClosestTouch(Dist,atckX-1,atckY,backX,backY,curDist);
            checkClosestTouch(Dist,atckX,atckY+1,backX,backY,curDist);
            checkClosestTouch(Dist,atckX,atckY-1,backX,backY,curDist);
            // Path to that target
            LinkTile* path = PathFrom(Dist,backX,backY);
            selection->AssignPath(path);
            UMap[selection->X][selection->Y] = NULL;
            // Queue up attack
            selection->target = attackTarget;
            i = 100;
          }
        }
        if(selection->target == NULL){
          NumMoves++;
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
        for(int i=0;i<NUM_HEROES;i++)
          Heroes[i].targetable = 0;
        for(int i=0;i<NUM_ENMIES;i++)
          Enmies[i].targetable = 0;
        selection = NULL;
        CurMove++;
      }
      else{
        CurMove++;
        NumMoves++;
      }
    }



	}

  // Free Textures
  // Other SDL
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
  IMG_Quit();
	SDL_Quit();

}
