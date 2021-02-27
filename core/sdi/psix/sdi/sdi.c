
#include "sdi.h"
#include <string.h>
#include "png.h"

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

typedef struct sdi_object {
  struct sdi_object *next;
  char *name;
  int *img_data;		/* 32 bits per pixel */
  int x,y;
  int height,width;
  int alpha;
  int index;
  int enabled;
} sdi_object;

typedef struct sdi_object_list {
  struct sdi_object *first;
  struct sdi_object *last;
  int num;
} sdi_object_list;

/* the list of objects */
sdi_object_list objects;

/* Initialize SDI */
int sdi_init()
{

#ifdef SDI_BACKEND_INIT
  if (sdi_backend_init())
    {
      sdi_error("Unable to init SDI Backened");
      return 1;
    }
#else
  sdi_error("No sdi_backend_init()");
#endif

  objects.num = 0;
  objects.first = 0;
  objects.last = 0;

  return 0;
}

void sdi_error(char *strerror)
{
  printf("%s\n",strerror);
}

/* Draws to the display */
int sdi_draw()
{
  struct sdi_object *obj=objects.first;
  /* Run through the list and blit objects */

  while (obj)
    {
      sdi_backend_blit(obj->x,obj->y,obj->height,obj->width,obj->alpha,&(obj->img_data));
      obj = obj->next;
    }

#ifdef SDI_BACKEND_FLIP
  sdi_backend_flip();
#endif
}

/* Creates a new object */
int sdi_new(char *name)
{
  struct sdi_object *obj;

  /* create an object */
  obj = (struct sdi_object *)malloc(sizeof(struct sdi_object));

  /* copy the name */
  obj->name = (char *)malloc(strlen(name)+1);
  strcpy(obj->name,name);
  (obj->name)[strlen(name)] = 0;
  
  /* set everything else to zero */
  obj->x = 0;
  obj->y = 0;
  obj->height = 0;
  obj->width = 0;
  obj->enabled = 0;
  obj->index = ++(objects.num);
  obj->next = 0;
  obj->img_data = 0;

  /* stick it in the list */
  if (objects.num > 1)
    {
      (objects.last)->next = obj;
      objects.last = obj;
    }
  else
    {
      objects.first = obj;
      objects.last = obj;
    }
}

/* Deletes an object */
int sdi_del(char *name)
{
  struct sdi_object *obj,*obj2;

  obj = obj2 = objects.first;

  while (obj && strcmp(obj->name,name))
    {
      obj2 = obj;
      obj = obj->next;
    }

  /* remove the object from the list */
  if (obj == objects.first)
    objects.first = obj->next;  
  else
    obj2->next = obj->next;

  if (obj)
    {
      /* save the index so we know to modify the layers above */
      int i = obj->index;

      if (obj->name)
	free(obj->name);
      if (obj->img_data)
      	free(obj->img_data);
      free(obj);

      objects.num--;

      /* now run through the rest of the list decrementing the index */
     
      obj = objects.first;

      while (obj)
	{
	  if (obj->index > i)
	    (obj->index)--;
	  obj = obj->next;
	}
      
      return 0;
    }

  sdi_error("Unable to remove object");
  return 1;
}

void sdi_print_object(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(name,obj->name))
    obj = obj->next;
  if (obj && !strcmp(name,obj->name))
    {
      printf("------ SDI Object ------\nname: %s\naddress: %.8x\nnext:%.8x\nx: %d\ny: %d\nwidth: %d\nheight: %d\nindex: %d\nenabled: %d\n------------------------\n",obj->name,obj,obj->next,obj->x,obj->y,obj->width,obj->height,obj->index,obj->enabled);
    }
  else
    printf ("Object not found.\n");
}

void sdi_print_objects()
{
  struct sdi_object *obj = objects.first;

  while (obj)
    {
      sdi_print_object (obj->name);
      obj = obj->next;
    }
}

/* Sets the object image */
int sdi_set_image(char *name, int **img, int w, int h)
{
  struct sdi_object *obj = objects.first;

  while(obj && strcmp(obj->name,name))
    obj = obj->next;

  /* see if we found it */
  if (obj && !strcmp(obj->name,name))
  {
    obj->img_data = malloc(w*h*4);
    memcpy(obj->img_data,*img,w*h*4);
    obj->width = w;
    obj->height = h;
  }
  else
    return 1;			/* we coulddn't find the object */
}

/* Sets/Gets image properties */
void sdi_set_height(char *name,int height)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    obj->height = height;
}

