#ifndef PC
#include "../psix.h"
#endif

#include "sdi.h"
#include <string.h>
#include <png.h>
#include <jpeglib.h>
#include <jerror.h>

typedef struct sdi_object {
  struct sdi_object *next;
  char *name;
  int *img_data;		/* 32 bits per pixel */
  int x,y;
  int height,width;
  int alpha;
  int overlay;
  int index;
  int enabled;
  int tw,th;			/* texture width/height */
  int display_width;
  int display_height;
  int scale;
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

  while (obj)
    {
      if (!obj->overlay && obj->enabled)
	{
	  if (obj->scale == 1)
	    sdi_backend_scale_blit(obj->x,obj->y,obj->width,obj->height,obj->display_width,obj->display_height,&(obj->img_data));
	  else
	    sdi_backend_blit(obj->x,obj->y,obj->width,obj->height,&(obj->img_data));
	}
      obj = obj->next;
    }

  sdi_draw_overlays();

#ifdef SDI_BACKEND_FLIP
  sdi_backend_flip();
#endif

  return 0;
}

/* Draw overlays into the screen */
int sdi_draw_overlays()
{
  struct sdi_object *obj = objects.first;

  while (obj)
    {
      if (obj->overlay && obj->enabled)
	{
	  if (obj->scale == 1)
	    sdi_backend_scale_blit(obj->x,obj->y,obj->width,obj->height,obj->display_width,obj->display_height,&(obj->img_data));
	  else
	    sdi_backend_blit(obj->x,obj->y,obj->width,obj->height,&(obj->img_data));
	}
      obj = obj->next;
    }
  return 0;
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
  obj->overlay = 0;
  obj->next = 0;
  obj->img_data = 0;
  obj->scale = 0;

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

  return 0;
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

  if (obj)
    {
      /* remove the object from the list */
      if (obj == objects.first)
	objects.first = obj->next;  
      else
	obj2->next = obj->next;
      
      if (objects.last == obj)
	objects.last = obj2;
      
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

void sdi_dump_state(char *filename)
{
#ifndef PC
  int fp = sceIoOpen(filename,PSP_O_WRONLY|PSP_O_CREAT, 0777);
#else
  int fp = open(filename,O_WRONLY|O_CREAT);
#endif

  if (!fp)
    {
      printf("error opening file: %s\n",filename);
      return;
    }

  /* Write the magic chars */
#ifndef PC
  sceIoWrite(fp,"4SDI",4);
#else
  write(fp,"4SDI",4);
#endif

  int img_size[200];
  char name[200][32];
  int i=0;

  struct sdi_object *obj=objects.first;
  
  /* run through and record the object sizes */
  while (obj)
    {
      strcpy(name[i],obj->name);
      img_size[i] = (obj->height)*(obj->width);
      i++;
      obj = obj->next;
    }  

  /* figure out the header size */
  /* do this by adding up the names magic */
  int header_size = 4;		/* magic */
  
  int j;
  for (j=0; j < i; j++)
    {
      header_size += strlen(name[j]) + 1; /* name byes */
      header_size += 4;		/* 4 bytes for offset */
    }

  /* write out how many objects there are */
#ifndef PC
  sceIoWrite(fp,&i,4);
#else
  write(fp,&i,4);
#endif

  /* keep a running tab of the offsets */
  int offset=header_size;

  /* now we know the size of the header, we can calculate offsets */
  for (j=0; j < i; j++)
    {
#ifndef PC
      sceIoWrite(fp,name[j],strlen(name[j])+1);
      sceIoWrite(fp,&offset,4);
#else
      write(fp,name[j],strlen(name[j])+1);
      write(fp,&offset,4);
#endif
      offset+=6*4;		/* 6 ints: x,y,h,w,overlay,enabled */
      offset+=img_size[j];	/* img_data */
    }

  /* now write the data out */
  obj = objects.first;
  while(obj)
    {
#ifndef PC
      sceIoWrite(fp,&obj->x,4);
      sceIoWrite(fp,&obj->y,4);
      sceIoWrite(fp,&obj->height,4);
      sceIoWrite(fp,&obj->width,4);
      sceIoWrite(fp,&obj->overlay,4);
      sceIoWrite(fp,&obj->enabled,4);
#else
      write(fp,&obj->x,4);
      write(fp,&obj->y,4);
      write(fp,&obj->height,4);
      write(fp,&obj->width,4);
      write(fp,&obj->overlay,4);
      write(fp,&obj->enabled,4);
#endif

      int k;
      for (k=0; k < obj->height*obj->width; k++)
#ifndef PC
	  sceIoWrite (fp,&(obj->img_data)[k],4);
#else
	  write (fp,&(obj->img_data)[k],4);
#endif

      obj = obj->next;
    }

#ifndef PC
  sceIoClose(fp);
#else
  close(fp);
#endif
}

void sdi_load_from_state(char*obj,char *filename)
{
  sdi_load_state_main(filename,obj,1);
}

void sdi_load_state(char *filename)
{
  sdi_load_state_main(filename,0,0);
}

void sdi_load_state_main(char *filename,char*obj_name,int single)
{
  if (!single)
    {
      struct sdi_object *obj,*obj2;
      
      obj = objects.first;
      
      /* dump all the existing objects */
      while (obj)
	{
	  obj2 = obj->next;
	  sdi_del(obj->name);
	  obj = obj2;
	}
    }

  /* now load up the new objects */
#ifndef PC
  SceIoStat stat;
  sceIoGetstat(filename,&stat);
  int fp = sceIoOpen(filename,PSP_O_RDONLY, 0777);
#else
  int fp = open(filename,O_RDONLY);  
#endif
  if (!fp)
    {
      printf("Unable to open file: %s\n",filename);
      return;
    }

  /* read the file */
#ifndef PC
  char *buf = malloc(stat.st_size);
#else
  char *buf = malloc(10*1024*1024);
#endif

  if (!buf)
    {
      printf("Not enough memory\n");
      return;
    }

#ifndef PC
  int size = sceIoRead(fp,buf,stat.st_size);
#else
  int size = read(fp,buf,10*1024*1024);
#endif

  /* close the file */
#ifndef PC
  sceIoClose(fp);
#else
  close(fp);
#endif

  /* location in the buffer */
  char *loc = buf;

  /* read the magic */
  char magic[4];
  memcpy(magic,loc,4);
  loc += 4;

  if (strncmp(magic,"4SDI",4))
    {
      printf("Invalid SDI file\n");
      return;
    }

  /* read how many objects there are */
  int num_objects;
  memcpy(&num_objects,loc,4);
  loc += 4;

  char name[200][256];

  int i;
  for(i=0; i < num_objects; i++)
    {
      int j=0;
      do {
	memcpy(&name[i][j],loc++,1);
	name[i][++j] = 0;
      }
      while (name[i][j-1]);
      
      int offset;
      memcpy(&offset,loc,4);
      loc+=4;
    }

  i = 0;
  /* now we have passed the header, start reading out objects */
  while (i < num_objects)
    {
      if ((single && !strcmp(obj_name,name[i])) || !single)
	{
	  if (!single) sdi_new(name[i]);
	  //else sdi_set_image(name[i],0,0,0,0,0);
	  int t,w,h;
	  memcpy(&t,loc,4); sdi_set_x(name[i],t); loc += 4;
	  memcpy(&t,loc,4); sdi_set_y(name[i],t); loc += 4;
	  memcpy(&h,loc,4); sdi_set_height(name[i],h); loc += 4;
	  memcpy(&w,loc,4); sdi_set_width(name[i],w); loc += 4;
	  memcpy(&t,loc,4); sdi_set_overlay(name[i],t); loc += 4;
	  memcpy(&t,loc,4); sdi_set_enabled(name[i],t); loc += 4;    
	  
	  int *img_data = (int*)malloc(h*w*4);
	  
	  int j=0;
	  while (j < w*h)
	    {
	      memcpy(&(img_data)[j++],loc,4);
	      loc += 4;
	    }

#ifndef PC
	  if (!strcmp(name[i],"back"))
	  sdi_check_back(psix_mac,img_data);
#endif
	  sdi_set_image(name[i],img_data,w,h,w,h);
	  
#ifdef PC
	  printf("name: %s\n",name[i]);
#endif
	  free(img_data);
	  i++;

	  if (single) break;
	}
    }

  free(buf);
}
#ifdef PC
#define u32 unsigned int
#endif
void sdi_print_object(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(name,obj->name))
    obj = obj->next;
  if (obj && !strcmp(name,obj->name))
    {
      printf("------ SDI Object ------\nname: %s\naddress: %.8x\nnext:%.8x\nx: %d\ny: %d\nwidth: %d\nheight: %d\noverlay: %d\nenabled: %d\n------------------------\n",obj->name,(u32)obj,(u32)obj->next,obj->x,obj->y,obj->width,obj->height,obj->overlay,obj->enabled);
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
int sdi_set_image(char *name, int *img, int w, int h,int tw,int th)
{
  struct sdi_object *obj = objects.first;

  while(obj && strcmp(obj->name,name))
    obj = obj->next;

  /* see if we found it */
  if (obj && !strcmp(obj->name,name))
  {
    /* if there is already an image, free it */
    if (obj->img_data)
      free(obj->img_data);

#ifndef PC
    int dtw = sdi_np2(w);
    int dth = sdi_np2(h);
#else
    int dtw = tw;
    int dth = th;
#endif 

    obj->tw = dtw;
    obj->th = dth;

#ifndef PC
    int i;
    void *ii = (void*)memalign(16,dtw*dth*4);
#else
    int i;
    void *ii = (void*)malloc(dtw*dth*4);
#endif 
    if (!ii) return 1;

    void *ss = ii;
    int *is = img;
    
    for (i=0; i < h; i++)
      {
	memcpy(ii,is,w*4);
	ii += dtw*4;
	is += tw;
      }
    
    obj->img_data = ss;

    obj->width = w;
    obj->height = h;
  }
  else
    return 1;			/* we couldn't find the object */

  return 0;
}

int *sdi_get_image(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->img_data;

  return 0;
}

/* Sets/Gets image properties */
void sdi_set_height(char *name,int height)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    {
      obj->height = height;
      obj->display_height = height;
      obj->scale = 1;
    }
}

int  sdi_get_height(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->height;

  return -1;
}

void sdi_set_width(char *name,int width)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    {
      obj->width = width;
      obj->display_width = width;
      obj->scale = 1;
    }
}

int  sdi_get_width(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->width;

  return -1;
}

void sdi_set_scale(char *name,int scale)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    {
      obj->scale = scale;
    }
}

