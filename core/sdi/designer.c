#include <stdio.h>
#include "sdi.h"
#include <SDL_image.h>

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

  int numpix = img_sdl->h * img_sdl->w;

  int i,j;
  for (i=0; i < numpix; i++)
    {
      int pixel = *((unsigned int *)((unsigned char *)img_sdl->pixels + i * img_sdl->format->BytesPerPixel));

      Uint8 red,green,blue,alpha;
      SDL_GetRGBA(pixel,fmt,&red,&green,&blue,&alpha);
      //printf("Color: R[%d] G[%d] B[%d] A[%d]\n",red,green,blue,alpha); 
     
      *pixel_out++ = (alpha << 24) | (blue << 16) | (green << 8) | red;
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

  sdi_set_image(name,img,img_sdl->w,img_sdl->h,img_sdl->w,img_sdl->h);  

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
  printf("  4) Overlay\n");
  printf("  5) Index\n");
  printf("  6) Enable/Disable\n");
  printf("  7) Change image\n");
  printf("  8) Remove object\n");
  printf("  9) Move object below another\n");
  printf(" 11) Return To Main Menu\n  ");

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
	case '4': sdi_set_overlay(name,value); break;
	case '5': sdi_set_index(name,value); break;
	case '6': sdi_set_enabled(name,value); break;
	}
      sdi_print_object(name);
    }
  else if (c == '8')
    sdi_del(name);
  else if (c == '7')
    {
      char path[256];
      int x,y;
      char num[4];
      printf("Enter an image path: ");
      scanf("%s%*c",path);
      
      SDL_Surface *img_sdl;
      img_sdl = IMG_Load(path);
      
      SDL_LockSurface(img_sdl);  
      
      int *img = (int*)malloc(sizeof(int)*img_sdl->h*img_sdl->w);
      int *pixel_in = (int*)img_sdl->pixels;
      int *pixel_out = img;
      
      SDL_PixelFormat *fmt = img_sdl->format;
      
      int numpix = img_sdl->h * img_sdl->w;
      
      int i,j;
      for (i=0; i < numpix; i++)
	{
	  int pixel = *((unsigned int *)((unsigned char *)img_sdl->pixels + i * img_sdl->format->BytesPerPixel));
	  Uint8 red,green,blue,alpha;
	  SDL_GetRGBA(pixel,fmt,&red,&green,&blue,&alpha);
	  *pixel_out++ = (alpha << 24) | (blue << 16) | (green << 8) | red;
	}
      
      SDL_UnlockSurface(img_sdl);  
      
      printf("Enter X position: ");
      scanf("%s%*c",num);
      x = atoi(num);
      
      printf("Enter Y position: ");
      scanf("%s%*c",num);
      y = atoi(num);
      
      sdi_set_image(name,img,img_sdl->w,img_sdl->h,img_sdl->w,img_sdl->h);  

      free(img);
      SDL_FreeSurface(img_sdl);      
    }
  else if (c == '9')
    {
      char below[256];
      printf("Enter the object name: ");
      scanf("%s%*c",below);
      sdi_move_below(name,below);
      sdi_print_object(name);
    }
}

void save_state()
{
  sdi_dump_state("psix.theme");
}

void load_state()
{
  sdi_load_state("psix.theme");
}

int main(int argc, char **argv)
{
  sdi_error("Testing sdi_error()...done");

  printf("Initializing SDI...");
  sdi_init();
  printf("SDI Initialized\n");

  //sdi_new("t");
  //sdi_set_enabled("t",0);
  //sdi_load_image("t","m.jpg");
  //sdi_set_position("t",0,0);
  //printf("loaded image\n");

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
      printf("7) List objects\n");
      printf("8) Set MAC address\n");
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
	case '7': sdi_print_objects(); break;
	  //	case '8': set_mac(); break;
	case '9': case 'q': done = 1; break;
	}
    }

  sdi_shutdown();
  return 0;
}

void test()
{
  sdi_new("t");
  sdi_set_x("t",100);
  sdi_set_y("t",0);
  sdi_set_enabled("t",1);
  sdi_print_wrap("t","font_orange","PSIX Internal BETA",50);
}
