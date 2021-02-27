#include <stdio.h>
#include "sdi.h"

int main(int argc, char **argv)
{
  sdi_error("Testing sdi_error()...done");

  printf("Initializing SDI...");
  sdi_init();
  printf("SDI Initialized\n");

  printf("Adding tests...");
  sdi_new("test1");
  sdi_new("test2");
  //sdi_new("test3");
  printf("added\n");

  sdi_set_x("test1",10);
  sdi_set_x("test2",40);
  //sdi_set_x("test3",80);
  sdi_set_y("test1",10);
  sdi_set_y("test2",40);
  //sdi_set_y("test3",80);
  sdi_set_height("test1",10);
  sdi_set_height("test2",10);
  //sdi_set_height("test3",10);
  sdi_set_width("test1",10);
  sdi_set_width("test2",10);
  //sdi_set_width("test3",10);
  sdi_set_enabled("test1",1);
  sdi_set_enabled("test2",1);
  //sdi_set_enabled("test3",1);
  sdi_set_image("test1","colormap.png");
  sdi_set_image("test2","wall.png");
  //sdi_set_image("test3","wall.png");

  //sdi_print_objects();
  
  printf("drawing...\n");

  //  while (1)
    sdi_draw();

  sdi_shutdown();
  return 0;
}