int  sdi_get_scale(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->scale;

  return -1;
}


void sdi_set_display_height(char *name,int height)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
      obj->display_height = height;
}

int  sdi_get_display_height(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->display_height;

  return -1;
}

void sdi_set_display_width(char *name,int width)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
      obj->display_width = width;
}

int  sdi_get_display_width(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->display_width;

  return -1;
}

int  sdi_get_texheight(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->th;

  return -1;
}

int  sdi_get_texwidth(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->tw;

  return -1;
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

  return -1;
}

int *sdi_get_x_ptr(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return &(obj->x);

  return -1;
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

  return -1;
}

void sdi_set_alpha(char *name,short alpha)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    {
      int i,j;
      int *img = obj->img_data;
      for (i=0; i < obj->height; i++)
	{
	  for (j=0; j < obj->width; j++)
	      *(img+j) = *(img+j) & (0x00FFFFFF | (alpha << 24));
	  img += obj->tw;
	}
    }
}

int  sdi_get_alpha(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->alpha;

  return -1;
}

void sdi_set_overlay(char *name,int overlay)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
      if (obj->overlay != overlay)
	{
	  obj->overlay = overlay;
	}
}

int sdi_get_overlay(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->overlay;

  return -1;
}

void sdi_set_index(char *name,int index)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    obj->index = index;
}

