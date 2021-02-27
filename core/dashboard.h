#ifndef __DASHBOARD_H
#define __DASHBOARD_H

struct psix_dashboard_item {
  char name[50];
  char path[256];
  char img[50];
  //char img_over[50];
  int enabled;
  int w;
  int h;
};

enum psix_dashboard_state { DASH_MS0=1, DASH_UMD, DASH_MOD, DASH_NET };

extern int psix_dashboard_show_overlay;

int psix_dashboard_init();
int psix_dashboard_check_extension(char *filename,char *entension);
void psix_dashboard_scan_for_exec(char *path);
int psix_dashboard_add_item(struct psix_dashboard_item *new_item);
void psix_dashboard_set_state(int state);
void psix_dashboard_hide();
void psix_dashboard_show();
void psix_dashboard_click();
void psix_dashboard_cursor_moved();
void psix_dashboard_icon_set_overlay(int j, int i, int value);
void psix_dashboard_vdm_click();

#endif
