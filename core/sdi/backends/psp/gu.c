#include "gu.h"

#include "../../../psix.h"

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4)
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) 
#define FRAMEBUFFER_SIZE (PSP_LINE_SIZE*SCREEN_HEIGHT*4)
#define	PSP_LINE_SIZE 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

typedef u32 Color;
#define A(color) ((u8)(color >> 24 & 0xFF))
#define B(color) ((u8)(color >> 16 & 0xFF))
#define G(color) ((u8)(color >> 8 & 0xFF))
#define R(color) ((u8)(color & 0xFF))

typedef struct
{
	unsigned short u, v;
	short x, y, z;
} Vertex;

unsigned int __attribute__((aligned(16))) list[262144];
static int dispBufferNumber;
Color* g_vram_base = (Color*) (0x40000000 | 0x04000000);

int sdi_backend_np2(int width);


int sdi_backend_init()
{
  dispBufferNumber = 0;
  
  sceGuInit();
  
  sceGuStart(GU_DIRECT, list);
  sceGuDrawBuffer(GU_PSM_8888, (void*)FRAMEBUFFER_SIZE, PSP_LINE_SIZE);
  sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void*)0, PSP_LINE_SIZE);
  sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
  sceGuDepthBuffer((void*) (FRAMEBUFFER_SIZE*2), PSP_LINE_SIZE);
  sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
  sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
  sceGuDepthRange(0xc350, 0x2710);
  sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  sceGuEnable(GU_SCISSOR_TEST);
  sceGuAlphaFunc(GU_GREATER, 0, 0xff);
  sceGuEnable(GU_ALPHA_TEST);
  sceGuDepthFunc(GU_GEQUAL);
  sceGuEnable(GU_DEPTH_TEST);
  sceGuFrontFace(GU_CW);
  sceGuShadeModel(GU_SMOOTH);
  sceGuEnable(GU_CULL_FACE);
  sceGuEnable(GU_TEXTURE_2D);
  sceGuEnable(GU_CLIP_PLANES);
  sceGuTexMode(GU_PSM_8888, 0, 0, 0);
  sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
  sceGuTexFilter(GU_NEAREST, GU_NEAREST);
  sceGuAmbientColor(0xffffffff);
  sceGuEnable(GU_BLEND);
  sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
  sceGuFinish();
  sceGuSync(0, 0);
  
  sceDisplayWaitVblankStart();
  sceGuDisplay(GU_TRUE);

  return 0;
}

/* rw,rh = render height/width */
int sdi_backend_blit(int x,int y,int w,int h,int **img_data)
{
  int tw = sdi_backend_np2(w);
  int th = sdi_backend_np2(h);

  sceKernelDcacheWritebackInvalidateAll();
  sceGuStart(GU_DIRECT, list);
  sceGuTexImage(0, tw, th, tw, (void*) *img_data);
  float u = 1.0f / ((float)tw);
  float v = 1.0f / ((float)th);
  sceGuTexScale(u, v);
  
  int j = 0;
  while (j < w) {
    Vertex* vertices = (Vertex*) sceGuGetMemory(2 * sizeof(Vertex));
    int sliceWidth = 64;
    if (j + sliceWidth > w) sliceWidth = w - j;
    vertices[0].u = j;
    vertices[0].v = 0;
    vertices[0].x = x + j;
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[1].u = j + sliceWidth;
    vertices[1].v = h;
    vertices[1].x = x + j + sliceWidth;
    vertices[1].y = y + h;
    vertices[1].z = 0;
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
    j += sliceWidth;
  }
  
  sceGuFinish();
  sceGuSync(0, 0);

  return 0;
}


int sdi_backend_scale_blit(int x,int y,int w,int h,int rw,int rh ,int **img_data)
{
  int tw = sdi_backend_np2(w);
  int th = sdi_backend_np2(h);

  float sw = ((float)rw)/((float)w);
  float sh = ((float)rh)/((float)h);

  sceKernelDcacheWritebackInvalidateAll();
  sceGuStart(GU_DIRECT, list);
  sceGuTexImage(0, tw, th, tw, (void*) *img_data);
  float u = 1.0f / ((float)tw);
  float v = 1.0f / ((float)th);
  sceGuTexScale(u, v);
  
  int j = 0;
  while (j < w) {
    Vertex* vertices = (Vertex*) sceGuGetMemory(2 * sizeof(Vertex));
    int sliceWidth = 64;
    if (j + sliceWidth > w) sliceWidth = w - j;
    vertices[0].u = j;
    vertices[0].v = 0;
    vertices[0].x = x + (int)((float)j*sw);
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[1].u = j + sliceWidth;
    vertices[1].v = h;
    vertices[1].x = x + (int)((float)(j + sliceWidth)*sw);
    vertices[1].y = y + (int)((float)h*sh);
    vertices[1].z = 0;
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
    j += sliceWidth;
  }
  
  sceGuFinish();
  sceGuSync(0, 0);

  return 0;
}

void sdi_backend_flip()
{
  sceGuSwapBuffers();
  dispBufferNumber ^= 1;
}

/*
void sdi_backend_take_screenshot()
{
  int *ip = sdi_backend_screenshot;
  u32 *vram;

  vram = getVramDrawBuffer();

  int i;
  for (i=0; i < SCREEN_HEIGHT; i++)
    {
      memcpy(ip,vram,SCREEN_WIDTH*4);
      ip += SCREEN_WIDTH;
      vram += PSP_LINE_SIZE;
    }
  
}

void sdi_backend_draw_screenshot()
{
  int *ip = sdi_backend_screenshot;
  u32 *vram;
  
  vram = getVramDrawBuffer();

  int i;
  for (i=0; i < SCREEN_HEIGHT; i++)
    {
      memcpy(vram,ip,SCREEN_WIDTH*4);
      ip += SCREEN_WIDTH;
      vram += PSP_LINE_SIZE;
    }
}
*/

int sdi_backend_np2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}

