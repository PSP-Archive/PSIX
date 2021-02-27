#ifndef __PAGE_H
#define __PAGE_H

#include "psix.h"

enum psix_dashboard_state_enum { PAGE_AUDIO=1, PAGE_VIDEO, PAGE_IMAGE, PAGE_FILE };

void psix_page_init();
void psix_page_set_page(int state);
void psix_page_show_page(int state);
void psix_page_update();
void psix_page_prepare_transition();

#endif
