#ifndef __SDI_GU_H
#define __SDI_GU_H

#include "../../sdi.h"

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <pspgu.h>

#define SDI_BACKEND_INIT
int sdi_backend_init();

#define SDI_BACKEND_BLIT
int sdi_backend_blit(int x,int y,int w,int h,int **img_data);
int sdi_backend_scale_blit(int x,int y,int w,int h,int rw,int rh ,int **img_data);

#define SDI_BACKEND_BLIT_PART
void sdi_backend_blit_part(int x,int y,int w,int h,int dx,int dy,int *img_data,int width);

#define SDI_BACKEND_FLIP
void sdi_backend_flip();

#define SDI_BACKEND_TAKE_SCREENSHOT
void sdi_backend_take_screenshot();

#define SDI_BACKEND_DRAW_SCREENSHOT
void sdi_backend_draw_screenshot();

#endif
