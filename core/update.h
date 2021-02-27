#ifndef __UPDATE_H
#define __UPDATE_H

#include "psix.h"

struct psix_update_table_item {
  char filename[50];
  char version[20];
  int status;
  int size;
  int checked;
};

void psix_update_table_draw();
void psix_update_selection_bar_draw();
int psix_update_table_add(struct psix_update_table_item *item);
void psix_update_show();

#endif
