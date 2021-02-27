#ifndef __SDI_PSPFB_H
#define __SDI_PSPFB_H

#include "../../sdi.h"

#define MKRGB(r,g,b) ( ((b&0xf8)<<7)+((g&0xf8)<<2)+((r&0xf8)>>3)+0x8000 )

#define SDI_BACKEND_INIT
int sdi_backend_init();

#define SDI_BACKEND_BLIT
int sdi_backend_blit(int x,int y,int height,int width,int alpha,int **img_data);

#define SDI_BACKEND_FLIP
void sdi_backend_flip();

#endif
