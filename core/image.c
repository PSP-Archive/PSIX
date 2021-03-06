#include "psix.h"

#define IMAGE_MAX_FILES 1000
#define IMAGE_FILES_LEFT 20
#define IMAGE_FILES_TOP 54
#define IMAGE_FILES_WIDTH 155
#define IMAGE_FILES_HEIGHT 226-54
#define IMAGE_CWD_LEFT 10
#define IMAGE_CWD_TOP 39
#define IMAGE_CWD_WIDTH 165
#define IMAGE_CWD_HEIGHT 10
#define IMAGE_FILE_LINE_MAX 13
#define IMAGE_DELETE_LEFT 252
#define IMAGE_DELETE_TOP 215
#define IMAGE_DELETE_WIDTH 67
#define IMAGE_DELETE_HEIGHT 15
#define IMAGE_PSIX_INTERFACE_LEFT 182
#define IMAGE_PSIX_INTERFACE_TOP 215
#define IMAGE_PSIX_INTERFACE_WIDTH 67
#define IMAGE_PSIX_INTERFACE_HEIGHT 15
#define IMAGE_REMOVE_BG_LEFT 251
#define IMAGE_REMOVE_BG_TOP 197
#define IMAGE_REMOVE_BG_WIDTH 67
#define IMAGE_REMOVE_BG_HEIGHT 15
#define IMAGE_SET_BG_LEFT 182
#define IMAGE_SET_BG_TOP 197
#define IMAGE_SET_BG_WIDTH 67
#define IMAGE_SET_BG_HEIGHT 15
#define IMAGE_PREVIOUS_LEFT 344
#define IMAGE_PREVIOUS_TOP 202
#define IMAGE_PREVIOUS_WIDTH 53
#define IMAGE_PREVIOUS_HEIGHT 15
#define IMAGE_FULLSCREEN_LEFT 398
#define IMAGE_FULLSCREEN_TOP 202
#define IMAGE_FULLSCREEN_WIDTH 16
#define IMAGE_FULLSCREEN_HEIGHT 15
#define IMAGE_NEXT_LEFT 415
#define IMAGE_NEXT_TOP 202
#define IMAGE_NEXT_WIDTH 53
#define IMAGE_NEXT_HEIGHT 15
#define IMAGE_PREVIEW_LEFT 184
#define IMAGE_PREVIEW_TOP 39
#define IMAGE_PREVIEW_WIDTH 471-184
#define IMAGE_PREVIEW_HEIGHT 193-39


enum psix_image_types {IMAGE_DIR, IMAGE_JPG, IMAGE_PNG};

enum psix_audio_states {IMAGE_FILE=10,IMAGE_SET_BG,IMAGE_REMOVE_BG,
IMAGE_PSIX_INTERFACE, IMAGE_DELETE,
IMAGE_NEXT, IMAGE_PREVIOUS,
IMAGE_FULLSCREEN};

struct psix_image_file_info {
	char path[1024];
	char name[100];
	int type;
};

struct psix_image_file_info *psix_image_files[IMAGE_MAX_FILES];

int psix_image_state;
int psix_image_file_num;
int psix_image_file_line;
int psix_image_file_line_start;
int psix_image_preview_width;
int psix_image_show_psix_interface;
int psix_image_fullscreen;
int psix_image_background_set;


