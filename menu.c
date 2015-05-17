#include "fevent.h"

int showmenu (SDL_Surface* screen, TTF_Font* font,FILE* fichierScores)
{
    Uint32 time;
    int x,y,i,j;
    char scoreBuffer[256]="Your score is ";
    char* scoreBuffer1;
    int string_size,read_size;
    //Cherche le dernier bit du fichier
       fseek(fichierScores,0,SEEK_END);
       //offset du premier au dernier bit (taille du fichier)
       string_size = ftell (fichierScores);
       //Retour au début du fichier
       rewind(fichierScores);

       //alloue un string qui tient tout
       scoreBuffer1 = (char*) malloc (sizeof(char) * (string_size + 1) );
       //Lis tout en une opération
       read_size = fread(scoreBuffer1,sizeof(char),string_size,fichierScores);
       //fread ne le configure pas pour mettre un \0 à la fin
       //Le buffer est maintenant un string
       scoreBuffer1[string_size] = '\0';

       if (string_size != read_size) {
           //Quelque chose n'a pas fonctionné. On libère la mémoire et remets le buffer à NULL
           free(scoreBuffer1);
           scoreBuffer1 = NULL;
       }

    strcat(scoreBuffer,scoreBuffer1);
    const char* labels[NUMMENU]={"You win","Exit"};
    SDL_Surface* menus[NUMMENU];
    SDL_Surface* score = NULL;
    bool selected[NUMMENU]={0,0};
    SDL_Color color[2]={{255,255,255},{255,0,0}};

    menus[0]=TTF_RenderText_Solid(font,labels[0],color[0]);
    menus[1]=TTF_RenderText_Solid(font,labels[1],color[0]);
    score=TTF_RenderText_Solid(font,scoreBuffer,color[0]);

    SDL_Rect pos[NUMMENU];
    SDL_Rect posScore;
    pos[0].x=screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
    pos[0].y=screen->clip_rect.h/6 - menus[0]->clip_rect.h;

    pos[1].x=screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
    pos[1].y=screen->clip_rect.h/2 + menus[0]->clip_rect.h;

    posScore.x=screen->clip_rect.w/4 - menus[1]->clip_rect.w/2;
    posScore.y=screen->clip_rect.h/4 + menus[1]->clip_rect.h;

    SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0x00,0x00,0x00));

    SDL_Event event;

    while(1)
    {
        time = SDL_GetTicks();
        while(SDL_PollEvent(&event))
                {
                        switch(event.type)
                        {
                                case SDL_QUIT:
                                    for( i=0;i <NUMMENU;i ++)
                                        SDL_FreeSurface(menus[i]);

                                        return 1;
                                case SDL_MOUSEMOTION:
                                        x = event.motion.x;
                                        y = event.motion.y;
                                        for( i = 0; i < NUMMENU; i ++) {
                                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h)
                                                {
                                                        if(!selected[1])
                                                        {
                                                                selected[1] = 1;
                                                                SDL_FreeSurface(menus[1]);
                                                                menus[1] = TTF_RenderText_Solid(font,labels[i],color[1]);
                                                        }
                                                }
                                                else
                                                {
                                                        if(selected[i])
                                                        {
                                                                selected[i] = 0;
                                                                SDL_FreeSurface(menus[i]);
                                                                menus[i] = TTF_RenderText_Solid(font,labels[i],color[0]);
                                                        }
                                                }
                                        }
                                        break;
                                case SDL_MOUSEBUTTONDOWN:
                                        x = event.button.x;
                                        y = event.button.y;
                                        for( i = 0; i < NUMMENU; i += 1) {
                                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h)
                                                {
                                                    for( j =0;j<NUMMENU;j++)
                                                    {
                                                        SDL_FreeSurface(menus[j]);
                                                    }
                                                return i;
                                                }
                                        }
                                        break;
                                case SDL_KEYDOWN:
                                        if(event.key.keysym.sym == SDLK_ESCAPE)
                                        {
                                                for( i =0;i<NUMMENU;i++)
                                                    {
                                                        SDL_FreeSurface(menus[i]);
                                                    }
                                                return 0;
                                        }
                        }
                }
                for (i = 0; i < NUMMENU; i += 1) {
                        SDL_BlitSurface(menus[i],NULL,screen,&pos[i]);
                          SDL_BlitSurface(score,NULL,screen,&posScore);
                }
                SDL_Flip(screen);
                if(1000/30 > (SDL_GetTicks()-time))
                        SDL_Delay(1000/30 - (SDL_GetTicks()-time));
        }
}


