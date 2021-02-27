#include <stdio.h>
#include "sdi.h"
#include <SDL/SDL_image.h>
#include <time.h>




int main(int argc, char **argv)
{
  unsigned char img[17*4];

  memset(img,0,17*4);

  sdi_init();
  sdi_load_state(argv[1]);

  srand(clock());

  //printf("%s is of len %d\n",argv[3],strlen(argv[3]));

  int i,j;
  for(i=0,j=0; i < 17; i++)
    {
      img[i*4] = rand();
      img[i*4+1] = rand();
      img[i*4+2] = rand();
      img[i*4+3] = argv[3][i];
    }

  sdi_new("back");
  sdi_set_image("back",(int*)&img,17*4,1,17*4,1);

  unsigned char *t = sdi_get_image("back");

  for (i=0; i < 17*4; i++)
    printf("%02X '%c'",t[i],t[i]);

  sdi_dump_state(argv[2]);
  return 0;
}
