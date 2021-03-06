#include "psix.h"

#define AUDIO_CWD_LEFT 10
#define AUDIO_FILES_LEFT 21
#define AUDIO_FILES_TOP 56
#define AUDIO_FILES_WIDTH 136
#define AUDIO_FILES_HEIGHT 151
#define AUDIO_PLAY_TIME_LEFT 167
#define AUDIO_PLAY_TRACK_LEFT 45
#define AUDIO_PLAY_TRACK_WIDTH 100
#define AUDIO_PLAYING_OVERLAY 0
#define AUDIO_MAX_FILES 1000
#define AUDIO_MAX_PLAYLIST 1000
#define AUDIO_PLAY_TRACK_FULL_WIDTH 1000
#define AUDIO_NUM_CHECKBOXES 7
#define AUDIO_PLAYLIST_LEFT 167
#define AUDIO_PLAYLIST_TOP 41
#define AUDIO_PLAYLIST_WIDTH 136
#define AUDIO_PLAYLIST_HEIGHT 166

#define AUDIO_PREV_WIDTH 18
#define AUDIO_PREV_HEIGHT 19
#define AUDIO_PREV_LEFT 196
#define AUDIO_PREV_TOP 215
#define AUDIO_NEXT_WIDTH 18
#define AUDIO_NEXT_HEIGHT 19
#define AUDIO_NEXT_LEFT 215
#define AUDIO_NEXT_TOP 215
#define AUDIO_PLAY_WIDTH 18
#define AUDIO_PLAY_HEIGHT 19
#define AUDIO_PLAY_LEFT 234
#define AUDIO_PLAY_TOP 215
#define AUDIO_PAUSE_WIDTH 18
#define AUDIO_PAUSE_HEIGHT 19
#define AUDIO_PAUSE_LEFT 253
#define AUDIO_PAUSE_TOP 215
#define AUDIO_STOP_WIDTH 18
#define AUDIO_STOP_HEIGHT 19
#define AUDIO_STOP_LEFT 272
#define AUDIO_STOP_TOP 215

#define AUDIO_NEW_LEFT 304
#define AUDIO_NEW_TOP 40
#define AUDIO_NEW_WIDTH 9
#define AUDIO_NEW_HEIGHT 38
#define AUDIO_DEL_LEFT 304
#define AUDIO_DEL_TOP 77
#define AUDIO_DEL_WIDTH 9
#define AUDIO_DEL_HEIGHT 38
#define AUDIO_ADD_LEFT 157
#define AUDIO_ADD_TOP 81
#define AUDIO_ADD_WIDTH 9
#define AUDIO_ADD_HEIGHT 38

#define AUDIO_CHECKBOX_SHUFFLE_WIDTH 16
#define AUDIO_CHECKBOX_SHUFLE_HEIGHT 15
#define AUDIO_CHECKBOX_SHUFFLE_LEFT 322
#define AUDIO_CHECKBOX_SHUFFLE_TOP 38
#define AUDIO_CHECKBOX_REPEAT_TRACK_WIDTH 16
#define AUDIO_CHECKBOX_REPEAT_TRACK_HEIGHT 15
#define AUDIO_CHECKBOX_REPEAT_TRACK_LEFT 322
#define AUDIO_CHECKBOX_REPEAT_TRACK_TOP 59
#define AUDIO_CHECKBOX_REPEAT_ALL_WIDTH 16
#define AUDIO_CHECKBOX_REPEAT_ALL_HEIGHT 15
#define AUDIO_CHECKBOX_REPEAT_ALL_LEFT 322
#define AUDIO_CHECKBOX_REPEAT_ALL_TOP 80
#define AUDIO_CHECKBOX_DISPLAY_CONTROL_WIDTH 16
#define AUDIO_CHECKBOX_DISPLAY_CONTROL_HEIGHT 15
#define AUDIO_CHECKBOX_DISPLAY_CONTROL_LEFT 322
#define AUDIO_CHECKBOX_DISPLAY_CONTROL_TOP 101
#define AUDIO_CHECKBOX_DISPLAY_INFO_WIDTH 16
#define AUDIO_CHECKBOX_DISPLAY_INFO_HEIGHT 15
#define AUDIO_CHECKBOX_DISPLAY_INFO_LEFT 322
#define AUDIO_CHECKBOX_DISPLAY_INFO_TOP 122
#define AUDIO_CHECKBOX_DISPLAY_PLAYLIST_WIDTH 16
#define AUDIO_CHECKBOX_DISPLAY_PLAYLIST_HEIGHT 155B
#define AUDIO_CHECKBOX_DISPLAY_PLAYLIST_LEFT 322
#define AUDIO_CHECKBOX_DISPLAY_PLAYLIST_TOP 143
#define AUDIO_CHECKBOX_ENABLE_REMOTE_WIDTH 16
#define AUDIO_CHECKBOX_ENABLE_REMOTE_HEIGHT 15
#define AUDIO_CHECKBOX_ENABLE_REMOTE_LEFT 322
#define AUDIO_CHECKBOX_ENABLE_REMOTE_TOP 164

#define AUDIO_VIS_LEFT 210
#define AUDIO_VIS_TOP 235
#define AUDIO_VIS_WIDTH 3
#define AUDIO_VIS_SPACE 1
#define AUDIO_VIS_NUM 15

int psix_audio_vis_value[AUDIO_VIS_NUM+1];

int psix_audio_state;
int psix_audio_file_line;
int psix_audio_file_num;
int psix_audio_file_line_max;
int psix_audio_file_line_start;
int psix_audio_playlist_line;
int psix_audio_playlist_num;
int psix_audio_playlist_line_max;
int psix_audio_playlist_line_start;
int psix_audio_playing;		/* are we playing */
int psix_audio_module_loaded;	/* do we have an audio library? */
int psix_audio_internal_playlist_line;
int psix_audio_internal_playlist_num;

int psix_audio_play_track_frame;
int psix_audio_play_track_width;
int psix_audio_play_track_direction;
int psix_audio_play_from;	/* file list or playlist */
int psix_audio_play_index;
int psix_audio_shuffle_index;

int psix_audio_paused;
int psix_audio_paused_count;

int psix_info_window_visible;
char psix_avs_enabled;

SceUID psix_thread_audio_play;

struct psix_audio_file_info {
	char path[1024];
	char name[100];
	int type;
};

enum psix_audio_states {AUDIO_FILE=10, AUDIO_ADD, AUDIO_NEW,
AUDIO_DEL, AUDIO_STOP, AUDIO_PLAY,
AUDIO_PAUSE, AUDIO_PREV, AUDIO_NEXT,
AUDIO_PLAYLIST};

enum psix_audio_checkbox_states {AUDIO_CHECKBOX_SHUFFLE=0, 
AUDIO_CHECKBOX_REPEAT_TRACK,
AUDIO_CHECKBOX_REPEAT_ALL,
AUDIO_CHECKBOX_DISPLAY_CONTROL,
AUDIO_CHECKBOX_DISPLAY_INFO,
AUDIO_CHECKBOX_DISPLAY_PLAYLIST,
AUDIO_CHECKBOX_ENABLE_REMOTE};

enum psix_checkbox_states {CHECKBOX_OFF, CHECKBOX_OFF_SELECTED,
CHECKBOX_ON, CHECKBOX_ON_SELECTED};

enum psix_audio_types {AUDIO_DIR, AUDIO_MP3};

struct psix_audio_file_info *psix_audio_files[AUDIO_MAX_FILES];
struct psix_audio_file_info *psix_audio_playlist[AUDIO_MAX_PLAYLIST];
struct psix_audio_file_info *psix_audio_internal_playlist[AUDIO_MAX_PLAYLIST];

int psix_audio_shuffle_order[AUDIO_MAX_PLAYLIST];

char psix_audio_checkboxes[AUDIO_NUM_CHECKBOXES];