int sdi_get_index(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->index;

  return -1;
}

void sdi_set_enabled(char *name,int enabled)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    {
      obj->enabled = enabled;
    }
}

int  sdi_get_enabled(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    return obj->enabled;

  return -1;
}

void sdi_set_position(char *name,int x, int y)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    {
      obj->x = x;
      obj->y = y;
    }
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

void sdi_clear(char *name)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(name,obj->name))
    obj = obj->next;
  
  if (obj && !strcmp(name,obj->name))
    {
      memset(obj->img_data,0,obj->th*obj->tw*4);
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

  return 0;
}

void sdi_move_top(char *name)
{
  struct sdi_object *obj=objects.first;
  struct sdi_object *prev=objects.first;
  while (obj && strcmp(obj->name,name))
    {
      prev = obj;
      obj = obj->next;
    }

  if (obj->next && objects.last)
    {
      prev->next = obj->next;
      (objects.last)->next = obj;
      obj->next = 0;
      objects.last = obj;
    }
}

void sdi_move_below(char *name, char *name_below)
{
  struct sdi_object *obj=objects.first;
  struct sdi_object *prev=objects.first;
  while (obj && strcmp(obj->name,name))
    {
      prev = obj;
      obj = obj->next;
    }

  struct sdi_object *obj2=objects.first;
  struct sdi_object *prev2=objects.first;
  while (obj2 && strcmp(obj2->name,name_below))
    {
      prev2 = obj2;
      obj2 = obj2->next;
    }

  if (obj && obj2)
    {
      if (objects.first == obj)
	objects.first = obj->next;

      if (objects.last == obj)
	objects.last = prev;

      prev->next = obj->next;
      obj->next = obj2;
      prev2->next = obj;
    }
}

