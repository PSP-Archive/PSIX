#ifndef __SDI_SDL_H
#define __SDI_SDL_H


#include <stdlib.h>
#include <SDL.h>
#include "../../sdi.h"

#define SDI_BACKEND_INIT
int sdi_backend_init();

#define SDI_BACKEND_BLIT
int sdi_backend_blit(int x,int y,int width,int height,int **img_data);

#define SDI_BACKEND_BLIT_PART
void sdi_backend_blit_part(int x,int y,int w,int h,int dx,int dy,int *img_data,int width);

#define SDI_BACKEND_FLIP
void sdi_backend_flip();

#define SDI_BACKEND_SHUTDOWN
int sdi_backend_shutdown();

#endif