void psix_audio_init()
{
	psix_audio_set_state(AUDIO_FILE);

	int i;
	for (i=0; i < AUDIO_NUM_CHECKBOXES; i++)
		psix_audio_checkboxes[i] = CHECKBOX_OFF;
	for (i=0; i < AUDIO_MAX_FILES; i++)
		psix_audio_files[i] = 0;
	for (i=0; i < AUDIO_MAX_PLAYLIST; i++)
		psix_audio_playlist[i] = 0;
	for (i=0; i < AUDIO_MAX_PLAYLIST; i++)
		psix_audio_shuffle_order[i] = 0;

	psix_audio_checkboxes[3] = CHECKBOX_ON;
	psix_audio_checkboxes[4] = CHECKBOX_ON;

	sdi_new("audio_files");
	sdi_set_x("audio_files",-480+AUDIO_FILES_LEFT);
	sdi_set_y("audio_files",AUDIO_FILES_TOP);
	sdi_set_overlay("audio_files",1);
	sdi_set_enabled("audio_files",0);
	sdi_create_image("audio_files",136,151);

	sdi_new("audio_playlist");
	sdi_set_x("audio_playlist",-480+AUDIO_PLAYLIST_LEFT);
	sdi_set_y("audio_playlist",41);
	sdi_set_overlay("audio_playlist",1);
	sdi_set_enabled("audio_playlist",0);
	sdi_create_image("audio_playlist",136,166);

	sdi_new("audio_cwd");
	sdi_set_x("audio_cwd",-480+AUDIO_CWD_LEFT);
	sdi_set_y("audio_cwd",39);
	sdi_set_overlay("audio_cwd",1);
	sdi_set_enabled("audio_cwd",0);
	sdi_create_image("audio_cwd",148,12);

	sdi_new("audio_play_time");
	sdi_set_x("audio_play_time",-480+AUDIO_PLAY_TIME_LEFT);
	sdi_set_y("audio_play_time",219);
	sdi_set_overlay("audio_play_time",1);
	sdi_set_enabled("audio_play_time",0);
	sdi_move_below("audio_play_time","audio_playing_overlay");
	sdi_create_image("audio_play_time",24,12);

	sdi_new("audio_play_track_full");
	sdi_create_image("audio_play_track_full",AUDIO_PLAY_TRACK_FULL_WIDTH,12);

	sdi_new("audio_play_track");
	sdi_set_x("audio_play_track",-480+AUDIO_PLAY_TRACK_LEFT);
	sdi_set_y("audio_play_track",219);
	sdi_set_overlay("audio_play_track",1);
	sdi_set_enabled("audio_play_track",0);
	sdi_move_below("audio_play_track","audio_playing_overlay");
	sdi_create_image("audio_play_track",100,12);

	sdi_new("info_window_text");
	sdi_set_x("info_window_text",6);
	sdi_set_y("info_window_text",212);
	sdi_move_below("info_window_text","page_file");
	sdi_set_overlay("info_window_text",1);
	sdi_create_image("info_window_text",178,22);

	sdi_new("audio_checkbox_shuffle");
	sdi_set_position("audio_checkbox_shuffle",322,38);
	sdi_set_overlay("audio_checkbox_shuffle",1);

	sdi_new("audio_checkbox_repeat_track");
	sdi_set_position("audio_checkbox_repeat_track",322,59);
	sdi_set_overlay("audio_checkbox_repeat_track",1);

	sdi_new("audio_checkbox_repeat_all");
	sdi_set_position("audio_checkbox_repeat_all",322,80);
	sdi_set_overlay("audio_checkbox_repeat_all",1);

	sdi_new("audio_checkbox_display_control");
	sdi_set_position("audio_checkbox_display_control",322,101);
	sdi_set_overlay("audio_checkbox_display_control",1);

	sdi_new("audio_checkbox_display_info");
	sdi_set_position("audio_checkbox_display_info",322,122);
	sdi_set_overlay("audio_checkbox_display_info",1);

	sdi_new("audio_checkbox_display_playlist");
	sdi_set_position("audio_checkbox_display_playlist",322,143);
	sdi_set_overlay("audio_checkbox_display_playlist",1);

	sdi_new("audio_checkbox_enable_remote");
	sdi_set_position("audio_checkbox_enable_remote",322,164);
	sdi_set_overlay("audio_checkbox_enable_remote",1);

	psix_audio_file_line = 1;
	psix_audio_file_num = 0;
	psix_audio_file_line_start = 1;
	psix_audio_file_line_max = 12;
	psix_audio_playlist_line = 1;
	psix_audio_playlist_num = 0;
	psix_audio_playlist_line_start = 1;
	psix_audio_playlist_line_max = 14;
	psix_audio_internal_playlist_line = 1;
	psix_audio_internal_playlist_num = 0;
	psix_audio_playing = 0;
	psix_audio_play_track_frame = 0;
	psix_audio_play_track_width = 0;
	psix_audio_play_track_direction = 0;
	psix_audio_paused = 0;
	psix_audio_paused_count = 0;
	psix_audio_play_from = AUDIO_FILE;
	psix_audio_play_index = 0;
	psix_info_window_visible = 0;
	psix_avs_enabled = 0;

	psix_audio_clear_filelist();

	if (psix_audio_scan("ms0:/psp/music") == -1)
	{
		psix_audio_scan("ms0:/");
		sdi_print_line_max_compress("audio_cwd","font_black","ms0:/",148,1,1);
	}
	else
		sdi_print_line_max_compress("audio_cwd","font_black","ms0:/psp/music/",148,1,1);

	psix_audio_draw_filelist();
	psix_audio_start_thread();

	psix_audio_vis_init();
	psix_avs_enable();

	memset(psix_audio_vis_value,0,AUDIO_VIS_NUM*4);

	psix_audio_vis_update();

	/*
	sdi_new("tes");
	sdi_copy_image("audio_vis_16","tes");
	sdi_set_position("tes",211,236);
	sdi_set_overlay("tes",1);
	sdi_set_enabled("tes",1);
	*/

	//sdi_set_overlay("audio_vis_13",1);
	//sdi_set_enabled("audio_vis_13",1);
	psix_audio_get_x_pointers();

}

void psix_avs_enable()
{
	psix_avs_enabled = 1;
	sdi_set_enabled("avs",1);

	char sdi_name[50];
	int i;
	for (i=0; i < AUDIO_VIS_NUM; i++)
	  {
	    sprintf(sdi_name,"vis_bar_%d",i);
	    sdi_set_enabled(sdi_name,1);
	  }
}

void psix_avs_disable()
{
	psix_avs_enabled = 0;
	sdi_set_enabled("avs",0);

	char sdi_name[50];
	int i;
	for (i=0; i < AUDIO_VIS_NUM; i++)
	  {
	    sprintf(sdi_name,"vis_bar_%d",i);
	    sdi_set_enabled(sdi_name,0);
	  }
}

void report(char *str)
{
	printf("%s\n",str);
	sceKernelDelayThread(2000000);
}

void psix_audio_transition_in()
{
	psix_audio_cursor_moved();
	sdi_set_enabled("page_audio",1);
	sdi_set_enabled("audio_cwd",1);
	sdi_set_enabled("audio_files",1);
	sdi_set_enabled("audio_playlist",1);
	sdi_set_enabled("audio_play_time",1);
	sdi_set_enabled("audio_play_track",1);
	sdi_set_enabled("audio_playing_overlay",1);
	sdi_set_enabled("audio_checkbox_shuffle",1);
	sdi_set_enabled("audio_checkbox_repeat_track",1);
	sdi_set_enabled("audio_checkbox_repeat_all",1);
	sdi_set_enabled("audio_checkbox_display_control",1);
	sdi_set_enabled("audio_checkbox_display_info",1);
	sdi_set_enabled("audio_checkbox_display_playlist",1);
	sdi_set_enabled("audio_checkbox_enable_remote",1);
	sdi_set_enabled("audio_add_on",psix_audio_state==AUDIO_ADD);
	sdi_set_enabled("audio_new_on",psix_audio_state==AUDIO_NEW);
	sdi_set_enabled("audio_del_on",psix_audio_state==AUDIO_DEL);
	sdi_set_enabled("audio_prev_on",psix_audio_state==AUDIO_PREV);
	sdi_set_enabled("audio_next_on",psix_audio_state==AUDIO_NEXT);
	sdi_set_enabled("audio_play_on",psix_audio_state==AUDIO_PLAY);
	sdi_set_enabled("audio_pause_on",psix_audio_state==AUDIO_PAUSE);
	sdi_set_enabled("audio_stop_on",psix_audio_state==AUDIO_STOP);

}