void psix_image_init()
{
	psix_image_set_state(IMAGE_FILE);

	int i;
	for (i=0; i < IMAGE_MAX_FILES; i++)
		psix_image_files[i] = 0;

	sdi_new("image_files");
	sdi_set_position("image_files",-480+IMAGE_FILES_LEFT,IMAGE_FILES_TOP);
	sdi_set_overlay("image_files",1);
	sdi_set_enabled("image_files",0);
	sdi_create_image("image_files",175-20,226-54);

	sdi_new("image_cwd");
	sdi_set_position("image_cwd",-480+IMAGE_CWD_LEFT,IMAGE_CWD_TOP);
	sdi_set_overlay("image_cwd",1);
	sdi_set_enabled("image_cwd",0);
	sdi_create_image("image_cwd",IMAGE_CWD_WIDTH,IMAGE_CWD_HEIGHT);

	sdi_new("image_preview");
	sdi_set_position("image_preview",-480+IMAGE_PREVIEW_LEFT,IMAGE_PREVIEW_TOP);
	sdi_set_overlay("image_preview",1);
	sdi_set_enabled("image_preview",0);
	sdi_create_image("image_preview",IMAGE_PREVIEW_WIDTH,IMAGE_PREVIEW_HEIGHT);
	sdi_move_below("image_preview","image_psix_overlay");
	sdi_set_display_width("image_preview",IMAGE_PREVIEW_WIDTH);
	sdi_set_display_height("image_preview",IMAGE_PREVIEW_HEIGHT);
	sdi_set_scale("image_preview",1);

	sdi_set_display_width("image_psix_overlay",IMAGE_PREVIEW_WIDTH-16);
	sdi_set_display_height("image_psix_overlay",IMAGE_PREVIEW_HEIGHT);
	sdi_set_scale("image_psix_overlay",1);
	sdi_set_x("image_psix_overlay",IMAGE_PREVIEW_LEFT+(IMAGE_PREVIEW_WIDTH-(IMAGE_PREVIEW_WIDTH-16))/2);
	sdi_set_y("image_psix_overlay",IMAGE_PREVIEW_TOP);


	psix_image_file_num = 0;
	psix_image_file_line = 1;
	psix_image_file_line_start = 1;
	psix_image_preview_width = 0;
	psix_image_show_psix_interface = 0;
	psix_image_fullscreen = 0;
	psix_image_background_set = 0;

	psix_image_clear_filelist();

	if (psix_image_scan("ms0:/psp/photo") == -1)
	{
		psix_audio_scan("ms0:/");
		sdi_print_line_max_compress("image_cwd","font_black","ms0:/",IMAGE_CWD_WIDTH,1,1);
	}
	else
		sdi_print_line_max_compress("image_cwd","font_black","ms0:/psp/photo/",IMAGE_CWD_WIDTH,1,1);

	psix_image_draw_filelist();

	psix_image_get_x_pointers();
}

void psix_image_keypress(u32 pad)
{
	if (pad & PSP_CTRL_DOWN)
	{
		switch (psix_image_state)
		{
		case IMAGE_FILE:
			psix_image_increment_file_line();
			psix_image_show_file_preview();
			break;
		case IMAGE_SET_BG:
			psix_image_set_state(IMAGE_PSIX_INTERFACE); break;
		case IMAGE_REMOVE_BG:
			psix_image_set_state(IMAGE_DELETE); break;
		}
	}
	if (pad & PSP_CTRL_UP)
	{
		switch (psix_image_state)
		{
		case IMAGE_FILE:
			psix_image_decrement_file_line();
			psix_image_show_file_preview();
			break;
		case IMAGE_PSIX_INTERFACE:
			psix_image_set_state(IMAGE_SET_BG); break;
		case IMAGE_DELETE:
			psix_image_set_state(IMAGE_REMOVE_BG); break;
		}
	}
	if (pad & PSP_CTRL_RIGHT)
	{
		if (psix_image_fullscreen)
		{
			psix_image_increment_file_line();
			psix_image_show_file_preview(); 
		}
		else
			switch (psix_image_state)
		{
			case IMAGE_FILE:
				psix_image_set_state(IMAGE_SET_BG); break;
			case IMAGE_SET_BG:
				psix_image_set_state(IMAGE_REMOVE_BG); break;
			case IMAGE_REMOVE_BG:
				psix_image_set_state(IMAGE_PREVIOUS); break;
			case IMAGE_FULLSCREEN:
				psix_image_set_state(IMAGE_NEXT); break;
			case IMAGE_NEXT:
				psix_image_set_state(IMAGE_FILE); break;
			case IMAGE_PSIX_INTERFACE:
				psix_image_set_state(IMAGE_DELETE); break;
			case IMAGE_DELETE:
				psix_image_set_state(IMAGE_PREVIOUS); break;
			case IMAGE_PREVIOUS:
				psix_image_set_state(IMAGE_FULLSCREEN); break;
		}
	}
	if (pad & PSP_CTRL_LEFT)
	{
		if (psix_image_fullscreen)
		{
			psix_image_decrement_file_line();
			psix_image_show_file_preview(); 
		}
		else
			switch (psix_image_state)
		{
			case IMAGE_FILE:
				psix_image_set_state(IMAGE_NEXT); break;
			case IMAGE_NEXT:
				psix_image_set_state(IMAGE_FULLSCREEN); break;
			case IMAGE_FULLSCREEN:
				psix_image_set_state(IMAGE_PREVIOUS); break;
			case IMAGE_PREVIOUS:
				psix_image_set_state(IMAGE_REMOVE_BG); break;
			case IMAGE_REMOVE_BG:
				psix_image_set_state(IMAGE_SET_BG); break;
			case IMAGE_SET_BG:
				psix_image_set_state(IMAGE_FILE); break;
			case IMAGE_DELETE:
				psix_image_set_state(IMAGE_PSIX_INTERFACE); break;
			case IMAGE_PSIX_INTERFACE:
				psix_image_set_state(IMAGE_FILE); break;
		}
	}
	if (pad & PSP_CTRL_CROSS)
	{
		if (psix_image_fullscreen)
		{
			psix_image_toggle_fullscreen();
			psix_image_set_preview_size();
		}
		else
			switch (psix_image_state)
		{
			case IMAGE_SET_BG: psix_image_set_background(); break;
			case IMAGE_REMOVE_BG: psix_image_remove_background(); break;
			case IMAGE_PSIX_INTERFACE: psix_image_toggle_psix_interface(); break;
			case IMAGE_PREVIOUS: 
				psix_image_decrement_file_line();
				psix_image_show_file_preview();
				break;
			case IMAGE_NEXT: 
				psix_image_increment_file_line();
				psix_image_show_file_preview(); 
				break;
			case IMAGE_FULLSCREEN:
				psix_image_toggle_fullscreen();
				psix_image_set_preview_size();
				break;
			case IMAGE_FILE:
				psix_image_file_select();
				break;
		}
	}
}