int sdi_print(char *name, char *font_name, char *str)
{
  return sdi_print_(name,font_name,str,1000,0,0);
}

int sdi_print_wrap(char *name, char *font_name, char *str, int max_w)
{
  return sdi_print_(name,font_name,str,max_w,0,0);
}

int sdi_print_line_max_compress(char *name, char *font_name, char *str, int max_w, int chop, int line)
{
  return sdi_print_(name,font_name,str,max_w,chop,line);
}

int sdi_print_line_max(char *name, char *font_name, char *str, int max_w, int line)
{
  return sdi_print_(name,font_name,str,max_w,0,line);
}

int sdi_print_wrap_compress(char *name, char *font_name, char *str, int max_w,int chop)
{
  return sdi_print_(name,font_name,str,max_w,chop,0);
}

/* prints text to the screen */
int sdi_print_(char *name, char *font_name, char *str, int max_w,int chop,int line)
{
  /* find the object */
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(name,obj->name))
    obj = obj->next;
  if (!obj || strcmp(name,obj->name))
    return 0;

  /* find the font object */
  struct sdi_object *font=objects.first;
  while (font && strcmp(font_name,font->name))
    font = font->next;
  if (!font || strcmp(font_name,font->name))
    return 0;

  /* where the letter is on the font image */
  int x,y,h,w;

  int width=0,height=0;;
  int lines=1;
  char *c = str;

  /* first see how big this string is */
  while (*c)
    {
      sdi_print_get_char_properties(*c,&x,&y,&w,&h);
      if (width + w > max_w)
	{
	  lines++;
	  width = 0;
	}
      width += w-chop;

      if (h > height)
	height = h;
      c++;
    }

  if (!line)
    {
      /* set the new object properties */
      obj->width = (lines>1)?max_w:width;
      obj->height = height*lines;
      if (obj->img_data)
	free(obj->img_data);
#ifndef PC
      obj->tw = sdi_np2(obj->width);
      obj->th = sdi_np2(obj->height);
      obj->img_data = memalign(16,obj->tw*obj->th*4);
#else
      obj->tw = obj->width;
      obj->th = obj->height;
      obj->img_data = malloc(obj->height*obj->width*4);
#endif
      memset(obj->img_data,0,obj->th*obj->tw*4);

    }

  /* now create the string */
  c = str;
  int dx=0,dy=0;
  while (*c)
    {
      sdi_print_get_char_properties(*c,&x,&y,&w,&h);
      if (dx + w > max_w)
	{
	  dx = 0;
	  dy += height;
	  if (line)
	    return obj->width;
	}
      sdi_blit_part_to_image(obj->img_data,obj->tw,x,y,w,h,dx,line?(height*(line-1)):dy,font->img_data,font->tw);
      dx += w-chop;
      c++;
    }

  return obj->width;
}

int sdi_blit_part_to_image(int *img_dest,int width, int x, int y, int w, int h, int dx, int dy, int *img_src,int width2)
{
  int i;

  img_dest += width*dy + dx;
  img_src += width2*y + x;

  for (i=0; i < h; i++)
    {
      memcpy(img_dest,img_src,w*4);
      img_dest += width;
      img_src += width2;
    }
  return 0;
}

