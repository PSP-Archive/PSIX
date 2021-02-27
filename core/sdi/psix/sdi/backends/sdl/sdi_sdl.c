
#include "sdi_sdl.h"

/* get a buffer for the screen */
SDL_Surface *screen;
int *screen_pix;

int sdi_backend_init()
{
  /* intialize SDL */
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
    return -1;
  }

  screen = SDL_SetVideoMode(SDI_SCREEN_WIDTH,SDI_SCREEN_HEIGHT,SDI_SCREEN_BPP, SDL_HWSURFACE|SDL_ANYFORMAT);
  if (!screen) {
    fprintf(stderr, "Could not create a screen surface: %s\n", SDL_GetError());
    return -1;
  }

  SDL_WM_SetCaption("SDI", "MAXIMIZE ME");

  screen_pix = (int*)screen->pixels;

  return 0;
}

int sdi_backend_create_pixel(SDL_PixelFormat *fmt, Uint8 red, Uint8
 green, Uint8 blue, Uint8 alpha)
{
  return ((red >> fmt->Rloss) << fmt->Rshift) + ((green >> fmt->Gloss) << fmt->Gshift) + ((blue >> fmt->Bloss) << fmt->Bshift) +  ((alpha >> fmt->Aloss) << fmt->Ashift);
}

void sdi_backend_set_pixel(int x, int y, Uint8 red1, Uint8 green1, Uint8 blue1, Uint8 alpha)
{
  int off = (screen->pitch/4 * y + x);
  int pixel = screen_pix[off];
  
  SDL_PixelFormat *fmt = screen->format;
  Uint8 red2 = ((pixel & fmt->Rmask ) >> fmt->Rshift) << fmt->Rloss;
  Uint8 green2 = ((pixel & fmt->Gmask) >> fmt->Gshift) << fmt->Gloss;
  Uint8 blue2 = ((pixel & fmt->Bmask) >> fmt->Bshift) << fmt->Bloss;
  
  Uint8 r = ((red1*alpha) + (red2*(255-alpha))) / 255;
  Uint8 g = ((green1*alpha) + (green2*(255-alpha))) / 255;
  Uint8 b = ((blue1*alpha) + (blue2*(255-alpha))) / 255;

  int col = sdi_backend_create_pixel(screen->format,r,g,b,0);
  screen_pix[off] = col;
}

int sdi_backend_blit(int x,int y,int height,int width,int alpha,int **img_data)
{/*
  int *raw_pixels;
  int col,off;
  raw_pixels = (int*)screen->pixels;*/
  int i,j,*img = *img_data;
  /*  SDL_PixelFormat *fmt = screen->format;

  SDL_Surface *surf = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA,SDI_SCREEN_WIDTH,SDI_SCREEN_HEIGHT,32,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask);
  SDL_SetAlpha( surf, 0, 0 );

  SDL_LockSurface(surf);
  
  int *surf_pix = (int*)surf->pixels;
  */
  for (i=0;i < width;i++)
    for (j=0;j < height;j++)
      {
	Uint8 r,g,b,a;
	r = (Uint8)(*img >> 24);
	g = (Uint8)(*img >> 16);
	b = (Uint8)(*img >> 8);
	a = (Uint8)(*img);

	sdi_backend_set_pixel(x+i,y+j,r,g,b,a);

	/*	
	col = sdi_backend_create_pixel(screen->format,r,g,b,a);
	off = (screen->pitch/4 * j + i);
	surf_pix[off] = col;
	*/
	img++;
      }

  /*

  SDL_Rect dest;
  SDL_Rect src;
  dest.x = x;
  dest.y = y;
  dest.w = width;
  dest.h = height;
  src.w = width;
  src.h = height;
  src.x = 0;
  src.y = 0;

  SDL_UnlockSurface(surf);

  SDL_BlitSurface(surf, &src, screen, &dest);

  SDL_UpdateRect(screen,x,y,height,width);
  SDL_FreeSurface(surf);
  */
}

void sdi_backend_flip()
{
  SDL_Flip(screen);  
  return;

  SDL_Event event;
  do {
    SDL_PollEvent(&event);
    } while (event.key.keysym.sym != SDLK_ESCAPE);
}

int sdi_backend_shutdown()
{
  SDL_Quit();
}