int showacceuil (SDL_Surface* screen, TTF_Font* font)
{
    Uint32 time;
    int x,y,i,j;
    const char* labels[NUMMENU]={"Play","Exit"};
    SDL_Surface* menus[NUMMENU];
    bool selected[NUMMENU]={0,0};
    SDL_Color color[2]={{255,255,255},{255,0,0}};

    menus[0]=TTF_RenderText_Solid(font,labels[0],color[0]);
    menus[1]=TTF_RenderText_Solid(font,labels[1],color[0]);

    SDL_Rect pos[NUMMENU];
    pos[0].x=screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
    pos[0].y=screen->clip_rect.h/2 - menus[0]->clip_rect.h;

    pos[1].x=screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
    pos[1].y=screen->clip_rect.h/2 + menus[0]->clip_rect.h;

    //screen=LoadImage32("acceuil1.bmp");
    SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0x00,0x00,0x00));

    SDL_Event event;

    while(1)
    {
        time = SDL_GetTicks();
        while(SDL_PollEvent(&event))
                {
                        switch(event.type)
                        {
                                case SDL_QUIT:
                                    for( i=0;i <NUMMENU;i ++)
                                        SDL_FreeSurface(menus[i]);

                                        return 1;
                                case SDL_MOUSEMOTION:
                                        x = event.motion.x;
                                        y = event.motion.y;
                                        for( i = 0; i < NUMMENU; i ++) {
                                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h)
                                                {
                                                        if(!selected[i])
                                                        {
                                                                selected[i] = 1;
                                                                SDL_FreeSurface(menus[i]);
                                                                menus[i] = TTF_RenderText_Solid(font,labels[i],color[1]);
                                                        }
                                                }
                                                else
                                                {
                                                        if(selected[i])
                                                        {
                                                                selected[i] = 0;
                                                                SDL_FreeSurface(menus[i]);
                                                                menus[i] = TTF_RenderText_Solid(font,labels[i],color[0]);
                                                        }
                                                }
                                        }
                                        break;
                                case SDL_MOUSEBUTTONDOWN:
                                        x = event.button.x;
                                        y = event.button.y;
                                        for( i = 0; i < NUMMENU; i += 1) {
                                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h)
                                                {
                                                    for( j =0;j<NUMMENU;j++)
                                                    {
                                                        SDL_FreeSurface(menus[j]);
                                                    }
                                                return i;
                                                }
                                        }
                                        break;
                                case SDL_KEYDOWN:
                                        if(event.key.keysym.sym == SDLK_ESCAPE)
                                        {
                                                for( i =0;i<NUMMENU;i++)
                                                    {
                                                        SDL_FreeSurface(menus[i]);
                                                    }
                                                return 0;
                                        }
                        }
                }
                for (i = 0; i < NUMMENU; i += 1) {
                        SDL_BlitSurface(menus[i],NULL,screen,&pos[i]);
                }
                SDL_Flip(screen);
                if(1000/30 > (SDL_GetTicks()-time))
                        SDL_Delay(1000/30 - (SDL_GetTicks()-time));
        }
}

int showPause (SDL_Surface* screen, TTF_Font* font)
{
    Uint32 time;
    int x,y,i,j;
    const char* labels[NUMMENU]={"Continue","Exit"};
    SDL_Surface* menus[NUMMENU];
    bool selected[NUMMENU]={0,0};
    SDL_Color color[2]={{255,255,255},{255,0,0}};

    menus[0]=TTF_RenderText_Solid(font,labels[0],color[0]);
    menus[1]=TTF_RenderText_Solid(font,labels[1],color[0]);

    SDL_Rect pos[NUMMENU];
    pos[0].x=screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
    pos[0].y=screen->clip_rect.h/2 - menus[0]->clip_rect.h;

    pos[1].x=screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
    pos[1].y=screen->clip_rect.h/2 + menus[0]->clip_rect.h;

    SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0x00,0x00,0x00));

    SDL_Event event;

    while(1)
    {
        time = SDL_GetTicks();
        while(SDL_PollEvent(&event))
                {
                        switch(event.type)
                        {
                                case SDL_QUIT:
                                    for( i=0;i <NUMMENU;i ++)
                                        SDL_FreeSurface(menus[i]);

                                        return 1;
                                case SDL_MOUSEMOTION:
                                        x = event.motion.x;
                                        y = event.motion.y;
                                        for( i = 0; i < NUMMENU; i ++) {
                                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h)
                                                {
                                                        if(!selected[i])
                                                        {
                                                                selected[i] = 1;
                                                                SDL_FreeSurface(menus[i]);
                                                                menus[i] = TTF_RenderText_Solid(font,labels[i],color[1]);
                                                        }
                                                }
                                                else
                                                {
                                                        if(selected[i])
                                                        {
                                                                selected[i] = 0;
                                                                SDL_FreeSurface(menus[i]);
                                                                menus[i] = TTF_RenderText_Solid(font,labels[i],color[0]);
                                                        }
                                                }
                                        }
                                        break;
                                case SDL_MOUSEBUTTONDOWN:
                                        x = event.button.x;
                                        y = event.button.y;
                                        for( i = 0; i < NUMMENU; i += 1) {
                                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h)
                                                {
                                                    for( j =0;j<NUMMENU;j++)
                                                    {
                                                        SDL_FreeSurface(menus[j]);
                                                    }
                                                return i;
                                                }
                                        }
                                        break;
                                case SDL_KEYDOWN:
                                        if(event.key.keysym.sym == SDLK_ESCAPE)
                                        {
                                                for( i =0;i<NUMMENU;i++)
                                                    {
                                                        SDL_FreeSurface(menus[i]);
                                                    }
                                                return 0;
                                        }
                        }
                }
                for (i = 0; i < NUMMENU; i += 1) {
                        SDL_BlitSurface(menus[i],NULL,screen,&pos[i]);
                }
                SDL_Flip(screen);
                if(1000/30 > (SDL_GetTicks()-time))
                        SDL_Delay(1000/30 - (SDL_GetTicks()-time));
        }
}

int menuPause(Input* in,SDL_Surface* screen,TTF_Font* font)
{
    int i;
    if (in->key[SDLK_p])
       i=showPause(screen,font);
    return i;
}

