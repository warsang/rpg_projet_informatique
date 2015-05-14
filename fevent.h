#include <SDL/SDL.h>

typedef struct
{
	char key[SDLK_LAST];
	int mousex,mousey;
	int mousexrel,mouseyrel;
	char mousebuttons[8];
	char quit;
} Input;

//void inputDuNom(int* playerHasName);
void UpdateEvents(Input* in);
void InitEvents(Input* in);
