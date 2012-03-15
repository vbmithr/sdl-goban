#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <assert.h>
#include "sdl_utils.h"

#define SCREEN_DEPTH 32

void init()
{
  int rv;
  // Init the SDL
  rv = SDL_Init(SDL_INIT_VIDEO);
  if (rv == -1)
    {
      printf("init: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }

  // Init SDL_image
  rv = IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);
  if (rv == -1)
    {
      printf("init: %s\n", IMG_GetError());
      exit(EXIT_FAILURE);
    }
}

void draw_lines(SDL_Surface *surf, SDL_Rect *r, int n)
{
  int i,j;
  int stone_size = r->h / (n-1);
  Uint32 black = SDL_MapRGB(surf->format, 0, 0, 0);
  SDL_Rect rect;

  rect.y = r->y;
  rect.w = 1;
  rect.h = r->h;

  for (i=0; i<n; i++)
    {
      rect.x = r->x+i*stone_size;
      SDL_FillRect(surf, &rect, black);
    }

  rect.x = r->x;
  rect.h = 1;
  rect.w = r->w+1;

  for (i=0; i<n; i++)
    {
      rect.y = r->y+i*stone_size;
      SDL_FillRect(surf, &rect, black);
    }

  switch (n)
    {
    case 9:
      for (i=0;i<9;i++)
        for(j=0;j<9;j++)
          if ( (i==j && (i==4||i==6)) || ((i == 6 || i == 2) && (j == 6 || j == 2)))
            filledCircleColor(surf, r->x+i*stone_size, r->y+j*stone_size, 3, 0x000000ff);

      break;
    case 13:
      for (i=0;i<13;i++)
        for(j=0;j<13;j++)
          if ((i==3 || i==6 || i==9) && (j==3 || j==6 || j==9))
            filledCircleColor(surf, r->x+i*stone_size, r->y+j*stone_size, 3, 0x000000ff);
      break;
    case 19:
      for (i=0;i<19;i++)
        for(j=0;j<19;j++)
          if ((i==3 || i==9 || i==15) && (j==3 || j==9 || j==15))
            filledCircleColor(surf, r->x+i*stone_size, r->y+j*stone_size, 3, 0x000000ff);
      break;
    default:
      // Do nothing
      break;
    }

}

// Return the dst SDL_Rect for blitting
SDL_Rect get_stone_coords(int size, int stone_size,
                          SDL_Rect *game_rect, SDL_Event *evt, SDL_Rect *old_coords)
{
  double dist, best_dist = 100000;
  int best_x, best_y, cur_x, cur_y;
  int i,j, besti, bestj;
  if (old_coords == NULL)
    {
      // Have to recompute everything
      for(i=0;i<size;i++)
        for(j=0;j<size;j++)
          {
            cur_x = game_rect->x + i*stone_size;
            cur_y = game_rect->y + j*stone_size;
            dist = sqrt( pow(cur_x-evt->motion.x, 2) + pow(cur_y-evt->motion.y, 2) );
            if (dist < best_dist)
              {
                best_x = cur_x;
                best_y = cur_y;
                best_dist = dist;
                besti = i;
                bestj = j;
              }
          }
    }

  else
    {
      // TO BE IMPL.
    }
  return SDLU_MakeRect(best_x-stone_size/2, best_y-stone_size/2, besti, bestj);
}

void draw_goban(SDL_Surface *empty, SDL_Surface *dst, SDL_Surface *stones,
                SDL_Rect *gamerect,
                char *goban, int size, int stone_size)
{
  int i,j;
  SDL_Rect blackr = SDLU_MakeRect(stone_size, 0, stone_size, stone_size);
  SDL_Rect whiter = SDLU_MakeSquare(0, 0, stone_size);
  SDL_Rect dstr;

  // Blit empty goban
  SDL_BlitSurface(empty, NULL, dst, NULL);
  for(j=0;j<size;j++)
    for(i=0;i<size;i++)
      {
        SDLU_SetRect(&dstr, gamerect->x+i*stone_size-stone_size/2,
                     gamerect->y+j*stone_size-stone_size/2,
                     stone_size, stone_size);

        switch(goban[j*size+i])
          {
          case 1: // black
            SDL_BlitSurface(stones, &blackr, dst, &dstr);
            break;
          case 2: // white
            SDL_BlitSurface(stones, &whiter, dst, &dstr);
            break;
          case -1: // black prisoner
            break;
          case -2: // white prisoner
            break;
          default:
            break;
          }
      }
}

int main(int argc, char **argv)
{

  SDL_Surface *bg_img = NULL, *screen = NULL, *stones = NULL;
  SDL_Event event;
  SDL_Rect gamerect, gobanrect, white_rect, black_rect, cur_rect;
  int size, stone_size, cur_color = 1;
  int quit = 0;
  char goban[361] = {0};

  if(argc < 2)
    {
      printf("Usage: %s <goban_size>\n", argv[0]);
      exit(EXIT_SUCCESS);
    }
  size = strtol(argv[1], NULL, 10);

  // Initialization of SDL and SDL_image
  init();

  stones = IMGU_Load("stones_small.png");

  stone_size = stones->h;

  // Set some rects
  SDLU_SetSquare(&white_rect, 0, 0, stones->h);
  SDLU_SetRect(&black_rect, stones->h, 0, stones->h, stones->h);
  SDLU_SetSquare(&gamerect, 2*stone_size, 2*stone_size, (size-1)*stone_size);
  SDLU_SetSquare(&gobanrect, stone_size, stone_size, (size+1)*stone_size);
  cur_rect = black_rect;

  screen = SDL_SetVideoMode((size+3)*stone_size, (size+3)*stone_size, 0, SDL_SWSURFACE);

  //SDL_SetClipRect(screen, &gobanrect);

  SDL_WM_SetCaption("eGoban SIMU", NULL);

  SDL_Surface *stones_opt = SDL_DisplayFormatAlpha(stones);
  SDL_FreeSurface(stones);
  stones = stones_opt;

  // infer sizes
  bg_img = IMGU_LoadOptimize("WOOD_I15.jpg");
  SDLU_PrintPixelFormat(bg_img->format);

  // set the background
  SDLU_ApplyTile(bg_img, screen);

  // draw the lines
  draw_lines(screen, &gamerect, size);

  // Save the empty goban in a Surface
  SDL_Surface *empty_goban;
  empty_goban = SDLU_CopySurface(screen);
  SDL_Flip(screen);




  SDL_Rect old_coords;
  SDL_Rect new_coords;
  while (!quit)
    {
      while (SDL_PollEvent(&event))
        {
          printf("Event type %d received\n", event.type);
          switch(event.type)
            {
            case SDL_QUIT:
              quit = 1;
              break;
            case SDL_SYSWMEVENT:
              quit = 1;
              break;

            case SDL_KEYDOWN:
              switch(event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                  quit = 1; break;
                case SDLK_q:
                  quit = 1; break;
                case SDLK_b:
                  cur_rect = black_rect;
                  cur_color = 1;
                  draw_goban(empty_goban, screen, stones, &gamerect, goban, size, stone_size);
                  SDL_BlitSurface(stones, &cur_rect, screen, &new_coords);
                  SDL_Flip(screen);

                  break;
                case SDLK_w:
                  cur_rect = white_rect;
                  cur_color = 2;
                  draw_goban(empty_goban, screen, stones, &gamerect, goban, size, stone_size);
                  SDL_BlitSurface(stones, &cur_rect, screen, &new_coords);
                  SDL_Flip(screen);
                  break;
                case SDLK_e: // Empty goban
                  memset(goban, 0, 361);
                  draw_goban(empty_goban, screen, stones, &gamerect, goban, size, stone_size);
                  SDL_Flip(screen);
                default:
                  break;
                };
              break;

            case SDL_MOUSEMOTION:
              new_coords = get_stone_coords(size, stone_size, &gamerect, &event, NULL);

              if (new_coords.x != old_coords.x || new_coords.y != old_coords.y)
                {
                  // Reset the screen to empty goban
                  draw_goban(empty_goban, screen, stones, &gamerect, goban, size, stone_size);
                  SDL_BlitSurface(stones, &cur_rect, screen, &new_coords);
                  SDL_Flip(screen);
                }
              old_coords = new_coords;
              break;

            case SDL_MOUSEBUTTONUP:
              new_coords = get_stone_coords(size, stone_size, &gamerect, &event, NULL);
              switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                  goban[new_coords.h*size+new_coords.w] = cur_color;
                  break;
                case SDL_BUTTON_RIGHT:
                  goban[new_coords.h*size+new_coords.w] = 0;
                  break;
                case SDL_BUTTON_MIDDLE:
                  // Change stone color.
                  break;
                }
              draw_goban(empty_goban, screen, stones, &gamerect, goban, size, stone_size);
            }
        }
    }

  SDL_FreeSurface(bg_img);
  SDL_FreeSurface(stones);

  IMG_Quit();
  SDL_Quit();

  return 0;
}
