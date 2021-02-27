
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

int sdi_backend_scale_blit(int x, int y, int width, int height, int sw, int sh, int ** img_data)
{

}

int sdi_backend_blit(int x,int y,int width,int height,int **img_data)
{
  int i,j,*img = *img_data;

  for (i=0;i < height;i++)
    for (j=0;j < width;j++)
      {
	Uint8 r,g,b,a;
	a = (Uint8)(*img >> 24);
	b = (Uint8)(*img >> 16);
	g = (Uint8)(*img >> 8);
	r = (Uint8)(*img);

	sdi_backend_set_pixel(x+j,y+i,r,g,b,a);

	img++;
      }
}

void sdi_backend_blit_part(int x,int y,int w,int h,int dx,int dy,int *img_data,int width)
{
  int i,j,*img;

  for (i=y;i < y+h;i++)
    for (j=x;j < x+w;j++)
      {
	img = img_data + width*i + j; 
	Uint8 r,g,b,a;
	a = (Uint8)(*img >> 24);
	b = (Uint8)(*img >> 16);
	g = (Uint8)(*img >> 8);
	r = (Uint8)(*img);

	sdi_backend_set_pixel(dx+j-x,dy+i-y,r,g,b,a);
      }
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