void psix_image_file_select()
{
	if (psix_image_files[psix_image_file_line-1]->type == IMAGE_DIR)
	{
		sdi_clear("image_cwd");
		sdi_print_line_max_compress("image_cwd","font_black",psix_image_files[psix_image_file_line-1]->path,IMAGE_FILES_WIDTH,1,1);     
		char path[1024];
		strcpy(path,psix_image_files[psix_image_file_line-1]->path);

		psix_image_clear_filelist();
		psix_image_scan(path);
		psix_image_draw_filelist();
	}
	else
		if ((psix_image_files[psix_image_file_line-1]->type == IMAGE_JPG ||
			psix_image_files[psix_image_file_line-1]->type == IMAGE_PNG ) &&
			!psix_image_fullscreen)
		{
			psix_image_toggle_fullscreen();
			psix_image_set_preview_size();
		}
}

void psix_image_toggle_fullscreen()
{
	if (psix_image_fullscreen)
		psix_image_set_fullscreen_off();
	else
		psix_image_set_fullscreen_on();

	psix_image_fullscreen = !psix_image_fullscreen;
}

void psix_image_set_fullscreen_on()
{
	int *img = malloc(4*4*4);
	if (!img) return;
	int i,j;
	for (i=0; i < 4*4; i++)
		//for (j=0; j < 2; j++)
		img[i] = 0xFF000000;

	sdi_new("image_black");
	sdi_set_position("image_black",0,0);
	sdi_set_image("image_black",img,4,4,4,4);
	sdi_set_display_width("image_black",480);
	sdi_set_display_height("image_black",272);
	sdi_set_scale("image_black",1);
	sdi_set_enabled("image_black",1);
	sdi_set_overlay("image_black",1);

	sdi_move_top("image_preview");

	free(img);
}


void psix_image_set_fullscreen_off()
{
	sdi_del("image_black");
	sdi_move_below("image_preview","image_psix_overlay");
}


void psix_image_toggle_psix_interface()
{
	psix_image_show_psix_interface = !psix_image_show_psix_interface;
	sdi_set_enabled("image_psix_overlay",psix_image_show_psix_interface);
}

void psix_image_remove_background()
{
	sdi_load_from_state("wall","ms0:/psp/game/psix/psix.theme");

	/*
	if (psix_image_background_set)
	{
	psix_image_background_set = 0; 
	sdi_swap_image("wall","new_wall");
	sdi_del("new_wall");
	}
	*/
}

