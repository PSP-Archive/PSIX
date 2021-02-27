#include <stdio.h>
#include "sdi.h"
#include <SDL/SDL_image.h>

extern SDL_Surface *screen;

int create_pixel(SDL_PixelFormat *fmt, Uint8 red, Uint8
 green, Uint8 blue, Uint8 alpha)
{
  int value;
  value = ((red >> fmt->Rloss) << fmt->Rshift) + ((green >> fmt->Gloss) << fmt->Gshift) + ((blue >> fmt->Bloss) << fmt->Bshift) +  ((alpha >> fmt->Aloss) << fmt->Ashift);
  return value;
}

void create_object()
{
  char name[256],path[256],num[4];
  int x,y;
  printf("Enter the object name: ");
  scanf("%s%*c",name);
  sdi_new(name);

  printf("Enter an image path: ");
  scanf("%s%*c",path);

  SDL_Surface *img_sdl;
  img_sdl = IMG_Load(path);

  SDL_LockSurface(img_sdl);  

  int *img = (int*)malloc(sizeof(int)*img_sdl->h*img_sdl->w);
  int *pixel_in = (int*)img_sdl->pixels;
  int *pixel_out = img;
  
  SDL_PixelFormat *fmt = img_sdl->format;

  int i,j;
  for (i=0; i < img_sdl->w; i++)
    for (j=0; j < img_sdl->h; j++)
      {
	int pixel = pixel_in[(img_sdl->pitch/img_sdl->format->BytesPerPixel * j + i)];
	Uint8 red = ((pixel & fmt->Rmask ) >> fmt->Rshift) << fmt->Rloss;
	Uint8 green = ((pixel & fmt->Gmask) >> fmt->Gshift) << fmt->Gloss;
	Uint8 blue = ((pixel & fmt->Bmask) >> fmt->Bshift) << fmt->Bloss;
	Uint8 alpha = ((pixel & fmt->Amask) >> fmt->Ashift) << fmt->Aloss;

	//	printf("Color: R[%d] G[%d] B[%d] A[%d]\n",red,green,blue,alpha);
	*pixel_out++ = (red << 24) | (green << 16) | (blue << 8) | alpha;
      }

  SDL_UnlockSurface(img_sdl);  

  printf("Enter X position: ");
  scanf("%s%*c",num);
  x = atoi(num);

  printf("Enter Y position: ");
  scanf("%s%*c",num);
  y = atoi(num);

  sdi_set_x(name,x);
  sdi_set_y(name,y);

  sdi_set_image(name,&img,img_sdl->w,img_sdl->h);  

  SDL_FreeSurface(img_sdl);
}

void view_object()
{
  char name[256];
  printf("Enter the object name: ");
  scanf("%s%*c",name);

  sdi_print_object(name);
}

void modify_object()
{
  char name[256];
  printf("Enter the object name: ");
  scanf("%s%*c",name);

  printf("  Choose a property:\n");
  printf("  1) Name\n");
  printf("  2) X\n");
  printf("  3) Y\n");
  printf("  4) Alpha\n");
  printf("  5) Index\n");
  printf("  6) Enable/Disable\n");
  printf("  7) Return To Main Menu\n  ");

  char c = getchar();

  if (c == '1')
    {
      char new_name[256];
      printf("Enter a new object name: ");
      scanf("%s%*c",new_name);
      sdi_rename_object(name,new_name);
      sdi_print_object(new_name);
    }
  else if (c == '2' || c == '3' || c == '4' || c == '5' || c == '6')
    {
      char num[4];
      printf("Enter a new value: ");
      scanf("%s%*c",num);
      int value = atoi(num);
      
      switch(c)
	{
	case '2': sdi_set_x(name,value); break;
	case '3': sdi_set_y(name,value); break;
	case '4': sdi_set_alpha(name,value); break;
	case '5': sdi_set_index(name,value); break;
	case '6': sdi_set_enabled(name,value); break;
	}
      sdi_print_object(name);
    }
  
}

void save_state()
{

}

void load_state()
{

}

int main(int argc, char **argv)
{
  sdi_error("Testing sdi_error()...done");

  printf("Initializing SDI...");
  sdi_init();
  printf("SDI Initialized\n");

  int done=0;
  while (!done)
    {
      sdi_draw();		/* show what we've got */

      printf("\n\nChoose an option:\n");
      printf("1) Create an object\n");
      printf("2) View an object\n");
      printf("3) Modify an object\n");
      printf("4) Save state\n");
      printf("5) Load state\n");
      printf("6) Redraw\n");
      printf("8) List objects\n");
      printf("9) Quit\n");

      char c;
      while ((c = getchar()) == '\n') {} 

      switch(c)
	{
	case '1': create_object(); break;
	case '2': view_object(); break;
	case '3': modify_object(); break;
	case '4': save_state(); break;
	case '5': load_state(); break;
	case '6': break;
	case '8': sdi_print_objects(); break;
	case '9': case 'q': done = 1;
	}
    }

  sdi_shutdown();
  return 0;
}