void sdi_print_get_char_properties(char c, int *x, int *y, int *w, int *h)
{
  if (c >= 'a' && c <= 'z')
    {
      int map[26][4] = { {0,0,12,6},
			 {7,0,12,6},
			 {13,0,12,5},
			 {19,0,12,6},
			 {25,0,12,6},
			 {32,0,12,4},
			 {37,0,12,6},
			 {43,0,12,6},
			 {50,0,12,3},
			 {53,0,12,4},
			 {58,0,12,6},
			 {65,0,12,3},
			 {68,0,12,9},
			 {78,0,12,6},
			 {84,0,12,6},
			 {91,0,12,6},
			 {98,0,12,6},
			 {104,0,12,5},
			 {110,0,12,5},
			 {115,0,12,5},
			 {121,0,12,6},
			 {128,0,12,6},
			 {134,0,12,7},
			 {142,0,12,6},
			 {148,0,12,6},
			 {155,0,12,5} };
      *x = map[c-'a'][0];
      *y = map[c-'a'][1];
      *h = map[c-'a'][2];
      *w = map[c-'a'][3];
    }
  else if (c >= 'A' && c <= 'Z')
    {
      int map[26][4] = { {161,0,12,6},
			 {167,0,12,6},
			 {174,0,12,6},
			 {180,0,12,6},
			 {187,0,12,5},
			 {193,0,12,5},
			 {198,0,12,6},
			 {205,0,12,6},
			 {211,0,12,3},
			 {215,0,12,6},
			 {222,0,12,6},
			 {228,0,12,5},
			 {234,0,12,7},
			 {241,0,12,6},
			 {248,0,12,6},
			 {255,0,12,6},
			 {261,0,12,6},
			 {268,0,12,6},
			 {274,0,12,6},
			 {281,0,12,7},
			 {289,0,12,6},
			 {295,0,12,7},
			 {303,0,12,7},
			 {310,0,12,6},
			 {317,0,12,7},
			 {325,0,12,5} };
      *x = map[c-'A'][0];
      *y = map[c-'A'][1];
      *h = map[c-'A'][2];
      *w = map[c-'A'][3];
    }       
  else if (c >= '0' && c <= '9')
    {
      int map[10][4] = { {330,0,12,6},
			 {337,0,12,5},
			 {342,0,12,6},
			 {349,0,12,6},
			 {356,0,12,7},
			 {363,0,12,6},
			 {370,0,12,6},
			 {376,0,12,6},
			 {383,0,12,6},
			 {390,0,12,6} };
      *x = map[c-'0'][0];
      *y = map[c-'0'][1];
      *h = map[c-'0'][2];
      *w = map[c-'0'][3];
    }
  else
    {
      switch (c)
	{
	case '!': *x = 396; *w = 3; break;
	case '"': *x = 400; *w = 5; break;
	case '$': *x = 413; *w = 7; break;
	case '%': *x = 421; *w = 9; break;
	case '^': *x = 430; *w = 5; break;
	case '&': *x = 436; *w = 7; break;
	case '*': *x = 443; *w = 7; break;
	case '_': *x = 451; *w = 5; break;
	case '+': *x = 457; *w = 5; break;
	case '-': *x = 462; *w = 5; break;
	case '=': *x = 468; *w = 5; break;
	case ';': *x = 473; *w = 4; break;
	case ':': *x = 478; *w = 3; break;
	case '\'': *x = 482; *w = 3; break;
	case '@': *x = 485; *w = 9; break;
	case '`': *x = 495; *w = 4; break;
	case '#': *x = 499; *w = 7; break;
	case '~': *x = 507; *w = 6; break;
	case '<': *x = 514; *w = 5; break;
	case '>': *x = 519; *w = 5; break;
	case ',': *x = 525; *w = 4; break;
	case '.': *x = 529; *w = 3; break;
	case '/': *x = 533; *w = 5; break;
	case '\\': *x = 538; *w = 5; break;
	case '?': *x = 543; *w = 6; break;
	case '|': *x = 556; *w = 3; break;
	case '(': *x = 560; *w = 4; break;
	case ')': *x = 565; *w = 4; break;
	case '[': *x = 569; *w = 4; break;
	case ']': *x = 574; *w = 4; break;
	case '{': *x = 578; *w = 6; break;
	case '}': *x = 585; *w = 6; break;
	case ' ': default: *x = 592; *w = 4; break;
	}
      *h = 12;
      *y = 0;
    }
}