void psix_image_set_background()
{
	/* dont save the old back */
	/*  if (!psix_image_background_set)
	{
	sdi_new("new_wall");
	if (psix_image_files[psix_image_file_line-1]->type != IMAGE_DIR)
	if (sdi_load_image("new_wall",psix_image_files[psix_image_file_line-1]->path))
	{
	sdi_swap_image("wall","new_wall");
	sdi_del("new_wall");
	}
	}

	if (!psix_image_background_set)
	{
	sdi_new("new_wall");
	if (psix_image_files[psix_image_file_line-1]->type != IMAGE_DIR)
	if (sdi_load_image("new_wall",psix_image_files[psix_image_file_line-1]->path))
	{
	sdi_swap_image("wall","new_wall");
	psix_image_background_set = 1;
	}
	}
	else
	{
	if (psix_image_files[psix_image_file_line-1]->type != IMAGE_DIR)
	if (!sdi_load_image("wall",psix_image_files[psix_image_file_line-1]->path))
	{
	sdi_swap_image("wall","new_wall");
	sdi_del("new_wall");
	psix_image_background_set = 0;
	}
	}*/

	sdi_new("new_wall");
	if (psix_image_files[psix_image_file_line-1]->type != IMAGE_DIR)
		if (sdi_load_image("new_wall",psix_image_files[psix_image_file_line-1]->path))
			sdi_swap_image("wall","new_wall");

	sdi_del("new_wall");
	sdi_set_display_width("wall",480);
	sdi_set_display_height("wall",272);
	sdi_set_scale("wall",1);
}

void psix_image_set_preview_size()
{
	int left,top,width,height;

	if (psix_image_fullscreen)
	{
		left = 0;
		top = 0;
		width = 480;
		height = 272;
	}
	else
	{
		left = IMAGE_PREVIEW_LEFT;
		top = IMAGE_PREVIEW_TOP;
		width = IMAGE_PREVIEW_WIDTH;
		height = IMAGE_PREVIEW_HEIGHT;
	}

	int w = sdi_get_width("image_preview");
	int h = sdi_get_height("image_preview");

	if (w > width || h > height)
	{
		float sw = (float)((float)w)/((float)width);
		float sh = (float)((float)h)/((float)height);
		float s = sw;

		if (sw > 1.0f && sh > 1.0f && sw < sh)
			s = sh;
		else
			if (sw <= 1.0f)
				s = sh;

		w = (int)((float)w/s);
		h = (int)((float)h/s);

		sdi_set_display_width("image_preview",w);
		sdi_set_display_height("image_preview",h);
		sdi_set_scale("image_preview",1);


	}
	else
		sdi_set_scale("image_preview",0);

	sdi_set_x("image_preview",left+(width-w)/2);
	sdi_set_y("image_preview",top+(height-h)/2);
	psix_image_preview_width = w;
}


void psix_image_show_file_preview()
{
	if (psix_image_files[psix_image_file_line-1]->type != IMAGE_DIR)
	{
		if (sdi_load_image("image_preview",psix_image_files[psix_image_file_line-1]->path) == 0)
			sdi_clear("image_preview");
		else
			psix_image_set_preview_size();
	}
	else
		sdi_clear("image_preview");
}

void psix_image_set_state(int state)
{
	psix_image_state = state;
	sdi_set_enabled("image_set_bg_on",state == IMAGE_SET_BG);
	sdi_set_enabled("image_previous_on",state == IMAGE_PREVIOUS);
	sdi_set_enabled("image_next_on",state == IMAGE_NEXT);
	sdi_set_enabled("image_fullscreen_on",state == IMAGE_FULLSCREEN);
	sdi_set_enabled("image_delete_on",state == IMAGE_DELETE);
	sdi_set_enabled("image_remove_bg_on",state == IMAGE_REMOVE_BG);
	sdi_set_enabled("image_psix_interface_on",state == IMAGE_PSIX_INTERFACE || psix_image_show_psix_interface);
	psix_image_draw_filelist();
}

void psix_image_increment_file_line()
{
	psix_image_file_line++;
	if (psix_image_file_line >= psix_image_file_num)
		psix_image_file_line = psix_image_file_num;

	if (psix_image_file_line > IMAGE_FILE_LINE_MAX)
	{
		if (psix_image_file_line_start + IMAGE_FILE_LINE_MAX -1 < psix_image_file_num)
		{
			psix_image_file_line_start++;
			sdi_clear("image_files");
		}
	}
	psix_image_draw_filelist();
}

void psix_image_decrement_file_line()
{
	psix_image_file_line--;
	if (psix_image_file_line < 1)
		psix_image_file_line = 1;

	if (psix_image_file_line < psix_image_file_line_start)
	{
		if (psix_image_file_line_start > 1)
			psix_image_file_line_start--;
		sdi_clear("image_files");
	}
	psix_image_draw_filelist();
}

void psix_image_transition_in()
{
	sdi_set_enabled("page_image",1);
	sdi_set_enabled("image_cwd",1);
	sdi_set_enabled("image_files",1);
	sdi_set_enabled("image_preview",1);
}

