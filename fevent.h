#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdbool.h>
#define NUMMENU 2

typedef struct
{
	char key[SDLK_LAST];
	int mousex,mousey;
	int mousexrel,mouseyrel;
	char mousebuttons[8];
	char quit;
} Input;

void UpdateEvents(Input* in,SDL_Surface* screen, TTF_Font* font,int* i);
void InitEvents(Input* in);
int showmenu (SDL_Surface* screen, TTF_Font* font,FILE* fichierScores);
int showacceuil (SDL_Surface* screen, TTF_Font* font);
int showPause (SDL_Surface* screen, TTF_Font* font);

int menuPause(Input * in,SDL_Surface* screen,TTF_Font* font);