int  sdi_get_height(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->height;
}

void sdi_set_width(char *name,int width)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    obj->width = width;
}

int  sdi_get_width(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->width;
}

void sdi_set_x(char *name,int x)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    obj->x = x;
}

int  sdi_get_x(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->x;
}

void sdi_set_y(char *name,int y)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    obj->y = y;
}

int  sdi_get_y(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->y;
}

void sdi_set_alpha(char *name,int alpha)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    obj->alpha = alpha;
}

int  sdi_get_alpha(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->alpha;
}

void sdi_set_index(char *name,int index)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    obj->index = index;
}

int  sdi_get_index(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->index;
}

void sdi_set_enabled(char *name,int enabled)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    obj->enabled = enabled;
}

int  sdi_get_enabled(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->enabled;
}

void sdi_rename_object(char *name, char *new_name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(name,obj->name))
    obj = obj->next;
  
  if (obj && !strcmp(name,obj->name))
    {
      free(obj->name);
      obj->name = (char*)malloc(strlen(new_name)+1);
      strcpy(obj->name,new_name);
    }
}

/* Shuts down and cleans up */
int sdi_shutdown()
{
  struct sdi_object *obj,*obj2;

  obj = objects.first;

  while (obj)
    {
      obj2 = obj->next;
      sdi_del(obj->name);
      obj = obj2;
    }
 
#ifdef SDI_BACKEND_SHUTDOWN
  /* Shut down the backend too */
  sdi_backend_shutdown();
#endif

}



void sdi_read_png_file(char *name, char* file_name)
{
  struct sdi_object *obj=objects.first;
  /* find the object */
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (!obj || strcmp(obj->name,name))
    {
      /* unable to find object */
      return;
    }

  int x, y;
  
  int width, height;
  png_byte color_type;
  png_byte bit_depth;
  
  png_structp png_ptr;
  png_infop info_ptr;
  int number_of_passes;
  png_bytep * row_pointers;
  
  
  char header[8];// 8 is the maximum size that can be checked
  
  /* open file and test for it being a png */
  FILE *fp = fopen(file_name, "rb");
  if (!fp)
    {
      /* file could not be opened */
    }
  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, 8))
    {
      /* not a png file */
    }


  
  /* initialize stuff */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  //  printf("here");


  if (!png_ptr)
    {
      // png_create_read_struct failed
    }
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    {
      // png_create_info_struct failed
    }
  
  if (setjmp(png_jmpbuf(png_ptr)))
    {
      // Error during init_io
    }
  
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);
  
  png_read_info(png_ptr, info_ptr);
  
  width = info_ptr->width;
  height = info_ptr->height;
  color_type = info_ptr->color_type;
  bit_depth = info_ptr->bit_depth;
  
  number_of_passes = png_set_interlace_handling(png_ptr);
  png_read_update_info(png_ptr, info_ptr);

  /* read file */
  if (setjmp(png_jmpbuf(png_ptr)))
    {
      /* error during read */

    }

  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
  for (y=0; y<height; y++)
    row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);

  png_read_image(png_ptr, row_pointers);

  fclose(fp);

  /* set some object info */
  obj->width = width;
  obj->height = height;

  /* start reading out the pixels */

  if (info_ptr->color_type != PNG_COLOR_TYPE_RGBA)
    { 
      /* must be PNG_COLOR_TYPE_RGBA */
    }

  /* allocate img_data */
  obj->img_data = (int*)malloc(sizeof(int)*height*width);

  int *img=obj->img_data;

  for (y=0; y<height; y++) {
    png_byte* row = row_pointers[y];
    for (x=0; x<width; x++) {
      //      *img++ = (int) row[x*4];
      //*img = (int)&(row[x*4]);
      //img++; 
      //printf("%.8x\n",*img);

      png_byte* ptr = &(row[x*4]);
      *img = (ptr[0] & 0xFF000000) | ((ptr[1] >> 8) & 0x00FF0000) | ((ptr[2] >> 16) & 0x0000FF00) | ((ptr[3] >> 24) & 0xFF);

      printf("0x%.8x",*img);
      img++;

      //*img = (int)*ptr;
      //      img++;

            printf("Pixel at position [ %d - %d ] has the following RGBA values: %d - %d - %d - %d\n",  x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

      /* set red value to 0 and green value to the blue one */
      //      ptr[0] = 0;
      //ptr[1] = ptr[2];
    }
    //    printf("\n");
  }

}

