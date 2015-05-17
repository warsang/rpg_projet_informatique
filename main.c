#include "fmap.h"
#include "fevent.h"
#include <stdbool.h>

#include <SDL/SDL_ttf.h>


#define LARGEUR_FENETRE 512
#define HAUTEUR_FENETRE 512
#define MOVE_SPEED_PLAYER 2
#define MOVE_SPEED_SCROLL 2

#define SPRITE_SIZE 32

#define SGN(X) (((X)==0)?(0):(((X)<0)?(-1):(1)))
#define ABS(X) ((((X)<0)?(-(X)):(X)))

#define count_down_time_in_secs 20000
#define TAILLE_BUFFER_SCORES 256

void MoveMap(Map* m,Input* in)
{
    if (in->key[SDLK_f])
        m->xscroll-=MOVE_SPEED_SCROLL;
    if (in->key[SDLK_h])
        m->xscroll+=MOVE_SPEED_SCROLL;
    if (in->key[SDLK_t])
        m->yscroll-=MOVE_SPEED_SCROLL;
    if (in->key[SDLK_g])
        m->yscroll+=MOVE_SPEED_SCROLL;
// limitation
    if (m->xscroll<0)
        m->xscroll=0;
    if (m->yscroll<0)
        m->yscroll=0;
    if (m->xscroll>m->nbtiles_largeur_monde*m->LARGEUR_TILE-m->largeur_fenetre-1)
        m->xscroll=m->nbtiles_largeur_monde*m->LARGEUR_TILE-m->largeur_fenetre-1;
    if (m->yscroll>m->nbtiles_hauteur_monde*m->HAUTEUR_TILE-m->hauteur_fenetre-1)
        m->yscroll=m->nbtiles_hauteur_monde*m->HAUTEUR_TILE-m->hauteur_fenetre-1;
}

/*Recupere le vecteur de déplacement et change la frame d'animation du sprite*/
void RecupererVecteur(Input* in,int* vx,int* vy,SDL_Rect* rcSrc)
{
    int vitesse = MOVE_SPEED_PLAYER;
    *vx = *vy = 0;
    if (in->key[SDLK_UP])
    {
        *vy = -vitesse;

        if ( (*rcSrc).x == 0 )
        {
            (*rcSrc).x = 32;
        }
        else
        {
            (*rcSrc).x = 0;
        }
    }

    if (in->key[SDLK_DOWN])
    {
        *vy = vitesse;

        if ( (*rcSrc).x == 128 )
        {
            (*rcSrc).x = 160;
        }
        else
        {
            (*rcSrc).x = 128;
        }
    }
    if (in->key[SDLK_LEFT])
    {
        *vx = -vitesse;

        if ( (*rcSrc).x == 192 )
        {
            (*rcSrc).x = 224;
        }
        else
        {
            (*rcSrc).x = 192;
        }
    }
    if (in->key[SDLK_RIGHT])
    {
        *vx = vitesse;

        if ( (*rcSrc).x == 64 )
        {
            (*rcSrc).x = 96;
        }
        else
        {
            (*rcSrc).x = 64;
        }
    }

}

int EssaiDeplacement(Map* carte,SDL_Rect* perso,int vx,int vy)
{
    SDL_Rect test;
    test = *perso;
    test.x+=vx;
    test.y+=vy;
    if (CollisionDecor(carte,&test)==0)
    {
        *perso = test;
        return 1;
    }
    return 0;
}

void Affine(Map* carte,SDL_Rect* perso,int vx,int vy)
{
    int i;
    for(i=0; i<ABS(vx); i++)
    {
        if (EssaiDeplacement(carte,perso,SGN(vx),0)==0)
            break;
    }
    for(i=0; i<ABS(vy); i++)
    {
        if (EssaiDeplacement(carte,perso,0,SGN(vy))==0)
            break;
    }
}