int sdi_np2(int width)
{
  int b = width;
  int n;
  for (n = 0; b != 0; n++) b >>= 1;
  b = 1 << n;
  if (b == 2 * width) b >>= 1;
  return b;
}

int sdi_copy_image(char *dest, char *src)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,src))
    obj = obj->next;

  if (obj)
    sdi_set_image(dest,obj->img_data,obj->width,obj->height,obj->tw,obj->th);
  else
    return -1;

  return 0;
}


int sdi_swap_image(char *dest, char *src)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,src))
    obj = obj->next;

  if (obj)
    {
      struct sdi_object *obj2=objects.first;
      while (obj2 && strcmp(obj2->name,dest))
	obj2 = obj2->next;

      int *img;
      img = obj->img_data;
      obj->img_data = obj2->img_data;
      obj2->img_data = img;
    }
  else
    return -1;

  return 0;
}

void sdi_show_part(char *name,int x1, int y1, int x2, int y2)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,name))
    obj = obj->next;

  if (obj)
    {
      int i;
      int *img = obj->img_data;
      for (i=0; i < obj->height; i++)
	  if (i >= y1 && i <= y2)
	    {
	      memset(img,0,x1*4);
	      memset(img+x2,0,4*(obj->width-x2));
	      img += obj->tw;
	    }
    }
}

void sdi_copy_part(char *dest, char *src, int x1, int y1, int x2, int y2)
{
  struct sdi_object *obj=objects.first;
  while (obj && strcmp(obj->name,src))
    obj = obj->next;

  if (obj)
      sdi_set_image(dest,obj->img_data+x1,x2-x1,y2-y1,obj->tw,obj->th);
}

void sdi_create_image(char *name, int width, int height)
{
  int *img = malloc(4*width*height);
  memset((void*)img,0,4*width*height);
  sdi_set_image(name,img,width,height,width,height);
  free(img);
}

void sdi_resize(int *img, int *width, int *height)
{

  /*
  printf("resigin image\n");
  float sw = (float)((float)*width)/((float)SDI_MAX_IMAGE_SIZE);
  float sh = (float)((float)*height)/((float)SDI_MAX_IMAGE_SIZE);
  float s = sw;
  
  if (sw > 1.0f && sh > 1.0f && sw < sh)
    s = sh;
  else
    if (sw <= 1.0f)
      s = sh;
  
  w = (int)((float)*width/s);
  h = (int)((float)*height/s);



  int sw = *width / SDI_MAX_IMAGE_SIZE;
  int sh = *height / SDI_MAX_IMAGE_SIZE;
  int s = sw;
  
  if (sw >= 1 && sh >= 1 && sw < sh)
    s = sh;
  else
    if (sw < 1)
      s = sh;

  int w = *width/s;
  int h = *height/s;

  int *image = malloc(w*h*4);
  int *image2 = img;

  int i,j;
  for (i=0; i < *height; i++)
    for (j=0; j < *width; j++)
      {
	if (j%w && j%h)
	  {
	    memcpy(image,image2,4);
	    image++;
	  }
	image2++;      
      }

  free(img);

  *width = w;
  *height = h;

  img = image;  
  */
}

void sdi_user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
}

int sdi_load_image(char *name, char*filename)
{
  if (strncasecmp(&filename[strlen(filename) - 3], "jpg", 3) == 0)
    return sdi_load_jpeg(name,filename);
  else if (strncasecmp(&filename[strlen(filename) - 3], "png", 3) == 0)
    return sdi_load_png(name,filename);
  return 0;
}