void psix_image_transition_out()
{
	sdi_set_enabled("page_image",0);
	sdi_set_enabled("image_cwd",0);
	sdi_set_enabled("image_files",0);
	sdi_set_enabled("image_preview",0);
}

int *psix_image_x_pointer[12];

void psix_image_get_x_pointers()
{
  psix_image_x_pointer[0] = sdi_get_x_ptr("page_image");
  psix_image_x_pointer[1] = sdi_get_x_ptr("image_files");
  psix_image_x_pointer[2] = sdi_get_x_ptr("image_cwd");
  psix_image_x_pointer[3] = sdi_get_x_ptr("image_psix_overlay");
  psix_image_x_pointer[4] = sdi_get_x_ptr("image_delete_on");
  psix_image_x_pointer[5] = sdi_get_x_ptr("image_psix_interface_on");
  psix_image_x_pointer[6] = sdi_get_x_ptr("image_remove_bg_on");
  psix_image_x_pointer[7] = sdi_get_x_ptr("image_set_bg_on");
  psix_image_x_pointer[8] = sdi_get_x_ptr("image_previous_on");
  psix_image_x_pointer[9] = sdi_get_x_ptr("image_fullscreen_on");
  psix_image_x_pointer[10] = sdi_get_x_ptr("image_next_on");
  psix_image_x_pointer[11] = sdi_get_x_ptr("image_preview");
}

void psix_image_set_left(int x)
{
  *(psix_image_x_pointer[0]) = x;
  *(psix_image_x_pointer[1]) = x+IMAGE_FILES_LEFT;
  *(psix_image_x_pointer[2]) = x+IMAGE_CWD_LEFT;
  *(psix_image_x_pointer[3]) = x+IMAGE_PREVIEW_LEFT+(IMAGE_PREVIEW_WIDTH-(IMAGE_PREVIEW_WIDTH-16))/2;
  *(psix_image_x_pointer[4]) = x+IMAGE_DELETE_LEFT;
  *(psix_image_x_pointer[5]) = x+IMAGE_PSIX_INTERFACE_LEFT;
  *(psix_image_x_pointer[6]) = x+IMAGE_REMOVE_BG_LEFT;
  *(psix_image_x_pointer[7]) = x+IMAGE_SET_BG_LEFT;
  *(psix_image_x_pointer[8]) = x+IMAGE_PREVIOUS_LEFT;
  *(psix_image_x_pointer[9]) = x+IMAGE_FULLSCREEN_LEFT;
  *(psix_image_x_pointer[10]) = x+IMAGE_NEXT_LEFT;
  *(psix_image_x_pointer[11]) = x+IMAGE_PREVIEW_LEFT+(IMAGE_PREVIEW_WIDTH-psix_image_preview_width)/2;
}

void psix_image_clear_filelist()
{
	int i;
	for (i=0; i < psix_image_file_num; i++)
	{
		free(psix_image_files[i]);
		psix_image_files[i] = 0;
	}

	psix_image_file_line = 1;
	psix_image_file_num = 0;
	psix_image_file_line_start = 1;
	sdi_clear("image_files");
}

void psix_image_add_filelist(char *name,char *path,int type)
{
	/* find the first free spot */
	int i;
	for (i=0;i < IMAGE_MAX_FILES; i++)
		if (!psix_image_files[i])
			break;
	if (i == IMAGE_MAX_FILES) return;

	psix_image_file_num = i+1;

	psix_image_files[i] = (struct psix_image_file_info*)malloc(sizeof(struct psix_image_file_info));
	strncpy(psix_image_files[i]->name,name,99);
	strncpy(psix_image_files[i]->path,path,1023);
	(psix_image_files[i]->path)[1023] = 0;
	(psix_image_files[i]->name)[98] = 0;
	psix_image_files[i]->type = type;
}

void psix_image_draw_filelist()
{
	int i=psix_image_file_line_start;

	while (i < psix_image_file_line_start + IMAGE_FILE_LINE_MAX && i <= psix_image_file_num)
	{
		if (i == psix_image_file_line && psix_image_state == IMAGE_FILE)
			sdi_print_line_max_compress("image_files","font_blue",psix_image_files[i-1]->name,136,1,i-psix_image_file_line_start+1);
		else
			sdi_print_line_max_compress("image_files","font_black",psix_image_files[i-1]->name,136,1,i-psix_image_file_line_start+1);

		i++;
	}
}

