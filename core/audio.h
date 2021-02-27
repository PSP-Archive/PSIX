#ifndef __AUDIO_H
#define __AUDIO_H

extern SceUID psix_thread_audio;

void psix_audio_init();
void psix_audio_transition_in();
void psix_audio_transition_out();
void psix_audio_set_left(int x);
int psix_audio_scan(char *path);
void psix_audio_keypress(u32 pad);
void psix_audio_set_state(int state);
void psix_audio_draw_filelist();
void psix_audio_clear_filelist();
void psix_audio_start_thread();
void psix_audio_increment_file_line();
void psix_audio_decrement_file_line();
void psix_audio_play_next();
void psix_audio_play_prev();
void psix_audio_stop();
void psix_audio_play_file(char *name, char *path);
void psix_audio_increment_playlist_line();
void psix_audio_decrement_playlist_line();
void psix_audio_add_selection_to_playlist();
void psix_audio_update_info_window_time(char *time);
void psix_audio_update_info_window_title(char *title);
void psix_audio_set_checkbox(int box, int value);
void psix_audio_toggle_checkbox(int box);
void psix_audio_update_checkboxes();
void psix_audio_shuffle();
void psix_info_window_hide();
void psix_audio_cursor_moved();
void psix_audio_load_module();
void psix_avs_enable();
void psix_audio_pause();
void psix_audio_get_x_pointers();

extern int psix_audio_playing;
extern int psix_audio_module_loaded;
extern char psix_avs_enabled;
#endif