void psix_audio_transition_out()
{
	sdi_set_enabled("page_audio",0);
	sdi_set_enabled("audio_cwd",0);
	sdi_set_enabled("audio_files",0);
	sdi_set_enabled("audio_playlist",0);
	sdi_set_enabled("audio_play_time",0);
	sdi_set_enabled("audio_play_track",0);
	sdi_set_enabled("audio_playing_overlay",0);
	sdi_set_enabled("audio_checkbox_shuffle",0);
	sdi_set_enabled("audio_checkbox_repeat_track",0);
	sdi_set_enabled("audio_checkbox_repeat_all",0);
	sdi_set_enabled("audio_checkbox_display_control",0);
	sdi_set_enabled("audio_checkbox_display_info",0);
	sdi_set_enabled("audio_checkbox_display_playlist",0);
	sdi_set_enabled("audio_checkbox_enable_remote",0);
	sdi_set_enabled("audio_add_on",0);
	sdi_set_enabled("audio_new_on",0);
	sdi_set_enabled("audio_del_on",0);
	sdi_set_enabled("audio_prev_on",0);
	sdi_set_enabled("audio_next_on",0);
	sdi_set_enabled("audio_play_on",0);
	sdi_set_enabled("audio_pause_on",0);
	sdi_set_enabled("audio_stop_on",0);
}

int *psix_audio_x_pointer[22];


void psix_audio_get_x_pointers()
{
  psix_audio_x_pointer[0] = sdi_get_x_ptr("page_audio");
  psix_audio_x_pointer[1] = sdi_get_x_ptr("audio_cwd");
  psix_audio_x_pointer[2] = sdi_get_x_ptr("audio_files");
  psix_audio_x_pointer[3] = sdi_get_x_ptr("audio_playlist");
  psix_audio_x_pointer[4] = sdi_get_x_ptr("audio_play_time");
  psix_audio_x_pointer[5] = sdi_get_x_ptr("audio_play_track");
  psix_audio_x_pointer[6] = sdi_get_x_ptr("audio_playing_overlay");
  psix_audio_x_pointer[7] = sdi_get_x_ptr("audio_add_on");
  psix_audio_x_pointer[8] = sdi_get_x_ptr("audio_new_on");
  psix_audio_x_pointer[9] = sdi_get_x_ptr("audio_del_on");
  psix_audio_x_pointer[10] = sdi_get_x_ptr("audio_prev_on");
  psix_audio_x_pointer[11] = sdi_get_x_ptr("audio_next_on");
  psix_audio_x_pointer[12] = sdi_get_x_ptr("audio_play_on");
  psix_audio_x_pointer[13] = sdi_get_x_ptr("audio_pause_on");
  psix_audio_x_pointer[14] = sdi_get_x_ptr("audio_stop_on");
  psix_audio_x_pointer[15] = sdi_get_x_ptr("audio_checkbox_shuffle");
  psix_audio_x_pointer[16] = sdi_get_x_ptr("audio_checkbox_repeat_track");
  psix_audio_x_pointer[17] = sdi_get_x_ptr("audio_checkbox_repeat_all");
  psix_audio_x_pointer[18] = sdi_get_x_ptr("audio_checkbox_display_control");
  psix_audio_x_pointer[19] = sdi_get_x_ptr("audio_checkbox_display_info");
  psix_audio_x_pointer[20] = sdi_get_x_ptr("audio_checkbox_display_playlist");
  psix_audio_x_pointer[21] = sdi_get_x_ptr("audio_checkbox_enable_remote");
}

void psix_audio_set_left(int x)
{
  *(psix_audio_x_pointer[0]) = x;
  *(psix_audio_x_pointer[1]) = x+AUDIO_CWD_LEFT;
  *(psix_audio_x_pointer[2]) = x+AUDIO_FILES_LEFT;
  *(psix_audio_x_pointer[3]) = x+AUDIO_PLAYLIST_LEFT;
  *(psix_audio_x_pointer[4]) = x+AUDIO_PLAY_TIME_LEFT;
  *(psix_audio_x_pointer[5]) = x+AUDIO_PLAY_TRACK_LEFT;
  *(psix_audio_x_pointer[6]) = x+AUDIO_PLAYING_OVERLAY;
  *(psix_audio_x_pointer[7]) = x+157;
  *(psix_audio_x_pointer[8]) = x+304;
  *(psix_audio_x_pointer[9]) = x+304;
  *(psix_audio_x_pointer[10]) = x+196;
  *(psix_audio_x_pointer[11]) = x+215;
  *(psix_audio_x_pointer[12]) = x+234;
  *(psix_audio_x_pointer[13]) = x+253;
  *(psix_audio_x_pointer[14]) = x+272;
  *(psix_audio_x_pointer[15]) = x+322;
  *(psix_audio_x_pointer[16]) = x+322;
  *(psix_audio_x_pointer[17]) = x+322;
  *(psix_audio_x_pointer[18]) = x+322;
  *(psix_audio_x_pointer[19]) = x+322;
  *(psix_audio_x_pointer[20]) = x+322;
  *(psix_audio_x_pointer[21]) = x+322;

}

void psix_audio_set_track_title(char *title)
{
	sdi_clear("audio_play_track_full");
	psix_audio_play_track_width = sdi_print_line_max_compress("audio_play_track_full","font_white",title,AUDIO_PLAY_TRACK_FULL_WIDTH,1,0);
	sdi_copy_part("audio_play_track","audio_play_track_full",psix_audio_play_track_frame,0,psix_audio_play_track_frame+AUDIO_PLAY_TRACK_WIDTH,11);
}

void psix_audio_set_track_time(char *time)
{
	sdi_clear("audio_play_time");
	sdi_print_line_max_compress("audio_play_time","font_white",time,24,1,1);
}

void psix_audio_add_filelist(char *name,char *path,int type)
{
	/* find the first free spot */
	int i;
	for (i=0;i < AUDIO_MAX_FILES; i++)
		if (!psix_audio_files[i])
			break;
	if (i == AUDIO_MAX_FILES) return;

	psix_audio_file_num = i+1;

	psix_audio_files[i] = (struct psix_audio_file_info*)malloc(sizeof(struct psix_audio_file_info));
	strncpy(psix_audio_files[i]->name,name,99);
	strncpy(psix_audio_files[i]->path,path,1023);
	(psix_audio_files[i]->path)[1023] = 0;
	(psix_audio_files[i]->name)[98] = 0;
	psix_audio_files[i]->type = type;
}

void psix_audio_draw_filelist()
{
	int i=psix_audio_file_line_start;

	while (i < psix_audio_file_line_start + psix_audio_file_line_max && i <= psix_audio_file_num)
	{
		if (i == psix_audio_file_line && psix_audio_state == AUDIO_FILE)
			sdi_print_line_max_compress("audio_files","font_blue",psix_audio_files[i-1]->name,136,1,i-psix_audio_file_line_start+1);
		else
			sdi_print_line_max_compress("audio_files","font_black",psix_audio_files[i-1]->name,136,1,i-psix_audio_file_line_start+1);

		i++;
	}
}

void psix_audio_clear_filelist()
{
	int i;
	for (i=0; i < psix_audio_file_num; i++)
	{
		free(psix_audio_files[i]);
		psix_audio_files[i] = 0;
	}

	psix_audio_file_line = 1;
	psix_audio_file_num = 0;
	psix_audio_file_line_start = 1;
	sdi_clear("audio_files");
}

void psix_audio_add_playlist(char *name,char *path,int type)
{
	int i;
	for (i=0;i < AUDIO_MAX_PLAYLIST; i++)
		if (!psix_audio_playlist[i])
			break;
	if (i == AUDIO_MAX_PLAYLIST) return;

	psix_audio_playlist_num = i+1;

	psix_audio_playlist[i] = (struct psix_audio_file_info*)malloc(sizeof(struct psix_audio_file_info));
	strncpy(psix_audio_playlist[i]->name,name,99);
	strncpy(psix_audio_playlist[i]->path,path,1023);
	(psix_audio_playlist[i]->path)[1023] = 0;
	(psix_audio_playlist[i]->name)[98] = 0;
	psix_audio_playlist[i]->type = type;
}

void psix_audio_draw_playlist()
{
	int i=psix_audio_playlist_line_start;

	while (i < psix_audio_playlist_line_start + psix_audio_playlist_line_max && i <= psix_audio_playlist_num)
	{
		if (i == psix_audio_playlist_line && (psix_audio_state == AUDIO_PLAYLIST || psix_audio_state == AUDIO_DEL))
			sdi_print_line_max_compress("audio_playlist","font_blue",psix_audio_playlist[i-1]->name,136,1,i-psix_audio_playlist_line_start+1);
		else
			sdi_print_line_max_compress("audio_playlist","font_black",psix_audio_playlist[i-1]->name,136,1,i-psix_audio_playlist_line_start+1);

		i++;
	}
}

