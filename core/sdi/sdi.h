#ifndef __SDI_H
#define __SDI_H

#define SDI_SCREEN_WIDTH  480
#define SDI_SCREEN_HEIGHT 272
#define SDI_SCREEN_BPP    32
#define SDI_MAX_IMAGE_SIZE 512

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef PC
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspiofilemgr.h>
#include <pspiofilemgr_fcntl.h>
#else
typedef char u8;
#endif

#ifdef SDL
#include "backends/sdl/sdi_sdl.h"
#endif

#ifdef PC
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif


#ifdef PSPGU
#include "backends/psp/gu.h"
#endif

/* Initialize SDI */
int sdi_init();

/* Reports a SDI error */
void sdi_error(char *strerror);

/* Draws to the base display */
int sdi_draw();

/* Draws the overlays */
int sdi_draw_overlays();

/* Draws part of an image to the screen */
void sdi_blit_part(int x, int y, int w, int h, int dx, int dy, char *name);

/* Creates a new object */
int sdi_new(char *name);

/* Deletes an object */
int sdi_del(char *name);

/* chops one px from the right of a char */
int sdi_print_line_max_compress(char *name, char *font_name, char *str, int max_w, int chop, int line);
int sdi_print_line_max(char *name, char *font_name, char *str, int max_w, int line);
int sdi_print_wrap_compress(char *name, char *font_name, char *str, int max_w,int chop);
int sdi_print_wrap(char *name, char *font_name, char *str, int w);
int sdi_print(char *name, char *font_name, char *str);
int sdi_print_(char *name, char *font_name, char *str, int max_w, int chop, int line);

/* Prints out all the object information */
void sdi_print_object(char *name);
void sdi_print_objects();

/* Sets the object image */
int sdi_set_image(char *name,int *img, int w, int h,int textureWidth,int textureHeight);
int *sdi_get_image(char *name);

/* Dump and load the state */
void sdi_dump_state(char *filename);
void sdi_load_state(char *filename);
void sdi_load_from_state(char*obj,char *filename);

/* Sets/Gets image properties */
void sdi_set_height(char *name,int height);
int  sdi_get_height(char *name);
void sdi_set_width(char *name,int width);
int  sdi_get_width(char *name);
void sdi_set_x(char *name,int x);
int  sdi_get_x(char *name);
int * sdi_get_x_ptr(char *name);
void sdi_set_y(char *name,int y);
int  sdi_get_y(char *name);
void sdi_set_alpha(char *name,short alpha);
int  sdi_get_alpha(char *name);
void sdi_set_overlay(char *name,int overlay);
int  sdi_get_overlay(char *name);
void sdi_set_index(char *name,int index);
int  sdi_get_index(char *name);
void sdi_set_enabled(char *name,int enabled);
int  sdi_get_enabled(char *name);
void sdi_set_position(char *name,int x, int y);
int  sdi_get_texwidth(char *name);
int  sdi_get_texheight(char *name);
void sdi_set_display_height(char *name,int height);
int  sdi_get_display_height(char *name);
void sdi_set_display_width(char *name,int width);
int  sdi_get_display_width(char *name);
int  sdi_get_scale(char *name);
void  sdi_set_scale(char *name,int scale);
/* rename an object */
void sdi_rename_object(char *name, char *new_name);

/* takes a screenshot and stores in internal buffer */
void sdi_take_screenshot();

/* Shuts down and cleans up */
int sdi_shutdown();

/* Move something to the top of the screen*/
void sdi_move_top(char *name);

/* move "name" below "name_below" */
void sdi_move_below(char *name, char *name_below);

void sdi_print_get_char_properties(char c, int *x, int *y, int *w, int *h);

int sdi_blit_part_to_image(int *img_dest,int width, int x, int y, int w, int h, int dx, int dy, int *img_src,int width2);

int sdi_np2(int width);

int sdi_swap_image(char *dest, char *src);
int sdi_copy_image(char *src, char *dest);
void sdi_clear(char *name);
void sdi_show_part(char *name,int x1, int y1, int x2, int y2);
void sdi_copy_part(char *dest, char *src, int x1, int y1, int x2, int y2);
void sdi_create_image(char *name, int width, int height);
int sdi_load_image(char *name, char*filename);
int sdi_load_png(char *name, char* filename);
int sdi_load_jpeg(char *name, char* filename);


#endif
