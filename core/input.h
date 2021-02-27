#ifndef __INPUT_H
#define __INPUT_H

#include "psix.h"

extern u32 lastpad;
extern SceCtrlData paddata;
extern SceUID psix_thread_input;

#define AVS_PAUSE_LEFT    211
#define AVS_PAUSE_TOP     258
#define AVS_PAUSE_WIDTH    11
#define AVS_PAUSE_HEIGHT   14
#define AVS_PREV_LEFT     223
#define AVS_PREV_TOP      258
#define AVS_PREV_WIDTH     11
#define AVS_PREV_HEIGHT    14
#define AVS_PLAY_LEFT     235
#define AVS_PLAY_TOP      258
#define AVS_PLAY_WIDTH     11
#define AVS_PLAY_HEIGHT    14
#define AVS_NEXT_LEFT     247
#define AVS_NEXT_TOP      258
#define AVS_NEXT_WIDTH     11
#define AVS_NEXT_HEIGHT    14
#define AVS_STOP_LEFT     259
#define AVS_STOP_TOP      258
#define AVS_STOP_WIDTH     11
#define AVS_STOP_HEIGHT    14



void psix_input_init();
void psix_input_update();
void psix_input_update_cursor();
void psix_input_start_thread();

extern float psix_cursor_x;
extern float psix_cursor_y;
extern int psix_bottab_state;

#endif