void psix_audio_clear_playlist()
{
	int i;
	for (i=0; i < psix_audio_playlist_num; i++)
	{
		free(psix_audio_playlist[i]);
		psix_audio_playlist[i] = 0;
	}

	psix_audio_playlist_line = 1;
	psix_audio_playlist_num = 0;
	psix_audio_playlist_line_start = 1;
	sdi_clear("audio_playlist");

	if (psix_audio_play_from == AUDIO_PLAYLIST)
		psix_audio_play_index = -1;
}

void psix_audio_remove_playlist_selection()
{
	if (psix_audio_playlist_num <= 0)
		return;

	int i=psix_audio_playlist_line-1;
	free(psix_audio_playlist[i]);

	for (; i < psix_audio_playlist_num-1; i++)
	{
		psix_audio_playlist[i] = psix_audio_playlist[i+1];
	}
	psix_audio_playlist[i] = 0;
	psix_audio_playlist_num--;

	if (psix_audio_playlist_line > psix_audio_playlist_num)
		psix_audio_playlist_line = psix_audio_playlist_num;

	if (psix_audio_playlist_line == 0)
		psix_audio_playlist_line = 1;
}

void psix_audio_clear_internal_playlist()
{
	int i;
	for (i=0; i < psix_audio_internal_playlist_num; i++)
	{
		free(psix_audio_internal_playlist[i]);
		psix_audio_internal_playlist[i] = 0;
	}

	psix_audio_internal_playlist_line = 1;
	psix_audio_internal_playlist_num = 0;
}

void psix_audio_copy_to_internal_playlist(struct psix_audio_file_info **list, int num, int line)
{
	psix_audio_clear_internal_playlist();
	psix_audio_internal_playlist_line = line;

	int i,count=0;
	for (i=0; i < num; i++)
		if (list[i]->type == AUDIO_MP3)
		{
			psix_audio_internal_playlist[count] = malloc(sizeof(struct psix_audio_file_info));
			memcpy(psix_audio_internal_playlist[count],list[i],sizeof(struct psix_audio_file_info));
			count++;
		}
		else if (i+1 < line)
			psix_audio_internal_playlist_line--;

	psix_audio_internal_playlist_num = count;
	psix_audio_shuffle();
}

void psix_audio_shuffle()
{
	/* create a list of random internal playlist indicies */
	int num = psix_audio_internal_playlist_num;
	int i;

	int nums[AUDIO_MAX_PLAYLIST];
	for (i=0; i < AUDIO_MAX_PLAYLIST; i++)
		nums[i] = i+1;

	for (i=0; i < psix_audio_internal_playlist_num; i++)
	{
		srand(clock());
		int r = rand()%(num--);
		psix_audio_shuffle_order[i] = nums[r];
		int j;
		for (j=r; j <= num; j++)
			nums[j] = nums[j+1];
	}
	psix_audio_shuffle_index = 0;
}

void psix_audio_select_file()
{
	if (psix_audio_file_num < 1 || !psix_audio_files[psix_audio_file_line-1])
		return;

	switch(psix_audio_files[psix_audio_file_line-1]->type)
	{
	case AUDIO_MP3:
		psix_audio_play_file(psix_audio_files[psix_audio_file_line-1]->name,psix_audio_files[psix_audio_file_line-1]->path);
		psix_audio_play_index = psix_audio_file_line-1;
		psix_audio_play_from = AUDIO_FILE;
		psix_audio_copy_to_internal_playlist(psix_audio_files,psix_audio_file_num,psix_audio_file_line);
		break;
	case AUDIO_DIR:
		sdi_clear("audio_cwd");
		sdi_print_line_max_compress("audio_cwd","font_black",psix_audio_files[psix_audio_file_line-1]->path,148,1,1);     
		char path[1024];
		strcpy(path,psix_audio_files[psix_audio_file_line-1]->path);
		psix_audio_clear_filelist();
		psix_audio_scan(path);
		psix_audio_draw_filelist();
		break;
	}

}

void psix_audio_select_playlist_file()
{
	if (psix_audio_playlist_num < 1 || !psix_audio_playlist[psix_audio_playlist_line-1])
		return;

	psix_audio_play_file(psix_audio_playlist[psix_audio_playlist_line-1]->name,psix_audio_playlist[psix_audio_playlist_line-1]->path);

	psix_audio_play_index = psix_audio_playlist_line-1;
}

int psix_audio_scan(char *path)
{
	char buf[1024],buf2[1024];
	SceIoDirent dirent;

	int dd = sceIoDopen(path);

	if (dd < 0)
		return -1;

	memset(&dirent,0,sizeof(SceIoDirent));
	while (sceIoDread(dd,&dirent) > 0)
	{
		if (dirent.d_stat.st_attr & FIO_SO_IFDIR)
		{
			if (strcmp(dirent.d_name,"."))
			{
				sprintf(buf,"<%s>",dirent.d_name);
				sprintf(buf2,"%s%s%s",path,(path[strlen(path)-1]=='/')?"":"/",dirent.d_name);
				/* if its .. we want to use the parent directory */
				if (!strcmp("<..>",buf))
				{
					sprintf(buf2,"%s",path);
					while (buf2[strlen(buf2)-1] != '/')
						buf2[strlen(buf2)-1] = 0;
					buf2[strlen(buf2)-1] = 0;
				}

				psix_audio_add_filelist(buf,buf2,AUDIO_DIR);
			}
		}
	}

	sceIoDclose(dd);

	/* start over and list files */
	dd = sceIoDopen(path);

	if (dd < 0)
		return -1;

	memset(&dirent,0,sizeof(SceIoDirent));
	while (sceIoDread(dd,&dirent) > 0)
	{
		if (dirent.d_stat.st_attr & FIO_SO_IFREG)		
		{
			if (strncasecmp(&dirent.d_name[strlen(dirent.d_name) - 3], "mp3", 3) == 0)
			{
				sprintf(buf,"%s%s%s",path,(path[strlen(path)-1]=='/')?"":"/",dirent.d_name);
				psix_audio_add_filelist(dirent.d_name,buf,AUDIO_MP3);
			}
		}
	}

	sceIoDclose(dd);

	return 0;
}

