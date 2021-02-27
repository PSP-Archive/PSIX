#ifndef __POWER_H
#define __POWER_H

#include "psix.h"

extern SceUID psix_thread_power;

void psix_power_init();
void psix_power_update();
void psix_power_start_thread();

#endif