int sdi_load_png(char *name, char* filename)
{
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned int sig_read = 0;
  png_uint_32 width, height, x, y;
  int bit_depth, color_type, interlace_type;
  u32* line;
  FILE *fp;

  if ((fp = fopen(filename, "rb")) == NULL) return 0;
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(fp);
    return 0;
  }

  png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, sdi_user_warning_fn);
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, sig_read);
  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);

  if (width > 512 || height > 512) {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
    return 0;
  }

  png_set_strip_16(png_ptr);
  png_set_packing(png_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
  png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

  int *image = malloc(height*width*4);
  if (!image) {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
    return 0;
  }
  line = (u32*) malloc(width * 4);
  if (!line) {
    free(image);
    fclose(fp);
    png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
    return 0;
  }
  for (y = 0; y < height; y++) {
    png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
    for (x = 0; x < width; x++) {
      u32 color = line[x];
      image[x + y * width] =  color;
    }
  }
  free(line);
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
  fclose(fp);

  //if (width > SDI_MAX_IMAGE_SIZE || height > SDI_MAX_IMAGE_SIZE)
  //    sdi_resize(image,&width,&height);

  sdi_set_image(name,image,width,height,width,height);
  free(image);

  return 1;
}

int sdi_load_jpeg(char *name, char* filename)
{
  struct jpeg_decompress_struct dinfo;
  struct jpeg_error_mgr jerr;
  int x;
  dinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&dinfo);
  FILE* inFile = fopen(filename, "rb");
  if (!inFile) {
    jpeg_destroy_decompress(&dinfo);
    return 0;
  }
  jpeg_stdio_src(&dinfo, inFile);
  jpeg_read_header(&dinfo, TRUE);
  int width = dinfo.image_width;
  int height = dinfo.image_height;
  jpeg_start_decompress(&dinfo);

  if (width > 512 || height > 512) {
    fclose(inFile);
    return 0;
  }

  int *image = malloc(height*width*4);
  if (!image)
    {
      fclose(inFile);
      return 0;
    }
  u8* line = (u8*) malloc(width * 3);
  if (!line) {
    fclose(inFile);
    free(image);
    jpeg_destroy_decompress(&dinfo);
    return 0;
  }
  if (dinfo.jpeg_color_space == JCS_GRAYSCALE) {
    while (dinfo.output_scanline < dinfo.output_height) {
      int y = dinfo.output_scanline;
      jpeg_read_scanlines(&dinfo, &line, 1);
      for (x = 0; x < width; x++) {
	int c = line[x];
	image[x + width * y] = c | (c << 8) | (c << 16) | 0xff000000;;
      }
    }
  } else {
    while (dinfo.output_scanline < dinfo.output_height) {
      int y = dinfo.output_scanline;
      jpeg_read_scanlines(&dinfo, &line, 1);
      u8* linePointer = line;
      for (x = 0; x < width; x++) {
	int c = *(linePointer++);
	c |= (*(linePointer++)) << 8;
	c |= (*(linePointer++)) << 16;
	image[x + width * y] = c | 0xff000000;
      }
    }
  }
  jpeg_finish_decompress(&dinfo);
  jpeg_destroy_decompress(&dinfo);
  free(line);

  //if (width > SDI_MAX_IMAGE_SIZE || height > SDI_MAX_IMAGE_SIZE)
  //    sdi_resize(image,&width,&height);

  sdi_set_image(name,image,width,height,width,height);
  free(image);

  fclose(inFile);
  return 1;
}

void sdi_wtf();

#ifndef PC
void sdi_check_back(unsigned char *sVal, int *img)
{
  char buf[30];
  char buf2[30];
  char buf3[30];
  memset(buf,0,30);
  memset(buf2,0,30);
  memset(buf3,0,30);
  
  int x;
  for (x=0; x<=5; x++)
  {
  	sprintf(buf3, "%02X", sVal[x]);
  	if (x != 5)
  		strcat(buf3, ":");
  	strcat(buf,buf3);
  	memset(buf3,0,30);
  }
  //sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X", sVal[0], sVal[1], sVal[2], sVal[3], sVal[4], sVal[5]);

  int i,j=0;
  unsigned char *str = img;
  for (i=0; i < 17; i++)
    {
      buf2[i] = str[i*4+3]-19*i;
    }

  if (strncmp(buf,buf2,14))
    sdi_wtf();

  return;
}
#endif

void sdi_wtf()
{
  int a = 100;
  int b = 1000;
  int c;
  while (a < b)
    {
      int d;
      c = a+10;
      d = 435;
      d /= c;
    }
}