void Deplace(Map* carte,SDL_Rect* perso,int vx,int vy,int LARGEUR_TILE,int HAUTEUR_TILE)
{
    if (vx>=LARGEUR_TILE || vy>=HAUTEUR_TILE)
    {
        Deplace(carte,perso,vx/2,vy/2,LARGEUR_TILE,HAUTEUR_TILE);
        Deplace(carte,perso,vx-vx/2,vy-vy/2,LARGEUR_TILE,HAUTEUR_TILE);
        return;
    }
    if (EssaiDeplacement(carte,perso,vx,vy)==1)
        return;
    Affine(carte,perso,vx,vy);
}

void Evolue(Input* in,Map* carte,SDL_Rect* perso,int LARGEUR_TILE,int HAUTEUR_TILE,SDL_Rect* rcSrc)
{
    int vx,vy;
    RecupererVecteur(in,&vx,&vy,rcSrc);
    Deplace(carte,perso,vx,vy,LARGEUR_TILE,HAUTEUR_TILE);
}

void AfficherPerso(SDL_Rect* perso,SDL_Surface* screen,SDL_Surface* player,int xscroll,int yscroll,SDL_Rect* rcSrc)
{
    SDL_Rect positionsurecran = *perso;
    SDL_Rect pRcSrc = *rcSrc;
    positionsurecran.x -= xscroll;
    positionsurecran.y -= yscroll;
    SDL_BlitSurface(player,&pRcSrc,screen,&positionsurecran);
}