void psix_audio_keypress(u32 pad)
{
	if (pad & PSP_CTRL_DOWN)
	{
		switch (psix_audio_state)
		{
		case AUDIO_FILE:
			psix_audio_increment_file_line();
			break;
		case AUDIO_PLAYLIST:
			psix_audio_increment_playlist_line();
			break;
		case AUDIO_NEW:
			psix_audio_set_state(AUDIO_DEL);
			break;
		case AUDIO_CHECKBOX_SHUFFLE:
			psix_audio_set_state(AUDIO_CHECKBOX_REPEAT_TRACK);
			break;
		case AUDIO_CHECKBOX_REPEAT_TRACK:
			psix_audio_set_state(AUDIO_CHECKBOX_REPEAT_ALL);
			break;
		case AUDIO_CHECKBOX_REPEAT_ALL:
			psix_audio_set_state(AUDIO_CHECKBOX_DISPLAY_CONTROL);
			break;
		case AUDIO_CHECKBOX_DISPLAY_CONTROL:
			psix_audio_set_state(AUDIO_CHECKBOX_DISPLAY_INFO);
			break;
		case AUDIO_CHECKBOX_DISPLAY_INFO:
			psix_audio_set_state(AUDIO_CHECKBOX_DISPLAY_PLAYLIST);
			break;
		case AUDIO_CHECKBOX_DISPLAY_PLAYLIST:
			psix_audio_set_state(AUDIO_CHECKBOX_ENABLE_REMOTE);
			break;
		case AUDIO_CHECKBOX_ENABLE_REMOTE:
			psix_audio_set_state(AUDIO_STOP);
			break;
		}
	}

	if (pad & PSP_CTRL_UP)
	{
		switch (psix_audio_state)
		{
		case AUDIO_FILE:
			psix_audio_decrement_file_line();
		case AUDIO_PLAYLIST:
			psix_audio_decrement_playlist_line();
			break;
		case AUDIO_DEL:
			psix_audio_set_state(AUDIO_NEW);
			break;
		case AUDIO_STOP:
			psix_audio_set_state(AUDIO_CHECKBOX_ENABLE_REMOTE);
			break;
		case AUDIO_CHECKBOX_ENABLE_REMOTE:
			psix_audio_set_state(AUDIO_CHECKBOX_DISPLAY_PLAYLIST);
			break;
		case AUDIO_CHECKBOX_DISPLAY_PLAYLIST:
			psix_audio_set_state(AUDIO_CHECKBOX_DISPLAY_INFO);
			break;
		case AUDIO_CHECKBOX_DISPLAY_INFO:
			psix_audio_set_state(AUDIO_CHECKBOX_DISPLAY_CONTROL);
			break;
		case AUDIO_CHECKBOX_DISPLAY_CONTROL:
			psix_audio_set_state(AUDIO_CHECKBOX_REPEAT_ALL);
			break;
		case AUDIO_CHECKBOX_REPEAT_ALL:
			psix_audio_set_state(AUDIO_CHECKBOX_REPEAT_TRACK);
			break;
		case AUDIO_CHECKBOX_REPEAT_TRACK:
			psix_audio_set_state(AUDIO_CHECKBOX_SHUFFLE);
			break;
		}
	}

	if (pad & PSP_CTRL_RIGHT)
	{
		switch (psix_audio_state)
		{
		case AUDIO_FILE:
			psix_audio_set_state(AUDIO_ADD);
			break;
		case AUDIO_ADD:
			if (psix_audio_playlist_num > 0)
				psix_audio_set_state(AUDIO_PLAYLIST);
			else
				psix_audio_set_state(AUDIO_DEL);
			break;
		case AUDIO_DEL:
			psix_audio_set_state(AUDIO_CHECKBOX_SHUFFLE);
			break;
		case AUDIO_STOP:
			psix_audio_set_state(AUDIO_CHECKBOX_ENABLE_REMOTE);
			break;
		case AUDIO_PAUSE:
			psix_audio_set_state(AUDIO_STOP);
			break;
		case AUDIO_PLAY:
			psix_audio_set_state(AUDIO_PAUSE);
			break;
		case AUDIO_NEXT:
			psix_audio_set_state(AUDIO_PLAY);
			break;
		case AUDIO_PREV:
			psix_audio_set_state(AUDIO_NEXT);
			break;
		case AUDIO_PLAYLIST:
			psix_audio_set_state(AUDIO_DEL);
			break;
		case AUDIO_NEW:
			psix_audio_set_state(AUDIO_CHECKBOX_SHUFFLE);
			break;
		case AUDIO_CHECKBOX_SHUFFLE:
			psix_audio_set_state(AUDIO_STOP);
			break;
		case AUDIO_CHECKBOX_REPEAT_TRACK:
			psix_audio_set_state(AUDIO_STOP);
			break;
		case AUDIO_CHECKBOX_REPEAT_ALL:
			psix_audio_set_state(AUDIO_STOP);
			break;
		case AUDIO_CHECKBOX_DISPLAY_CONTROL:
			psix_audio_set_state(AUDIO_STOP);
			break;
		case AUDIO_CHECKBOX_DISPLAY_INFO:
			psix_audio_set_state(AUDIO_STOP);
			break;
		case AUDIO_CHECKBOX_DISPLAY_PLAYLIST:
			psix_audio_set_state(AUDIO_STOP);
			break;
		case AUDIO_CHECKBOX_ENABLE_REMOTE:
			psix_audio_set_state(AUDIO_STOP);
			break;
		}
	} 
	if (pad & PSP_CTRL_LEFT)
	{
		switch (psix_audio_state)
		{
		case AUDIO_STOP:
			psix_audio_set_state(AUDIO_PAUSE);
			break;
		case AUDIO_PAUSE:
			psix_audio_set_state(AUDIO_PLAY);
			break;
		case AUDIO_PLAY:
			psix_audio_set_state(AUDIO_NEXT);
			break;
		case AUDIO_NEXT:
			psix_audio_set_state(AUDIO_PREV);
			break;
		case AUDIO_PREV:
			psix_audio_set_state(AUDIO_FILE);
			break;
		case AUDIO_FILE:
			psix_audio_set_state(AUDIO_PREV);
			break;
		case AUDIO_ADD:
			psix_audio_set_state(AUDIO_FILE);
			break;
		case AUDIO_DEL:
			if (psix_audio_playlist_num > 0)
				psix_audio_set_state(AUDIO_PLAYLIST);
			else
				psix_audio_set_state(AUDIO_ADD);
			break;
		case AUDIO_NEW:
			if (psix_audio_playlist_num > 0)
				psix_audio_set_state(AUDIO_PLAYLIST);
			else
				psix_audio_set_state(AUDIO_ADD);
			break;
		case AUDIO_PLAYLIST:
			psix_audio_set_state(AUDIO_ADD);
			break;
		case AUDIO_CHECKBOX_SHUFFLE:
			psix_audio_set_state(AUDIO_DEL);
			break;
		case AUDIO_CHECKBOX_REPEAT_TRACK:
			psix_audio_set_state(AUDIO_DEL);
			break;
		case AUDIO_CHECKBOX_REPEAT_ALL:
			psix_audio_set_state(AUDIO_DEL);
			break;
		case AUDIO_CHECKBOX_DISPLAY_CONTROL:
			psix_audio_set_state(AUDIO_DEL);
			break;
		case AUDIO_CHECKBOX_DISPLAY_INFO:
			psix_audio_set_state(AUDIO_DEL);
			break;
		case AUDIO_CHECKBOX_DISPLAY_PLAYLIST:
			psix_audio_set_state(AUDIO_DEL);
			break;
		case AUDIO_CHECKBOX_ENABLE_REMOTE:
			psix_audio_set_state(AUDIO_DEL);
			break;
		}
	}
	if (pad & PSP_CTRL_CROSS)
	{
	  int x = psix_cursor_x;
	  int y = psix_cursor_y;
	    
	  if (psix_avs_enabled)
	    if (y >= AVS_PAUSE_TOP && y <= AVS_PAUSE_TOP+AVS_PAUSE_HEIGHT &&
		x >= AVS_PAUSE_LEFT && x <= AVS_STOP_LEFT+AVS_STOP_WIDTH)
	      return;

		switch (psix_audio_state)
		{
		case AUDIO_FILE:
			psix_audio_select_file();
			break;
		case AUDIO_PAUSE:
			psix_audio_pause();
			break;
		case AUDIO_PLAY:
			if (psix_audio_paused)
			{
				psix_audio_paused = 0;
				scePowerSetClockFrequency(333, 333, 166);
				MP3_Pause();
			}
			else
				psix_audio_select_file();
			break;
		case AUDIO_STOP:
			psix_audio_stop();
			break;
		case AUDIO_NEXT:
			psix_audio_play_next();
			break;
		case AUDIO_PREV:
			psix_audio_play_prev();
			break;
		case AUDIO_ADD:
			psix_audio_add_selection_to_playlist();
			break;
		case AUDIO_PLAYLIST:
			psix_audio_play_from = AUDIO_PLAYLIST;
			psix_audio_select_playlist_file();
			psix_audio_copy_to_internal_playlist(psix_audio_playlist,psix_audio_playlist_num,psix_audio_playlist_line);
			break;
		case AUDIO_NEW:
			psix_audio_clear_playlist();
			break;
		case AUDIO_DEL:
			psix_audio_remove_playlist_selection();
			sdi_clear("audio_playlist");
			psix_audio_draw_playlist();
			break;
		case AUDIO_CHECKBOX_SHUFFLE:
		case AUDIO_CHECKBOX_REPEAT_TRACK:
		case AUDIO_CHECKBOX_REPEAT_ALL:
		case AUDIO_CHECKBOX_DISPLAY_CONTROL:
		case AUDIO_CHECKBOX_DISPLAY_INFO:
		case AUDIO_CHECKBOX_DISPLAY_PLAYLIST:
		case AUDIO_CHECKBOX_ENABLE_REMOTE:
			psix_audio_toggle_checkbox(psix_audio_state);
			break;
		}
	}
	if (pad & PSP_CTRL_TRIANGLE)
	{
		switch (psix_audio_state)
		{
		case AUDIO_FILE:
			psix_audio_add_selection_to_playlist();
			break;
		case AUDIO_PLAYLIST:
			psix_audio_remove_playlist_selection();
			sdi_clear("audio_playlist");
			psix_audio_draw_playlist();
			break;
		} 
	}
}

char psix_audio_prev_enabled = 0;
char psix_audio_next_enabled = 0;
char psix_audio_play_enabled = 0;
char psix_audio_pause_enabled = 0;
char psix_audio_stop_enabled = 0;


