#include "sdl_utils.h"

void SDLU_PrintPixelFormat(SDL_PixelFormat *fmt)
{
  printf("bpp:\t%u\nRmask:\t%08x\nGmask:\t%08x\nBmask:\t%08x\nAmask:\t%08x\n",
         fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask,
         fmt->Amask);
  printf("Rshift:\t%u\nGshift:\t%u\nBshift:\t%u\nAshift:\t%u\n",
         fmt->Rshift, fmt->Gshift, fmt->Bshift, fmt->Ashift);
  printf("Ckey:\t%08x\nAlpha:\t%02x\n\n",
         fmt->colorkey, fmt->alpha);
}

Uint32 SDLU_ARGB_of_RBGA(Uint32 RGBA)
{
  Uint32 A = (RGBA&0x000000ff)<<6;
  return (RGBA>>2)&A;
}

Uint32 SDLU_RBGA_of_ARGB(Uint32 ARGB)
{
  Uint32 A = ARGB&0xff000000;
  return (ARGB<<2)&(A>>6);
}

SDL_Rect SDLU_MakePoint(int x, int y)
{
  SDL_Rect p;
  p.x = x;
  p.y = y;
  return p;
}

SDL_Rect SDLU_CopyPoint(SDL_Rect *src, int offx, int offy)
{
  SDL_Rect p;
  p.x = src->x;
  p.y = src->y;
  p.x += offx;
  p.y += offy;
  return p;
}

SDL_Rect *SDLU_MovePoint(SDL_Rect *src, int offx, int offy)
{
  src->x = src->x-offx;
  src->y = src->y-offy;
  return src;
}

SDL_Rect SDLU_MakeRect(int x, int y, int w, int h)
{
  SDL_Rect r;
  r.x = x; r.y = y; r.w = w; r.h = h;
  return r;
}

SDL_Rect SDLU_MakeSquare(int x, int y, int w)
{
  return SDLU_MakeRect(x, y, w, w);
}

void SDLU_SetRect(SDL_Rect *r, int x, int y, int w, int h)
{
  r->x = x;
  r->y = y;
  r->w = w;
  r->h = h;
}

void SDLU_SetSquare(SDL_Rect *r, int x, int y, int w)
{
  SDLU_SetRect(r, x, y, w, w);
}

SDL_Surface *SDLU_CopySurface(SDL_Surface *f)
{
  SDL_Surface *s;
  s = SDL_CreateRGBSurface(f->flags,
                           f->w, f->h,
                           f->format->BitsPerPixel,
                           f->format->Rmask,
                           f->format->Bmask,
                           f->format->Gmask,
                           f->format->Amask);
  SDL_BlitSurface(f, NULL, s, NULL);
  return s;
}

int SDLU_ApplySurface(SDL_Surface* src, SDL_Rect *clip, SDL_Surface *dst, int x, int y)
{
  SDL_Rect r = SDLU_MakePoint(x, y);
  return SDL_BlitSurface(src, clip, dst, &r);
}

void SDLU_ApplyTile(SDL_Surface *src, SDL_Surface *dst)
{
  int i,j;
  int max_i = dst->w / src->w;
  int max_j = dst->h / src->h;
  for(i=0;i<=max_i;i++)
    for (j=0;j<=max_j; j++)
      SDLU_ApplySurface(src, NULL, dst, i*src->w, j*src->h);
}

SDL_Surface *IMGU_Load(char *filename)
{
  SDL_Surface *img = NULL;
  img = IMG_Load(filename);
  if (!img)
    {
      fprintf(stderr, "IMG_Load: %s\n", IMG_GetError());
    }
  return img;
}

SDL_Surface *IMGU_LoadOptimize(char *filename)
{
  SDL_Surface *loadedImage = NULL;
  SDL_Surface *optimizedImage = NULL;

  loadedImage = IMG_Load(filename);
  if (!loadedImage)
    {
      fprintf(stderr, "IMG_Load: %s\n", IMG_GetError());
      return NULL;
    }

  optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
  if (!optimizedImage)
    {
      fprintf(stderr, "SDL_DisplayFormat: %s\n", SDL_GetError());
      return loadedImage;
    }

  SDL_FreeSurface(loadedImage);
  return optimizedImage;
}