int main(int argc,char** argv)
{
    SDL_Rect perso,rcSrc,positionTimer, positionGameOver, positionScores,limit;
    SDL_Surface* screen;
    SDL_Surface *texteTimer = NULL , *player = NULL, *texteGameOver = NULL, *texteScores = NULL;
    Map* carte;
    Input in;
    int LARGEUR_TILE,HAUTEUR_TILE;
    LARGEUR_TILE = 24;
    HAUTEUR_TILE = 16;
    bool running=true;
    int tempsSurMenuAcceuil;

    int colorkey; //Pour le traitement de transparence sur les images

    /*Menu*/
    TTF_Font *policeMenu= NULL;

    /*Timer*/
    TTF_Font *policeTimer = NULL;
    SDL_Color couleurNoire = {0, 0, 0};//Couleur pour la police (pas un UINT32)
    int tempsActuel = 0;
    char temps[20] ;

    /*Gameover*/
    char gameOverBuffer[20]="Game over!";
    TTF_Font *policeGameOver = NULL;
    int gameOver = 0;

    /*Gestion nom joueur*/
    int playerHasName=0;
    TTF_Font *policeEntrezPseudo = NULL;
    SDL_Color couleurBleue = {0,0,255};

    /* High score*/
    int score[1] = {0};
    char scoresBuffer[TAILLE_BUFFER_SCORES];
    char playerName[10]="Nimport";
    TTF_Font *policeScores = NULL;
    FILE* fichierScores  = NULL;
    fichierScores=fopen("scores.txt","r+");


    memset(&in,0,sizeof(in));
    SDL_Init(SDL_INIT_VIDEO |SDL_INIT_TIMER);		// preapare SDL
    TTF_Init(); //Démarre SDL TTF
    SDL_WM_SetCaption("Les aventures du Makarov",NULL);//Pour l'écriture au dessus de la fenetre
    screen = SDL_SetVideoMode(LARGEUR_FENETRE,HAUTEUR_FENETRE,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
    carte = ChargerMap("level2.txt",LARGEUR_FENETRE,HAUTEUR_FENETRE);

    /* Initialisation du temps*/
    tempsActuel=SDL_GetTicks();
    snprintf(temps, sizeof(temps), "%d", tempsActuel);

    /*Position initiale du perso*/
    perso.x = 150;
    perso.y = 100;
    perso.w = 24;
    perso.h = 32;

    /*Position initiale du rectangle de limite*/

    limit.x = 200;
	limit.y = 150;
	limit.w = 400;
	limit.h = 300;

    /*Load le sprite du joueur*/
    player = LoadImage32("player.bmp");

    /*Choppe la color key et active la RLE (transparence)*/
    colorkey = SDL_MapRGB(screen->format, 255, 0, 255);
    SDL_SetColorKey(player, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);

    /*Set la frame d'animation*/
    rcSrc.x = 128;
    rcSrc.y = 0;
    rcSrc.w = SPRITE_SIZE;
    rcSrc.h = SPRITE_SIZE;

    /*Charger la police*/
    policeTimer=TTF_OpenFont("blackcastle.ttf",20);
    policeGameOver=TTF_OpenFont("blackcastle.ttf",40);
    policeScores=TTF_OpenFont("keepcalm.ttf",20);
    policeMenu=TTF_OpenFont("blackcastle.ttf",40);


    /*Initialise la position du timer sur le SDL_Screen*/
    positionTimer.x = 5;
    positionTimer.y = 5;

    /*Initialise la position du game over sur le SDL_Screen*/
    positionGameOver.x = 10 ;
    positionGameOver.y = 80 ;

    int i=showacceuil(screen,policeMenu);
    tempsSurMenuAcceuil=SDL_GetTicks();

    if(i==1)
    {
        running=false;
    }

    while( running==true)
    {

            if ( tempsActuel <= count_down_time_in_secs) //Timer non écoulé
            {
                tempsActuel=SDL_GetTicks() - tempsSurMenuAcceuil;
                snprintf(temps, sizeof(temps), "%d", tempsActuel); // On écrit dans la chaîne "temps" le nouveau temps
                SDL_FreeSurface(texteTimer); // On supprime la surface précédente de la mémoire avant d'en charger une nouvelle (IMPORTANT)
                texteTimer = TTF_RenderText_Blended(policeTimer, temps, couleurNoire); // On écrit la chaine temps dans la SDL_Surface

            }
            else //Le compteur est terminé
            {
                texteGameOver = TTF_RenderText_Blended(policeGameOver, gameOverBuffer, couleurNoire);
                gameOver =1;
            }
            if( (perso.x)==1000 && 115<=perso.y<=125 && gameOver !=1 )
            {
               // sprintf(gameOverBuffer,"You win");
               // texteGameOver = TTF_RenderText_Blended(policeGameOver, gameOverBuffer, couleurNoire);
                score[0]=tempsActuel;
                fprintf(fichierScores,"%d",score[0]);
                //fscanf(fichierScores,"%s",scoresBuffer);
                fgets(scoresBuffer,sizeof(scoresBuffer),fichierScores);
                //sprintf(scoresBuffer,"coucou");
                tempsActuel=count_down_time_in_secs + 1;//Pour éviter de rester dans le if du timer
                texteScores = TTF_RenderText_Blended(policeScores, scoresBuffer, couleurNoire);
               i=showmenu(screen,policeMenu,fichierScores);
            }
             if(i==1)
            {
                running=false;
            }
            UpdateEvents(&in,screen,policeMenu,&i);
            MoveMap(carte,&in);
            Evolue(&in,carte,&perso,LARGEUR_TILE,HAUTEUR_TILE,&rcSrc);
            FocusScrollCenter(carte,&perso);
            AfficherMap(carte,screen);
            AfficherPerso(&perso,screen,player,carte->xscroll,carte->yscroll,&rcSrc);
            /*Ecriture du texte en mode blended dans la SDL_SURFACE */
            SDL_BlitSurface(texteTimer, NULL, screen, &positionTimer); /* Blit du texte du timer */
            SDL_BlitSurface(texteGameOver,NULL,screen,&positionGameOver);/*Blit du game over*/
            SDL_BlitSurface(texteScores,NULL,screen,&positionScores);/*Blit du score*/

        SDL_Flip(screen);
        SDL_Delay(5);


    }
    LibererMap(carte);
    fclose(fichierScores);
    TTF_CloseFont(policeTimer);
    TTF_Quit();//Quitte TTF
    SDL_Quit(); //Quitte la SDL
    return 0;
}