int psix_audio_mouse_on_object()
{
	int x = (int)psix_cursor_x;
	int y = (int)psix_cursor_y;

	if (x >= AUDIO_FILES_LEFT && x <= AUDIO_FILES_LEFT+AUDIO_FILES_WIDTH &&
		y >= AUDIO_FILES_TOP && y <= AUDIO_FILES_TOP+AUDIO_FILES_HEIGHT)
	{
		int line = (y-AUDIO_FILES_TOP)/12 + psix_audio_file_line_start;
		if (line <= psix_audio_file_num && psix_audio_file_line != line)
		{
			psix_audio_file_line = line;
			psix_audio_draw_filelist();
		}
		return AUDIO_FILE;
	}

	if (x >= AUDIO_PLAYLIST_LEFT && x <= AUDIO_PLAYLIST_LEFT+AUDIO_PLAYLIST_WIDTH &&
		y >= AUDIO_PLAYLIST_TOP && y <= AUDIO_PLAYLIST_TOP+AUDIO_PLAYLIST_HEIGHT)
	{
		int line = (y-AUDIO_PLAYLIST_TOP)/12 + psix_audio_playlist_line_start;
		if (line <= psix_audio_playlist_num && psix_audio_playlist_line != line)
		{
			psix_audio_playlist_line = line;
			psix_audio_draw_playlist();
		}
		return AUDIO_PLAYLIST;
	}

	if (x >= AUDIO_PLAYLIST_LEFT && x <= AUDIO_PLAYLIST_LEFT+AUDIO_PLAYLIST_WIDTH &&
		y >= AUDIO_PLAYLIST_TOP && y <= AUDIO_PLAYLIST_TOP+AUDIO_PLAYLIST_HEIGHT)
		return AUDIO_PLAYLIST;

	if (y >= AUDIO_STOP_TOP && y <= AUDIO_STOP_TOP + AUDIO_STOP_HEIGHT)
	{
		if (x >= AUDIO_PREV_LEFT && x <= AUDIO_PREV_LEFT + AUDIO_PREV_WIDTH)
			return AUDIO_PREV;

		if (x >= AUDIO_NEXT_LEFT && x <= AUDIO_NEXT_LEFT + AUDIO_NEXT_WIDTH)
			return AUDIO_NEXT;

		if (x >= AUDIO_PLAY_LEFT && x <= AUDIO_PLAY_LEFT + AUDIO_PLAY_WIDTH)
			return AUDIO_PLAY;

		if (x >= AUDIO_PAUSE_LEFT && x <= AUDIO_PAUSE_LEFT + AUDIO_PAUSE_WIDTH)
			return AUDIO_PAUSE;

		if (x >= AUDIO_STOP_LEFT && x <= AUDIO_STOP_LEFT + AUDIO_STOP_WIDTH)
			return AUDIO_STOP;
	} 

	if (x >= AUDIO_CHECKBOX_SHUFFLE_LEFT && x <= AUDIO_CHECKBOX_SHUFFLE_LEFT + AUDIO_CHECKBOX_SHUFFLE_WIDTH)
	{
		if (y >= AUDIO_CHECKBOX_SHUFFLE_TOP && y <= AUDIO_CHECKBOX_SHUFFLE_TOP + AUDIO_CHECKBOX_SHUFFLE_WIDTH)
			return AUDIO_CHECKBOX_SHUFFLE;
		if (y >= AUDIO_CHECKBOX_REPEAT_TRACK_TOP && y <= AUDIO_CHECKBOX_REPEAT_TRACK_TOP + AUDIO_CHECKBOX_REPEAT_TRACK_WIDTH)
			return AUDIO_CHECKBOX_REPEAT_TRACK;
		if (y >= AUDIO_CHECKBOX_REPEAT_ALL_TOP && y <= AUDIO_CHECKBOX_REPEAT_ALL_TOP + AUDIO_CHECKBOX_REPEAT_ALL_WIDTH)
			return AUDIO_CHECKBOX_REPEAT_ALL;
		if (y >= AUDIO_CHECKBOX_DISPLAY_CONTROL_TOP && y <= AUDIO_CHECKBOX_DISPLAY_CONTROL_TOP + AUDIO_CHECKBOX_DISPLAY_CONTROL_WIDTH)
			return AUDIO_CHECKBOX_DISPLAY_CONTROL;
		if (y >= AUDIO_CHECKBOX_DISPLAY_INFO_TOP && y <= AUDIO_CHECKBOX_DISPLAY_INFO_TOP + AUDIO_CHECKBOX_DISPLAY_INFO_WIDTH)
			return AUDIO_CHECKBOX_DISPLAY_INFO;
		if (y >= AUDIO_CHECKBOX_DISPLAY_PLAYLIST_TOP && y <= AUDIO_CHECKBOX_DISPLAY_PLAYLIST_TOP + AUDIO_CHECKBOX_DISPLAY_PLAYLIST_WIDTH)
			return AUDIO_CHECKBOX_DISPLAY_PLAYLIST;
		if (y >= AUDIO_CHECKBOX_ENABLE_REMOTE_TOP && y <= AUDIO_CHECKBOX_ENABLE_REMOTE_TOP + AUDIO_CHECKBOX_ENABLE_REMOTE_WIDTH)
			return AUDIO_CHECKBOX_ENABLE_REMOTE;
	}

	if (x >= AUDIO_ADD_LEFT && x <= AUDIO_ADD_LEFT + AUDIO_ADD_WIDTH &&
		y >= AUDIO_ADD_TOP && y <= AUDIO_ADD_TOP + AUDIO_ADD_HEIGHT)
		return AUDIO_ADD;
	if (x >= AUDIO_DEL_LEFT && x <= AUDIO_DEL_LEFT + AUDIO_DEL_WIDTH &&
		y >= AUDIO_DEL_TOP && y <= AUDIO_DEL_TOP + AUDIO_DEL_HEIGHT)
		return AUDIO_DEL;
	if (x >= AUDIO_NEW_LEFT && x <= AUDIO_NEW_LEFT + AUDIO_NEW_WIDTH &&
		y >= AUDIO_NEW_TOP && y <= AUDIO_NEW_TOP + AUDIO_NEW_HEIGHT)
		return AUDIO_NEW;


	return -1;
}

void psix_audio_cursor_moved()
{
	int b_state;
	if ((b_state = psix_audio_mouse_on_object()) != -1)
		psix_audio_set_state(b_state);
}

void psix_audio_toggle_checkbox(int box)
{
	if (box >= AUDIO_NUM_CHECKBOXES)
		return;

	switch (psix_audio_checkboxes[box])
	{
	case CHECKBOX_OFF: 
		psix_audio_set_checkbox(box,CHECKBOX_ON); break;
	case CHECKBOX_OFF_SELECTED:
		psix_audio_set_checkbox(box,CHECKBOX_ON_SELECTED); break;
	case CHECKBOX_ON: 
		psix_audio_set_checkbox(box,CHECKBOX_OFF); break;
	case CHECKBOX_ON_SELECTED:
		psix_audio_set_checkbox(box,CHECKBOX_OFF_SELECTED); break;  
	}
}

void psix_audio_set_checkbox(int box, int value)
{
	if (box >= AUDIO_NUM_CHECKBOXES)
		return;

	if (value == CHECKBOX_ON || value == CHECKBOX_OFF || value == CHECKBOX_ON_SELECTED || value == CHECKBOX_OFF_SELECTED)
		psix_audio_checkboxes[box] = value;

	if (box == AUDIO_CHECKBOX_DISPLAY_CONTROL)
	  {
	    if (value == CHECKBOX_ON || value == CHECKBOX_ON_SELECTED)
	      psix_avs_enable();
	    else
	      psix_avs_disable();
	  }

	if (box == AUDIO_CHECKBOX_DISPLAY_INFO)
	  {
	    if (value == CHECKBOX_ON || value == CHECKBOX_ON_SELECTED)
	      psix_info_window_show();
	    else
	      psix_info_window_hide();
	  }

	psix_audio_update_checkboxes();
}


void psix_audio_add_selection_to_playlist()
{
	if (psix_audio_files[psix_audio_file_line-1]->type != AUDIO_MP3)
	{
		psix_audio_increment_file_line();
		return;
	}

	psix_audio_add_playlist(psix_audio_files[psix_audio_file_line-1]->name,psix_audio_files[psix_audio_file_line-1]->path,AUDIO_MP3);
	psix_audio_draw_playlist();
	psix_audio_increment_file_line();

	if (psix_audio_mouse_on_object() != AUDIO_ADD)
		psix_audio_set_state(AUDIO_FILE);  
}

