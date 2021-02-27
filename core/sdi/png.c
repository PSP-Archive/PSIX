#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

void sdi_read_png_file(char* file_name)
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
  
  if (!png_ptr)
    abort_("[read_png_file] png_create_read_struct failed");

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    abort_("[read_png_file] png_create_info_struct failed");
  
  if (setjmp(png_jmpbuf(png_ptr)))
    abort_("[read_png_file] Error during init_io");
  
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
      *img = &(row[x*4]);
      img++;  
      //      printf("Pixel at position [ %d - %d ] has the following RGBA values: %d - %d - %d - %d\n",  x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

      /* set red value to 0 and green value to the blue one */
      //      ptr[0] = 0;
      //ptr[1] = ptr[2];
    }
  }

  

}

