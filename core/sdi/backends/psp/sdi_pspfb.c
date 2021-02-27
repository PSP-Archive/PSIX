#include "sdi_pspfb.h"

#include "../../../psix.h"

char *sdi_backend_vramtop=(char *)0x04000000;
int sdi_backend_frame;

#define PIXELSIZE	1			//in short
#define	LINESIZE	512			//in short
#define	FRAMESIZE	0x44000			//in byte

#define MASK_RED        0x1F
#define MASK_GREEN      0x3E0
#define MASK_BLUE       0x7C00

#define GET_RED(c) ((unsigned char)((c & MASK_RED) << 3))
#define GET_GREEN(c) ((unsigned char)((c & MASK_GREEN) >> 2))
#define GET_BLUE(c) ((unsigned char)((c & MASK_BLUE) >> 7))

int sdi_backend_init()
{










  return 0;
  sdi_backend_frame = 0;
  sceDisplaySetMode(0,SDI_SCREEN_WIDTH,SDI_SCREEN_HEIGHT);
  
}

int sdi_backend_blit(int x,int y,int h,int w,int alpha,int **img_data)
{




  return 0;
  //  char *vptr0 = sdi_backend_vramtop+frame*(x*PIXELSIZE*2+y*LINESIZE*2+0x40000000);
  char *vptr0 = sdi_backend_vramtop+(x*PIXELSIZE*2 + y*LINESIZE*2)+sdi_backend_frame*(480*PIXELSIZE*2+272*LINESIZE*2);
  char *vptr;

  int i,j;
  for (i=0; i < h; i++)
    {
      vptr = vptr0;
    for (j=0; j < w; j++)
      {
	*(unsigned short *)vptr= 0xFFFF;	
	vptr += PIXELSIZE*2;
      }
    vptr0 += LINESIZE*2;
    }
  
  return 0;

}

void sdi_backend_flip()
{



  return 0;
  char *vptr0 = sdi_backend_vramtop+sdi_backend_frame*(480*PIXELSIZE*2+272*LINESIZE*2);
  sceDisplaySetFrameBuf(vptr0, LINESIZE, PIXELSIZE, 0);

  sdi_backend_frame = !sdi_backend_frame;
}