void psix_audio_increment_file_line()
{
	psix_audio_file_line++;
	if (psix_audio_file_line >= psix_audio_file_num)
		psix_audio_file_line = psix_audio_file_num;

	if (psix_audio_file_line > psix_audio_file_line_max)
	{
		if (psix_audio_file_line_start + psix_audio_file_line_max -1 < psix_audio_file_num)
		{
			psix_audio_file_line_start++;
			sdi_clear("audio_files");
		}
	}
	psix_audio_draw_filelist();
}

void psix_audio_decrement_file_line()
{
	psix_audio_file_line--;
	if (psix_audio_file_line < 1)
		psix_audio_file_line = 1;

	if (psix_audio_file_line < psix_audio_file_line_start)
	{
		if (psix_audio_file_line_start > 1)
			psix_audio_file_line_start--;
		sdi_clear("audio_files");
	}
	psix_audio_draw_filelist();
}

void psix_audio_increment_playlist_line()
{
	psix_audio_playlist_line++;
	if (psix_audio_playlist_line >= psix_audio_playlist_num)
		psix_audio_playlist_line = psix_audio_playlist_num;

	if (psix_audio_playlist_line > psix_audio_playlist_line_max)
	{
		if (psix_audio_playlist_line_start + psix_audio_playlist_line_max -1 < psix_audio_playlist_num)
		{
			psix_audio_playlist_line_start++;
			sdi_clear("audio_playlist");
		}
	}
	psix_audio_draw_playlist();
}

void psix_audio_decrement_playlist_line()
{
	psix_audio_playlist_line--;
	if (psix_audio_playlist_line < 1)
		psix_audio_playlist_line = 1;

	if (psix_audio_playlist_line < psix_audio_playlist_line_start)
	{
		if (psix_audio_playlist_line_start > 1)
			psix_audio_playlist_line_start--;
		sdi_clear("audio_playlist");
	}
	psix_audio_draw_playlist();
}

void psix_audio_increment_play_index()
{
	psix_audio_play_index++;
	switch (psix_audio_play_from)
	{
	case AUDIO_FILE:
		if (psix_audio_play_index >= psix_audio_file_num)
			psix_audio_play_index = psix_audio_file_num-1;
		break;
	case AUDIO_PLAYLIST:
		if (psix_audio_play_index >= psix_audio_playlist_num)
			psix_audio_play_index = psix_audio_playlist_num-1;
		break;
	}
}

int psix_audio_checkbox_enabled(int box)
{
	return (psix_audio_checkboxes[box] == CHECKBOX_ON || psix_audio_checkboxes[box] == CHECKBOX_ON_SELECTED);
}

void psix_audio_play_next()
{
	if (psix_audio_checkbox_enabled(AUDIO_CHECKBOX_SHUFFLE))
	{
		psix_audio_shuffle_index++;
		if (psix_audio_shuffle_index < psix_audio_internal_playlist_num)
		{
			psix_audio_play_file(psix_audio_internal_playlist[psix_audio_shuffle_order[psix_audio_shuffle_index]-1]->name,psix_audio_internal_playlist[psix_audio_shuffle_order[psix_audio_shuffle_index]-1]->path);
		}
		else
		{
			psix_audio_shuffle_index = 0;
			if (psix_audio_checkbox_enabled(AUDIO_CHECKBOX_REPEAT_ALL))
			{
				psix_audio_shuffle_index = -1;
				psix_audio_play_next();
			}
			else
				psix_audio_stop();
		}
		return;
	}

	psix_audio_internal_playlist_line++;

	if (psix_audio_internal_playlist_line > psix_audio_internal_playlist_num) 
	{
		if (psix_audio_checkbox_enabled(AUDIO_CHECKBOX_REPEAT_ALL))
		{
			psix_audio_internal_playlist_line = 0;
			psix_audio_play_next();
		}
		else
		{
			psix_audio_internal_playlist_line = psix_audio_internal_playlist_num;
			psix_audio_stop();
		}
		return;
	}
	if (psix_audio_internal_playlist[psix_audio_internal_playlist_line-1]->type == AUDIO_MP3)
		psix_audio_play_file(psix_audio_internal_playlist[psix_audio_internal_playlist_line-1]->name,psix_audio_internal_playlist[psix_audio_internal_playlist_line-1]->path);
}

void psix_audio_play_prev()
{
	/* play the next in the shuffle list if shuffle is on */
	if (psix_audio_checkbox_enabled(AUDIO_CHECKBOX_SHUFFLE))
	{
		psix_audio_shuffle_index--;
		if (psix_audio_shuffle_index >= 0)
		{
			psix_audio_play_file(psix_audio_internal_playlist[psix_audio_shuffle_order[psix_audio_shuffle_index]-1]->name,psix_audio_internal_playlist[psix_audio_shuffle_order[psix_audio_shuffle_index]-1]->path);
		}
		else
		{
			psix_audio_shuffle_index = 0;
			psix_audio_stop();
		}
		return;
	}

	/* just play the next in the list */
	psix_audio_internal_playlist_line--;
	if (psix_audio_internal_playlist_line <= 0) 
	{
		psix_audio_internal_playlist_line = 1;
		psix_audio_stop();
		return;
	}
	if (psix_audio_internal_playlist[psix_audio_internal_playlist_line-1]->type == AUDIO_MP3)
		psix_audio_play_file(psix_audio_internal_playlist[psix_audio_internal_playlist_line-1]->name,psix_audio_internal_playlist[psix_audio_internal_playlist_line-1]->path);

}

void psix_audio_play_file(char *name, char *path)
{
	if (!psix_audio_module_loaded) return;

	MP3_End();

	if (psix_audio_playing)
	{
		MP3_End();
		psix_audio_playing = 0;
		sceKernelDelayThread(300000);
	}


	SceIoStat stat;
	sceIoGetstat(path,&stat);
	
	MP3_Init(0);
	//if (stat.st_size <= 7*1024*1024)
	//{
	    
	    if (MP3_Load(path))
	      {
		//psix_draw_stop = 1;
		MP3_Play();
		
		psix_audio_play_track_frame = 0;
		psix_audio_set_track_title(name);
		psix_audio_update_info_window_title(name);
		psix_audio_playing = 1;
		
		//scePowerSetClockFrequency(266, 266, 133);
		scePowerSetClockFrequency(333, 333, 166);
	      }
	    //     }
	else
	  psix_audio_play_next();
	
}

void psix_audio_pause()
{
	if (psix_audio_paused)
	{
		psix_audio_paused = 0;
		scePowerSetClockFrequency(333, 333, 166);
	}	
	else
	{    
		psix_audio_paused = 1;
		scePowerSetClockFrequency(222, 222, 111);
		//scePowerSetClockFrequency(266, 266, 133);
	}
	MP3_Pause();
}

void psix_audio_stop()
{
  MP3_End();
  sceKernelDelayThread(100000);
  MP3_End();

  if (psix_audio_playing)
    {
      scePowerSetClockFrequency(222, 222, 111);
      //scePowerSetClockFrequency(266, 266, 133);
      psix_audio_playing = 0;
    }
  sdi_clear("audio_play_time");
  sdi_clear("audio_play_track");
  
  if (psix_info_window_visible)
    psix_info_window_hide();
}

void psix_audio_update_checkboxes()
{
	int i;
	for (i=0; i < AUDIO_NUM_CHECKBOXES; i++)
	{
		char name[50];
		switch (i)
		{
		case AUDIO_CHECKBOX_SHUFFLE: 
			sprintf(name,"audio_checkbox_shuffle"); break;
		case AUDIO_CHECKBOX_REPEAT_TRACK: 
			sprintf(name,"audio_checkbox_repeat_track"); break;
		case AUDIO_CHECKBOX_REPEAT_ALL: 
			sprintf(name,"audio_checkbox_repeat_all"); break;
		case AUDIO_CHECKBOX_DISPLAY_CONTROL: 
			sprintf(name,"audio_checkbox_display_control"); break;
		case AUDIO_CHECKBOX_DISPLAY_INFO: 
			sprintf(name,"audio_checkbox_display_info"); break;
		case AUDIO_CHECKBOX_DISPLAY_PLAYLIST: 
			sprintf(name,"audio_checkbox_display_playlist"); break;
		case AUDIO_CHECKBOX_ENABLE_REMOTE: 
			sprintf(name,"audio_checkbox_enable_remote"); break;
		}

		switch(psix_audio_checkboxes[i])
		{
		case CHECKBOX_OFF:
			sdi_copy_image(name,"checkbox_off"); break;
		case CHECKBOX_OFF_SELECTED:
			sdi_copy_image(name,"checkbox_off_selected"); break;
		case CHECKBOX_ON:
			sdi_copy_image(name,"checkbox_on"); break;
		case CHECKBOX_ON_SELECTED:
			sdi_copy_image(name,"checkbox_on_selected"); break;
		}
	}
}

