#ifndef __SDI_H
#define __SDI_H


#define SDI_SCREEN_WIDTH  480
#define SDI_SCREEN_HEIGHT 272
#define SDI_SCREEN_BPP    32

#ifdef SDL
#include "backends/sdl/sdi_sdl.h"
#endif

#include "png.h"

/* Initialize SDI */
int sdi_init();

/* Reports a SDI error */
void sdi_error(char *strerror);

/* Draws to the display */
int sdi_draw();

/* Creates a new object */
int sdi_new(char *name);

/* Deletes an object */
int sdi_del(char *name);

/* Prints out all the object information */
void sdi_print_object(char *name);
void sdi_print_objects();

/* Sets the object image */
int sdi_set_image(char *name,int **img, int w, int h);

/* Sets/Gets image properties */
void sdi_set_height(char *name,int height);
int  sdi_get_height(char *name);
void sdi_set_width(char *name,int width);
int  sdi_get_width(char *name);
void sdi_set_x(char *name,int x);
int  sdi_get_x(char *name);
void sdi_set_y(char *name,int y);
int  sdi_get_y(char *name);
void sdi_set_alpha(char *name,int alpha);
int  sdi_get_alpha(char *name);
void sdi_set_index(char *name,int index);
int  sdi_get_index(char *name);
void sdi_set_enabled(char *name,int enabled);
int  sdi_get_enabled(char *name);

/* rename an object */
void sdi_rename_object(char *name, char *new_name);

/* Shuts down and cleans up */
int sdi_shutdown();


/* load a png */
void sdi_read_png_file(char *name, char* file_name);


#endif
