#define _CRT_SECURE_NO_DEPRECATE   // pour visual C++ qui met des warning pour fopen et fscanf : aucun effet negatif pour les autres compilos.
#include <string.h>
#include "fmap.h"

#define CACHE_SIZE 5000

SDL_Surface* LoadImage32(const char* fichier_image)
{
	SDL_Surface* image_result;
	SDL_Surface* image_ram = SDL_LoadBMP(fichier_image);	// charge l'image dans image_ram en RAM
	if (image_ram==NULL)
	{
		printf("Image %s introuvable !! \n",fichier_image);
		SDL_Quit();
		system("pause");
		exit(-1);
	}
	image_result = SDL_DisplayFormat(image_ram);
	SDL_FreeSurface(image_ram);
	return image_result;
}

void ChargerMap_tileset(FILE* F,Map* m)
{
	int numtile,i,j;
	char buf[CACHE_SIZE];  // un buffer, petite mémoire cache
	char buf2[CACHE_SIZE];  // un buffer, petite mémoire cache
	fscanf(F,"%s",buf); // #tileset
	fscanf(F,"%s",buf); // nom du fichier
	m->tileset = LoadImage32(buf);
	fscanf(F,"%d %d",&m->nbtilesX,&m->nbtilesY);
	m->LARGEUR_TILE = m->tileset->w/m->nbtilesX;
	m->HAUTEUR_TILE = m->tileset->h/m->nbtilesY;
	m->props = malloc(m->nbtilesX*m->nbtilesY*sizeof(TileProp));
	for(j=0,numtile=0;j<m->nbtilesY;j++)
	{
		for(i=0;i<m->nbtilesX;i++,numtile++)
		{
			m->props[numtile].R.w = m->LARGEUR_TILE;
			m->props[numtile].R.h = m->HAUTEUR_TILE;
			m->props[numtile].R.x = i*m->LARGEUR_TILE;
			m->props[numtile].R.y = j*m->HAUTEUR_TILE;
			fscanf(F,"%s %s",buf,buf2);
			m->props[numtile].mur = 0;
			if (strcmp(buf2,"mur")==0)
				m->props[numtile].mur = 1;
		}
	}
}

void ChargerMap_level(FILE* F,Map* m)
{
	int i,j;
	char buf[CACHE_SIZE];  // un buffer, petite mémoire cache
	fscanf(F,"%s",buf); // #level
	fscanf(F,"%d %d",&m->nbtiles_largeur_monde,&m->nbtiles_hauteur_monde);
	m->schema = malloc(m->nbtiles_largeur_monde*sizeof(Uint16*));
	for(i=0;i<m->nbtiles_largeur_monde;i++)
		m->schema[i] = malloc(m->nbtiles_hauteur_monde*sizeof(Uint16));
	for(j=0;j<m->nbtiles_hauteur_monde;j++)
	{
		for(i=0;i<m->nbtiles_largeur_monde;i++)
		{
			int tmp;
			fscanf(F,"%d",&tmp);
			if (tmp>=m->nbtilesX*m->nbtilesY)
			{
				printf("level tile hors limite\n");
				SDL_Quit();
				system("pause");
				exit(-1);
			}
			m->schema[i][j] = tmp;
		}
	}
}

Map* ChargerMap(const char* level,int largeur_fenetre,int hauteur_fenetre)
{
	FILE* F;
	Map* m;
	F = fopen(level,"r");
	if (!F)
	{
		printf("fichier %s introuvable !! \n",level);
		SDL_Quit();
		system("pause");
		exit(-1);
	}
	m = malloc(sizeof(Map));
	ChargerMap_tileset(F,m);
	ChargerMap_level(F,m);
	m->largeur_fenetre = largeur_fenetre;
	m->hauteur_fenetre = hauteur_fenetre;
	m->xscroll = 0;
	m->yscroll = 0;
	fclose(F);
	return m;
}

int AfficherMap(Map* m,SDL_Surface* screen)
{
	int i,j;
	SDL_Rect Rect_dest;
	int numero_tile;
	int minx,maxx,miny,maxy;
	minx = m->xscroll / m->LARGEUR_TILE;
	miny = m->yscroll / m->HAUTEUR_TILE;
	maxx = (m->xscroll + m->largeur_fenetre)/m->LARGEUR_TILE;
	maxy = (m->yscroll + m->hauteur_fenetre)/m->HAUTEUR_TILE;
	for(i=minx;i<=maxx;i++)
	{
		for(j=miny;j<=maxy;j++)
		{
			Rect_dest.x = i*m->LARGEUR_TILE - m->xscroll;
			Rect_dest.y = j*m->HAUTEUR_TILE - m->yscroll;
			if (i<0 || i>=m->nbtiles_largeur_monde || j<0 || j>=m->nbtiles_hauteur_monde)
				numero_tile = 0;
			else
				numero_tile = m->schema[i][j];
			SDL_BlitSurface(m->tileset,&(m->props[numero_tile].R),screen,&Rect_dest);
		}
	}
	return 0;
}

int LibererMap(Map* m)
{
	int i;
	SDL_FreeSurface(m->tileset);
	for(i=0;i<m->nbtiles_hauteur_monde;i++)
		free(m->schema[i]);
	free(m->schema);
	free(m->props);
	free(m);
	return 0;
}

int CollisionDecor(Map* carte,SDL_Rect* perso)
{
	int xmin,xmax,ymin,ymax,i,j,indicetile;
	xmin = perso->x / carte->LARGEUR_TILE;
	ymin = perso->y / carte->HAUTEUR_TILE;
	xmax = (perso->x + perso->w -1) / carte->LARGEUR_TILE;
	ymax = (perso->y + perso->h -1) / carte->HAUTEUR_TILE;
	if (xmin<0 || ymin<0 || xmax>=carte->nbtiles_largeur_monde || ymax>=carte->nbtiles_hauteur_monde)
		return 1;
	for(i=xmin;i<=xmax;i++)
	{
		for(j=ymin;j<=ymax;j++)
		{
			indicetile = carte->schema[i][j];
			if (carte->props[indicetile].mur)
				return 1;
		}
	}
	return 0;
}


void ClampScroll(Map* m)
{
	if (m->xscroll<0)
		m->xscroll=0;
	if (m->yscroll<0)
		m->yscroll=0;
	if (m->xscroll>m->nbtiles_largeur_monde*m->LARGEUR_TILE-m->largeur_fenetre-1)
		m->xscroll=m->nbtiles_largeur_monde*m->LARGEUR_TILE-m->largeur_fenetre-1;
	if (m->yscroll>m->nbtiles_hauteur_monde*m->HAUTEUR_TILE-m->hauteur_fenetre-1)
		m->yscroll=m->nbtiles_hauteur_monde*m->HAUTEUR_TILE-m->hauteur_fenetre-1;
}

int FocusScrollCenter(Map* carte,SDL_Rect* perso)
{
	carte->xscroll = perso->x + perso->w/2 - carte->largeur_fenetre/2;
	carte->yscroll = perso->y + perso->h/2 - carte->hauteur_fenetre/2;
	ClampScroll(carte);
	return 0;
}
