#ifndef __OSK_H
#define __OSK_H

#define OSK_TEXT_MAX 50

extern char psix_osk_visible;
extern char psix_osk_text[OSK_TEXT_MAX];

void psix_osk_init();
void psix_osk_keypress(u32 pad);
int psix_osk_get_string(char *);
void psix_osk_start_transition_thread();
void psix_osk_hide();
void psix_osk_transition_in();
void psix_osk_increment_row();
void psix_osk_decrement_row();
void psix_osk_update_select();
void psix_osk_increment_col();
void psix_osk_decrement_col();
void psix_osk_char_selected(u32 pad);
void psix_osk_char_add(char c);
void psix_osk_text_update();

#endif