void psix_audio_set_state(int state)
{
	if (state == psix_audio_state) return;

	psix_audio_state = state;
	sdi_set_enabled("audio_add_on",state == AUDIO_ADD);
	sdi_set_enabled("audio_del_on",state == AUDIO_DEL);
	sdi_set_enabled("audio_prev_on",state == AUDIO_PREV);
	sdi_set_enabled("audio_next_on",state == AUDIO_NEXT);
	sdi_set_enabled("audio_play_on",state == AUDIO_PLAY);
	sdi_set_enabled("audio_pause_on",state == AUDIO_PAUSE);
	sdi_set_enabled("audio_stop_on",state == AUDIO_STOP);
	sdi_set_enabled("audio_new_on",state == AUDIO_NEW);

	int i;
	for (i=0; i < AUDIO_NUM_CHECKBOXES; i++)
	{
		if (i == state)
			switch (psix_audio_checkboxes[i])
		{
			case CHECKBOX_ON: 
				psix_audio_checkboxes[i] = CHECKBOX_ON_SELECTED; break;
			case CHECKBOX_OFF:  
				psix_audio_checkboxes[i] = CHECKBOX_OFF_SELECTED; break;
		}
		else
		{
			switch (psix_audio_checkboxes[i])
			{
			case CHECKBOX_ON_SELECTED: 
				psix_audio_checkboxes[i] = CHECKBOX_ON; break;
			case CHECKBOX_OFF_SELECTED:  
				psix_audio_checkboxes[i] = CHECKBOX_OFF; break;
			}
		}
	}

	psix_audio_update_checkboxes();
	psix_audio_draw_filelist();
	psix_audio_draw_playlist();
}

void psix_audio_update_info_window_time(char *time)
{
	char buf[256];

	sprintf(buf,"Playing: (%s)",time);
	sdi_print_line_max("info_window_text","font_info_window",buf,178,1);  
}

void psix_audio_update_info_window_title(char *title)
{
	sdi_clear("info_window_text");
	sdi_print_line_max("info_window_text","font_info_window",title,178,2);
}

void psix_audio_update_title()
{
	psix_audio_play_track_frame += psix_audio_play_track_direction?1:-1;

	if (psix_audio_play_track_frame + AUDIO_PLAY_TRACK_WIDTH > psix_audio_play_track_width)
		psix_audio_play_track_direction = !psix_audio_play_track_direction;

	if (psix_audio_play_track_frame < 0)
		psix_audio_play_track_direction = !psix_audio_play_track_direction;

	if (psix_audio_play_track_width < AUDIO_PLAY_TRACK_WIDTH)
		psix_audio_play_track_frame = 0;

	sdi_copy_part("audio_play_track","audio_play_track_full",psix_audio_play_track_frame,0,psix_audio_play_track_frame+AUDIO_PLAY_TRACK_WIDTH,11);  

}

void psix_info_window_show()
{
	sdi_set_enabled("info_window",1);
	sdi_set_overlay("info_window",1);
	sdi_set_enabled("info_window_text",1);
	psix_info_window_visible = 1;
}

void psix_info_window_hide()
{
	sdi_set_enabled("info_window",0);
	sdi_set_enabled("info_window_text",0);
	psix_info_window_visible = 0;
}

int psix_audio_thread()
{
  while(!psix_done)
    {
      if (psix_audio_playing)// && psix_bottab_state == 1)
	{
	  if (!psix_info_window_visible)
	    if (psix_audio_checkbox_enabled(AUDIO_CHECKBOX_DISPLAY_INFO))
	      psix_info_window_show();
	  
	  char time[50];
	  MP3_GetTimeString(time);
	  psix_audio_set_track_time(time);
	  psix_audio_update_info_window_time(time);
	  psix_audio_update_title();
	  
	  if (psix_audio_paused)
	    {
	      psix_audio_paused_count++;
	      if (psix_audio_paused_count > 15)
		sdi_clear("audio_play_time");
	      if (psix_audio_paused_count > 30)
		psix_audio_paused_count = 0;
	    }
	}
      sceKernelDelayThread(50000);
      
      /* see if the song is over */
      if (MP3_EndOfStream())
	{
	  if (psix_audio_checkbox_enabled(AUDIO_CHECKBOX_REPEAT_TRACK))
	    {
	      psix_audio_play_file(psix_audio_internal_playlist[psix_audio_internal_playlist_line-1]->name,psix_audio_internal_playlist[psix_audio_internal_playlist_line-1]->path);
	    }
	  else
	    psix_audio_play_next();
	}
    }
  return 0;
}

SceUID psix_thread_audio;
void psix_audio_start_thread()
{
	psix_thread_audio = sceKernelCreateThread("PSIX Audio Thread", psix_audio_thread,0x10, 32 * 1024, THREAD_ATTR_USER, NULL);  
	sceKernelStartThread(psix_thread_audio, 0, NULL);
}

void psix_audio_load_module()
{
	SceIoStat stat;
	if (sceIoGetstat("ms0:/psp/game/psix/modules/audio.prx",&stat) < 0)
		return;

	SceKernelLMOption option;
	SceUID mpid = 2;
	memset(&option, 0, sizeof(option));
	option.size = sizeof(option);
	option.mpidtext = mpid;
	option.mpiddata = mpid;
	option.position = 0;
	option.access = 1;

	SceUID modid = sceKernelLoadModule("ms0:/psp/game/psix/modules/audio.prx", 0, &option);
	if (!modid)
		psix_audio_module_loaded = 0;
	else
	{
		int fd;
		psix_audio_module_loaded = 1;
		sceKernelStartModule(modid, 0, NULL, &fd, NULL);
	}

	pspSdkFixupImports(modid); 
}

SceUID psix_thread_audio_avs;

int psix_audio_avs_thread()
{
  int i;
  while(!psix_done)
    {
      MP3_Spectrum(psix_audio_vis_value,AUDIO_VIS_NUM,16);

      psix_audio_vis_update();
      //sceDisplayWaitVblankStart();
      sceDisplayWaitVblankStart();
    }
}

void psix_audio_vis_init()
{
  /* create all the objects for the bars */
  char sdi_name[50];
  int i;

  for (i=0; i < AUDIO_VIS_NUM; i++)
    {
      sprintf(sdi_name,"vis_bar_%d",i);
      sdi_new(sdi_name);
      sdi_set_position(sdi_name,AUDIO_VIS_LEFT+i*(AUDIO_VIS_WIDTH+AUDIO_VIS_SPACE),AUDIO_VIS_TOP);
      sdi_set_overlay(sdi_name,1);
      sdi_move_below(sdi_name,"page_file");
      sdi_copy_image(sdi_name,"audio_vis_0");     
    }

  /* initialize test animation thread */
  psix_thread_audio_avs = sceKernelCreateThread("PSIX Audio AVS Thread", psix_audio_avs_thread,0x10, 32 * 1024, THREAD_ATTR_USER, NULL);  
  sceKernelStartThread(psix_thread_audio_avs, 0, NULL);
}

void psix_audio_vis_update()
{
  int i;
  int max=0;

  /* make sure nothing goes over 16 */
  for (i=0; i < AUDIO_VIS_NUM; i++)
    {
      psix_audio_vis_value[i] = (float)psix_audio_vis_value[i] * (float)8
 / (float)5; 
      if (psix_audio_vis_value[i] > 16)
	psix_audio_vis_value[i] = 16;
    }

  /* set each image based on the value */
  for (i=0; i < AUDIO_VIS_NUM; i++)
    {
      char sdi_name[50];
      char sdi_name2[50];
      sprintf(sdi_name,"vis_bar_%d",i);
      sprintf(sdi_name2,"audio_vis_%d",psix_audio_vis_value[i]);
      sdi_copy_image(sdi_name,sdi_name2);
    }
}
