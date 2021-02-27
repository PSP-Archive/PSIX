#ifndef __PBP_H
#define __PBP_H

#include "psix.h"


struct psix_pbp_read {
  char title[50]; 
  char region[50]; 
  char firmware[50]; 
};


int psix_pbp_read(char *filename,struct psix_pbp_read*);
int psix_pbp_psf_is(void *);
int psix_pbp_psf_get_info(void *,char*,char*,char*);

#endif
