#ifndef __FILE_H
#define __FILE_H

void psix_file_init();
void psix_file_transition_in();
void psix_file_transition_out();
void psix_file_set_left(int x);
void psix_file_keypress(u32 pad);
void psix_file_set_state(int state);
void psix_file_update_checkbox();
void psix_file_set_checkbox(int value);
void psix_file_toggle_checkbox();
void psix_file_set_side(int side);
int psix_file_right_scan(char *path);
int psix_file_left_scan(char *path);
void psix_file_decrement_right_line();
void psix_file_increment_right_line();
void psix_file_decrement_left_line();
void psix_file_increment_left_line();
void psix_file_draw_right_files();
void psix_file_draw_left_files();
void psix_file_add_file_right(char *name,char *path,int type);
void psix_file_add_file_left(char *name,char *path,int type);
void psix_file_clear_files_right();
void psix_file_clear_files_left();
void psix_file_select(void *file);
void psix_file_select_right();
void psix_file_select_left();
void psix_file_cursor_moved();
void psix_file_delete();
void psix_file_move();
void psix_file_copy();
void psix_file_get_x_pointers();
void psix_file_rename();

#endif
