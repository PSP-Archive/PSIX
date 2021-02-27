#ifndef __IMAGE_H
#define __IMAGE_H

extern int psix_image_fullscreen;

void psix_image_init();
void psix_image_transition_in();
void psix_image_transition_out();
void psix_image_set_left(int x);
void psix_image_clear_filelist();
int psix_image_scan(char *path);
void psix_image_draw_filelist();
void psix_image_increment_file_line();
void psix_image_decrement_file_line();
void psix_image_keypress(u32 pad);
void psix_image_set_state(int state);
void psix_image_show_file_preview();
void psix_image_set_background();
void psix_image_remove_background();
void psix_image_toggle_psix_interface();
void psix_image_set_fullscreen_on();
void psix_image_set_fullscreen_off();
void psix_image_toggle_fullscreen();
void psix_image_set_preview_size();
void psix_image_cursor_moved();
void psix_image_file_select();
void psix_image_get_x_pointers();

#endif