int psix_image_scan(char *path)
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

				psix_image_add_filelist(buf,buf2,IMAGE_DIR);
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
			sprintf(buf,"%s%s%s",path,(path[strlen(path)-1]=='/')?"":"/",dirent.d_name);
			if (strncasecmp(&dirent.d_name[strlen(dirent.d_name) - 3], "jpg", 3) == 0)
				psix_image_add_filelist(dirent.d_name,buf,IMAGE_JPG);
			else if (strncasecmp(&dirent.d_name[strlen(dirent.d_name) - 3], "png", 3) == 0)
				psix_image_add_filelist(dirent.d_name,buf,IMAGE_PNG);
		}
	}

	sceIoDclose(dd);

	return 0;
}


int psix_image_mouse_on_object()
{
	int x = (int)psix_cursor_x;
	int y = (int)psix_cursor_y;
	/*
	if (x >= IMAGE_FILES_LEFT && x <= IMAGE_FILES_LEFT+IMAGE_FILES_WIDTH &&
	y >= IMAGE_FILES_TOP && y <= IMAGE_FILES_TOP+IMAGE_FILES_HEIGHT)
	{
	int line = (y-IMAGE_FILES_TOP)/12 + psix_image_file_line_start;
	if (line <= psix_image_file_num && psix_image_file_line != line)
	{
	psix_image_file_line = line;
	psix_image_show_file_preview(); 
	psix_image_draw_filelist();
	}
	return IMAGE_FILE;
	}
	*/
	if (x >= IMAGE_SET_BG_LEFT && x <= IMAGE_REMOVE_BG_LEFT+IMAGE_REMOVE_BG_WIDTH
		&& y >= IMAGE_SET_BG_TOP && y <= IMAGE_PSIX_INTERFACE_TOP+IMAGE_PSIX_INTERFACE_HEIGHT)
	{
		if (x >= IMAGE_SET_BG_LEFT && x <= IMAGE_SET_BG_LEFT+IMAGE_SET_BG_WIDTH
			&& y >= IMAGE_SET_BG_TOP && y <= IMAGE_SET_BG_TOP+IMAGE_SET_BG_HEIGHT)
			return IMAGE_SET_BG;
		if (x >= IMAGE_REMOVE_BG_LEFT && x <= IMAGE_REMOVE_BG_LEFT+IMAGE_REMOVE_BG_WIDTH
			&& y >= IMAGE_REMOVE_BG_TOP && y <= IMAGE_REMOVE_BG_TOP+IMAGE_REMOVE_BG_HEIGHT)
			return IMAGE_REMOVE_BG;
		if (x >= IMAGE_SET_BG_LEFT && x <= IMAGE_SET_BG_LEFT+IMAGE_SET_BG_WIDTH
			&& y >= IMAGE_PSIX_INTERFACE_TOP && y <= IMAGE_PSIX_INTERFACE_TOP+IMAGE_PSIX_INTERFACE_HEIGHT)
			return IMAGE_PSIX_INTERFACE;
		if (x >= IMAGE_DELETE_LEFT && x <= IMAGE_DELETE_LEFT+IMAGE_DELETE_WIDTH
			&& y >= IMAGE_DELETE_TOP && y <= IMAGE_DELETE_TOP+IMAGE_DELETE_HEIGHT)
			return IMAGE_DELETE;
	}

	if (x >= IMAGE_PREVIOUS_LEFT && x <= IMAGE_NEXT_LEFT + IMAGE_NEXT_WIDTH &&
		y >= IMAGE_PREVIOUS_TOP && y <= IMAGE_PREVIOUS_TOP+IMAGE_PREVIOUS_HEIGHT)
	{
		if (x >= IMAGE_FULLSCREEN_LEFT && x <= IMAGE_FULLSCREEN_LEFT + IMAGE_FULLSCREEN_WIDTH)
			return IMAGE_FULLSCREEN;

		if (x >= IMAGE_PREVIOUS_LEFT && x <= IMAGE_PREVIOUS_LEFT + IMAGE_PREVIOUS_WIDTH)
			return IMAGE_PREVIOUS;

		if (x >= IMAGE_NEXT_LEFT && x <= IMAGE_NEXT_LEFT + IMAGE_NEXT_WIDTH)
			return IMAGE_NEXT;
	}
	return -1;
}

void psix_image_cursor_moved()
{
	int b_state;
	if ((b_state = psix_image_mouse_on_object()) != -1)
		psix_image_set_state(b_state);
}
