#ifndef __SDL_UTILS__
#define __SDL_UTILS__

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

void SDLU_PrintPixelFormat(SDL_PixelFormat *fmt);

Uint32 SDLU_ARGB_of_RBGA(Uint32 RGBA);
Uint32 SDLU_RBGA_of_ARGB(Uint32 ARGB);

SDL_Rect SDLU_MakePoint(int x, int y);
SDL_Rect SDLU_CopyPoint(SDL_Rect *src, int offx, int offy);
SDL_Rect * SDLU_MovePoint(SDL_Rect *src, int offx, int offy);

SDL_Rect SDLU_MakeRect(int x, int y, int w, int h);
SDL_Rect SDLU_MakeSquare(int x, int y, int w);

void SDLU_SetRect(SDL_Rect *r, int x, int y, int w, int h);
void SDLU_SetSquare(SDL_Rect *r, int x, int y, int w);

SDL_Surface* SDLU_CopySurface(SDL_Surface *f);
int SDLU_ApplySurface(SDL_Surface* src, SDL_Rect *clip, SDL_Surface *dst, int x, int y);
void SDLU_ApplyTile(SDL_Surface *src, SDL_Surface *dst);

SDL_Surface *IMGU_Load(char *filename);
SDL_Surface *IMGU_LoadOptimize(char *filename);

#endif
