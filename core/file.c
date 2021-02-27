#include "psix.h"

#define FILE_RENAME_LEFT 237
#define FILE_DELETE_LEFT 237
#define FILE_MOVE_LEFT_LEFT 237
#define FILE_MOVE_RIGHT_LEFT 237
#define FILE_ACTIVE_LEFT_LEFT 188
#define FILE_ACTIVE_RIGHT_LEFT 243
#define FILE_CHECKBOX_LEFT 366

#define FILE_LEFT_LEFT 237
#define FILE_LEFT_TOP 88
#define FILE_LEFT_HEIGHT 34
#define FILE_LEFT_WIDTH 9

#define FILE_RIGHT_LEFT 237
#define FILE_RIGHT_TOP 53
#define FILE_RIGHT_WIDTH 9
#define FILE_RIGHT_HEIGHT 34

#define FILE_DEL_LEFT 237
#define FILE_DEL_TOP 123
#define FILE_DEL_WIDTH 9
#define FILE_DEL_HEIGHT 30

#define FILE_RENAME_LEFT 237
#define FILE_RENAME_TOP 154
#define FILE_RENAME_WIDTH 9
#define FILE_RENAME_HEIGHT 47


#define FILE_FILES_LEFT_LEFT 13
#define FILE_FILES_LEFT_TOP 55
#define FILE_FILES_LEFT_WIDTH 223
#define FILE_FILES_LEFT_HEIGHT 144
#define FILE_FILES_RIGHT_LEFT 247
#define FILE_FILES_RIGHT_TOP 55
#define FILE_FILES_RIGHT_WIDTH 223
#define FILE_FILES_RIGHT_HEIGHT 144

#define FILE_LEFT_CWD_LEFT 13
#define FILE_LEFT_CWD_TOP 40
#define FILE_LEFT_CWD_WIDTH 225
#define FILE_LEFT_CWD_HEIGHT 8
#define FILE_RIGHT_CWD_LEFT 245
#define FILE_RIGHT_CWD_TOP 40
#define FILE_RIGHT_CWD_WIDTH 225
#define FILE_RIGHT_CWD_HEIGHT 8

#define FILE_MAX_FILES_LEFT 1000
#define FILE_LEFT_LINE_MAX 12
#define FILE_MAX_FILES_RIGHT 1000
#define FILE_RIGHT_LINE_MAX 12

int psix_file_state;
int psix_file_side;
int psix_file_checkbox;

int psix_file_left_num;
int psix_file_left_line;
int psix_file_left_line_start;
int psix_file_right_num;
int psix_file_right_line;
int psix_file_right_line_start;

enum psix_checkbox_states {CHECKBOX_OFF, CHECKBOX_OFF_SELECTED,
CHECKBOX_ON, CHECKBOX_ON_SELECTED};


enum psix_file_sides {FILE_LEFT, FILE_RIGHT};
enum psix_file_states {FILE_FILES_LEFT, FILE_FILES_RIGHT,
FILE_MOVE_LEFT, FILE_MOVE_RIGHT,
FILE_DELETE, FILE_RENAME, FILE_NO_CONFIRM, FILE_HELP};


enum psix_image_types {FILE_DIR, FILE_FILE};

struct psix_file_info {
	char path[1024];
	char name[100];
	int type;
};

struct psix_file_info *psix_file_left_files[FILE_MAX_FILES_LEFT];
struct psix_file_info *psix_file_right_files[FILE_MAX_FILES_RIGHT];

char psix_file_left_cwd[512];
char psix_file_right_cwd[512];

void psix_file_init()
{
	int i;
	for (i=0; i < FILE_MAX_FILES_LEFT; i++)
		psix_file_left_files[i] = 0;
	for (i=0; i < FILE_MAX_FILES_RIGHT; i++)
		psix_file_right_files[i] = 0;

	psix_file_state = 0;
	psix_file_left_num = 0;
	psix_file_left_line = 1;
	psix_file_left_line_start = 1;
	psix_file_right_num = 0;
	psix_file_right_line = 1;
	psix_file_right_line_start = 1;

	psix_file_side = FILE_LEFT;
	psix_file_set_checkbox(CHECKBOX_OFF);
	psix_file_set_state(FILE_FILES_LEFT);
	sdi_set_enabled("file_active_left",0);

	sdi_new("file_checkbox");
	sdi_set_position("file_checkbox",FILE_CHECKBOX_LEFT,206);
	sdi_set_overlay("file_checkbox",1);

	sdi_new("file_files_left");
	sdi_set_position("file_files_left",FILE_FILES_LEFT_LEFT,FILE_FILES_LEFT_TOP);
	sdi_set_overlay("file_files_left",1);
	sdi_create_image("file_files_left",FILE_FILES_LEFT_WIDTH,FILE_FILES_LEFT_HEIGHT);

	sdi_new("file_files_right");
	sdi_set_position("file_files_right",FILE_FILES_RIGHT_LEFT,FILE_FILES_RIGHT_TOP);
	sdi_set_overlay("file_files_right",1);
	sdi_create_image("file_files_right",FILE_FILES_RIGHT_WIDTH,FILE_FILES_RIGHT_HEIGHT);
	sdi_move_below("file_files_right","osk");

	sdi_new("file_left_cwd");
	sdi_set_position("file_left_cwd",-480+FILE_LEFT_CWD_LEFT,FILE_LEFT_CWD_TOP);
	sdi_set_overlay("file_left_cwd",1);
	sdi_create_image("file_left_cwd",FILE_LEFT_CWD_WIDTH,FILE_LEFT_CWD_HEIGHT);
	sdi_print_line_max_compress("file_left_cwd","font_black","ms0:/",FILE_LEFT_CWD_WIDTH,1,0);


	sdi_new("file_right_cwd");
	sdi_set_position("file_right_cwd",-480+FILE_RIGHT_CWD_LEFT,FILE_RIGHT_CWD_TOP);
	sdi_set_overlay("file_right_cwd",1);
	sdi_create_image("file_right_cwd",FILE_RIGHT_CWD_WIDTH,FILE_RIGHT_CWD_HEIGHT);
	sdi_print_line_max_compress("file_right_cwd","font_black","ms0:/",FILE_LEFT_CWD_WIDTH,1,0);


	psix_file_left_scan("ms0:/");
	psix_file_draw_left_files();  
	psix_file_right_scan("ms0:/");
	psix_file_draw_right_files();  

	psix_file_get_x_pointers();
}

int *psix_file_x_pointer[12];

void psix_file_get_x_pointers()
{
  psix_file_x_pointer[0] = sdi_get_x_ptr("page_file");
  psix_file_x_pointer[1] = sdi_get_x_ptr("file_files_left");
  psix_file_x_pointer[2] = sdi_get_x_ptr("file_files_right");
  psix_file_x_pointer[3] = sdi_get_x_ptr("file_rename");
  psix_file_x_pointer[4] = sdi_get_x_ptr("file_delete");
  psix_file_x_pointer[5] = sdi_get_x_ptr("file_left");
  psix_file_x_pointer[6] = sdi_get_x_ptr("file_right");
  psix_file_x_pointer[7] = sdi_get_x_ptr("file_active_right");
  psix_file_x_pointer[8] = sdi_get_x_ptr("file_active_left");
  psix_file_x_pointer[9] = sdi_get_x_ptr("file_checkbox");
  psix_file_x_pointer[10] = sdi_get_x_ptr("file_left_cwd");
  psix_file_x_pointer[11] = sdi_get_x_ptr("file_right_cwd");
}


void psix_file_set_left(int x)
{
  *(psix_file_x_pointer[0]) = x;
  *(psix_file_x_pointer[1]) = x+FILE_FILES_LEFT_LEFT;
  *(psix_file_x_pointer[2]) = x+FILE_FILES_RIGHT_LEFT;
  *(psix_file_x_pointer[3]) = x+FILE_RENAME_LEFT;
  *(psix_file_x_pointer[4]) = x+FILE_DELETE_LEFT;
  *(psix_file_x_pointer[5]) = x+FILE_MOVE_LEFT_LEFT;
  *(psix_file_x_pointer[6]) = x+FILE_MOVE_RIGHT_LEFT;
  *(psix_file_x_pointer[7]) = x+FILE_ACTIVE_RIGHT_LEFT;
  *(psix_file_x_pointer[8]) = x+FILE_ACTIVE_LEFT_LEFT;
  *(psix_file_x_pointer[9]) = x+FILE_CHECKBOX_LEFT;
  *(psix_file_x_pointer[10]) = x+FILE_LEFT_CWD_LEFT;
  *(psix_file_x_pointer[11]) = x+FILE_RIGHT_CWD_LEFT;
}

void psix_file_transition_in()
{
	sdi_set_enabled("page_file",1);
	sdi_set_enabled("file_rename",psix_file_state==FILE_RENAME);
	sdi_set_enabled("file_delete",psix_file_state==FILE_DELETE);
	sdi_set_enabled("file_left",psix_file_state==FILE_MOVE_LEFT);
	sdi_set_enabled("file_right",psix_file_state==FILE_MOVE_RIGHT);
	sdi_set_enabled("file_active_right",psix_file_side == FILE_RIGHT);
	sdi_set_enabled("file_active_left",psix_file_side == FILE_LEFT);
	sdi_set_enabled("file_checkbox",1);
	sdi_set_enabled("file_files_left",1);
	sdi_set_enabled("file_files_right",1);
	sdi_set_enabled("file_left_cwd",1);
	sdi_set_enabled("file_right_cwd",1);
}

void psix_file_transition_out()
{
	sdi_set_enabled("page_file",0);
	sdi_set_enabled("file_rename",0);
	sdi_set_enabled("file_delete",0);
	sdi_set_enabled("file_left",0);
	sdi_set_enabled("file_right",0);
	sdi_set_enabled("file_active_right",0);
	sdi_set_enabled("file_active_left",0);
	sdi_set_enabled("file_checkbox",0);
	sdi_set_enabled("file_files_left",0);
	sdi_set_enabled("file_files_right",0);
	sdi_set_enabled("file_left_cwd",0);
	sdi_set_enabled("file_right_cwd",0);
}

void psix_file_set_state(int state)
{
	if (state == psix_file_state) return;

	psix_file_state = state;

	switch (state)
	{
	case FILE_FILES_LEFT: 
		psix_file_set_side(FILE_LEFT); 
		psix_file_draw_left_files();
		break;
	case FILE_FILES_RIGHT: 
		psix_file_set_side(FILE_RIGHT);
		psix_file_draw_right_files();
		break;
	}

	sdi_set_enabled("file_rename",state==FILE_RENAME);
	sdi_set_enabled("file_delete",state==FILE_DELETE);
	sdi_set_enabled("file_left",state==FILE_MOVE_LEFT);
	sdi_set_enabled("file_right",state==FILE_MOVE_RIGHT);

	if (state == FILE_NO_CONFIRM)
		switch (psix_file_checkbox)
	{
		case CHECKBOX_ON: 
			psix_file_set_checkbox(CHECKBOX_ON_SELECTED); break;
		case CHECKBOX_OFF:  
			psix_file_set_checkbox(CHECKBOX_OFF_SELECTED); break;
	}
	else
	{
		switch (psix_file_checkbox)
		{
		case CHECKBOX_ON_SELECTED: 
			psix_file_set_checkbox(CHECKBOX_ON); break;
		case CHECKBOX_OFF_SELECTED:  
			psix_file_set_checkbox(CHECKBOX_OFF); break;
		}
	}
}

void psix_file_set_side(int side)
{
	psix_file_side = side;
	sdi_set_enabled("file_active_left",side==FILE_LEFT);
	sdi_set_enabled("file_active_right",side==FILE_RIGHT);
}

void psix_file_keypress(u32 pad)
{
	if (pad & PSP_CTRL_LEFT)
	{
		switch (psix_file_state)
		{
		case FILE_MOVE_LEFT:
		case FILE_MOVE_RIGHT:
		case FILE_DELETE:
		case FILE_RENAME:
			psix_file_set_state(FILE_FILES_LEFT);
			break;
		case FILE_FILES_RIGHT:
			psix_file_set_state(FILE_MOVE_LEFT);
			break;
		case FILE_FILES_LEFT:
			psix_file_set_state(FILE_HELP);
			break;
		case FILE_NO_CONFIRM:
			psix_file_set_state(FILE_FILES_RIGHT);
			break;
		}
	}
	if (pad & PSP_CTRL_RIGHT)
	{
		switch (psix_file_state)
		{
		case FILE_MOVE_LEFT:
		case FILE_MOVE_RIGHT:
		case FILE_DELETE:
		case FILE_RENAME:
			psix_file_set_state(FILE_FILES_RIGHT);
			break;
		case FILE_FILES_LEFT:
			psix_file_set_state(FILE_MOVE_RIGHT);
			break;
		case FILE_FILES_RIGHT:
			psix_file_set_state(FILE_NO_CONFIRM);
			break;
		case FILE_HELP:
			psix_file_set_state(FILE_FILES_LEFT);
			break;
		}
	}
	if (pad & PSP_CTRL_DOWN)
	{
		switch (psix_file_state)
		{
		case FILE_MOVE_LEFT:
			psix_file_set_state(FILE_DELETE);
			break;
		case FILE_MOVE_RIGHT:
			psix_file_set_state(FILE_MOVE_LEFT);
			break;
		case FILE_DELETE:
			psix_file_set_state(FILE_RENAME);
			break;
		case FILE_FILES_LEFT:
			psix_file_increment_left_line();
			break;
		case FILE_FILES_RIGHT:
			psix_file_increment_right_line();
			break;
		}
	}
	if (pad & PSP_CTRL_UP)
	{
		switch (psix_file_state)
		{
		case FILE_RENAME:
			psix_file_set_state(FILE_DELETE);
			break;
		case FILE_DELETE:
			psix_file_set_state(FILE_MOVE_LEFT);
			break;
		case FILE_MOVE_LEFT:
			psix_file_set_state(FILE_MOVE_RIGHT);
			break;
		case FILE_FILES_LEFT:
			psix_file_decrement_left_line();
			break;
		case FILE_FILES_RIGHT:
			psix_file_decrement_right_line();
			break;
		case FILE_NO_CONFIRM:
			psix_file_set_state(FILE_FILES_RIGHT);
			break;
		case FILE_HELP:
			psix_file_set_state(FILE_FILES_LEFT);
			break;
		}
	}
	if (pad & PSP_CTRL_CROSS)
	{
	  switch (psix_file_state)
	    {
	    case FILE_NO_CONFIRM: psix_file_toggle_checkbox(); break;
	    case FILE_FILES_LEFT:
	      psix_file_select_left();
	      break;
	    case FILE_FILES_RIGHT:
	      psix_file_select_right();
	      break;
	    case FILE_DELETE:
	      psix_file_delete();
	      break;	  
	    case FILE_MOVE_RIGHT:
	      psix_file_copy();
	      break;
	    case FILE_MOVE_LEFT:
	      psix_file_copy();
	      break;
	    case FILE_RENAME:
	      psix_file_rename();
	      
	      break;
	    }
	}
}

void psix_file_rename()
{
  int result=0;
  char b[512];
  char oldpath[512];
  char newpath[512];
  char path_left[512];
  char path_right[512];
  int left_line = psix_file_left_line;
  int right_line = psix_file_right_line;

  strcpy(path_left,psix_file_left_cwd);
  strcpy(path_right,psix_file_right_cwd);
    
   
  if (psix_file_side == FILE_LEFT)
    {
      if (psix_file_left_files[psix_file_left_line-1]->type == FILE_DIR)
	return;
      strcpy(b,psix_file_left_files[psix_file_left_line-1]->name);
      result = psix_osk_get_string(b);
    }
  else if (psix_file_side == FILE_RIGHT)
    {
      if (psix_file_right_files[psix_file_right_line-1]->type == FILE_DIR)
	return;
      strcpy(b,psix_file_right_files[psix_file_right_line-1]->name);
      result = psix_osk_get_string(b);
    }

  //return;

  if (result)
    {
      //psix_draw_stop = 1; 
      //sceKernelDelayThread(1000000);
      //pspDebugScreenInit();
      
      //printf("got a result\n");
      
      if (psix_file_side == FILE_LEFT)
	strcpy(oldpath,psix_file_left_files[psix_file_left_line-1]->path);
      else if (psix_file_side == FILE_RIGHT)
	strcpy(oldpath,psix_file_right_files[psix_file_right_line-1]->path);
      //printf("oldpath: %s\n",oldpath);
      
      strcpy(newpath,oldpath);
      
      //printf("newpath: %s\n",newpath);
      
      while (newpath[strlen(newpath)-1] != '/')
	newpath[strlen(newpath)-1] = 0;
      
      //printf("newpath %s\n",newpath);
      
      strcat(newpath,psix_osk_text);
      //printf("newpath: %s\n",newpath);
      
      //printf("renaming %s to %s\n",oldpath,newpath);
      
      
      sceIoRename(oldpath,newpath);
      
      psix_file_clear_files_left();
      psix_file_left_scan(path_left);
      psix_file_left_line = left_line;
      if (psix_file_left_line > psix_file_left_num) psix_file_left_line = psix_file_left_num;
      psix_file_draw_left_files();
      
      psix_file_clear_files_right();
      psix_file_right_scan(path_right);
      psix_file_right_line = right_line;
      if (psix_file_right_line > psix_file_right_num) psix_file_right_line = psix_file_right_num;
      psix_file_draw_right_files();
      

      //sceKernelDelayThread(6000000);
      //psix_draw_stop = 0;
    }
}


void psix_file_copy()
{
  /*
  psix_draw_stop = 1;
  sceKernelDelayThread(1000000);
  pspDebugScreenInit();
  printf("in copy\n");
  sceKernelDelayThread(2000000);
  */
  char src_path[512];
  char src_file[512];
  char dest_path[512];
  char path_left[512];
  char path_right[512];
  int left_line = psix_file_left_line;
  int right_line = psix_file_right_line;

  switch (psix_file_state)
    {
    case FILE_MOVE_LEFT:
      if (psix_file_right_files[psix_file_right_line-1]->type == FILE_DIR)
	return;
      strcpy(src_path,psix_file_right_files[psix_file_right_line-1]->path);
      strcpy(dest_path,psix_file_left_cwd);
      break;
    case FILE_MOVE_RIGHT:
      if (psix_file_left_files[psix_file_left_line-1]->type == FILE_DIR)
	return;
      strcpy(src_path,psix_file_left_files[psix_file_left_line-1]->path);
      strcpy(dest_path,psix_file_right_cwd);
      break;
    default: return;
    }
  /*	
  strcpy(path_left,psix_file_left_files[psix_file_left_line-1]->path);
  strcpy(path_right,psix_file_right_files[psix_file_right_line-1]->path);
  
  while (path_left[strlen(path_left)-1] != '/')
    path_left[strlen(path_left)-1] = 0;
  
  while (path_right[strlen(path_right)-1] != '/')
    path_right[strlen(path_right)-1] = 0;
  */

  strcpy(path_left,psix_file_left_cwd);
  strcpy(path_right,psix_file_right_cwd);

  /* get the src file name */
  int i=strlen(src_path);
  int len = i;
  int num=0;
  while (src_path[i-1] != '/')
    {
      i--;
      num++;
    }

  //printf("num: %d\n",num);

  if (dest_path[strlen(dest_path)-1] != '/')
    strcat(dest_path,"/");

  //printf("dest_path: %s\n", dest_path);
  
  i=0;
  while(num >= 0)
    src_file[i++] = src_path[len-num--];

  while (src_path[strlen(src_path)-1] != '/')
    src_path[strlen(src_path)-1] = 0;
  
  //while (dest_path[strlen(dest_path)-1] != '/')
  //dest_path[strlen(dest_path)-1] = 0;

  /*
  printf("src_file: %s\n",src_file);


  printf("src_path: %s\n",src_path);
  printf("dest_path: %s\n",dest_path);
  printf("left_dir: %s\n", path_left);
  printf("right_dir: %s\n", path_right);
  */

  char rp[512];
  char wp[512];

  sprintf(rp,"%s%s",src_path, src_file);
  sprintf(wp,"%s%s",dest_path, src_file);

  //printf("copying %s to %s\n", rp, wp);
  
  if(!strcmp(rp,wp))
    return;
  

  
  int usb_enabled=psix_usb_enabled();
  if (usb_enabled)
    {
      //printf("disabling usb\n");
      psix_usb_toggle();
    }
  
#define BUFSIZE 32*1024

  int fd_read = sceIoOpen(rp, PSP_O_RDONLY, 0777);
  int fd_write = sceIoOpen(wp, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
  int readBytes = 0;
  char readBuffer[BUFSIZE];
  readBytes = sceIoRead(fd_read, readBuffer, BUFSIZE);
  while (readBytes > 0)
    {
      sceIoWrite(fd_write, readBuffer, readBytes);
      readBytes = sceIoRead(fd_read, readBuffer, BUFSIZE);
    }
  sceIoClose(fd_read);
  sceIoClose(fd_write);
  

  if (usb_enabled)
    {
      //printf("enabling usb\n");
      psix_usb_toggle();
    }

  //sceKernelDelayThread(10000000);

  psix_file_clear_files_left();
  psix_file_left_scan(path_left);
  psix_file_left_line = left_line;
  if (psix_file_left_line > psix_file_left_num) psix_file_left_line = psix_file_left_num;
  psix_file_draw_left_files();
  
  psix_file_clear_files_right();
  psix_file_right_scan(path_right);
  psix_file_right_line = right_line;
  if (psix_file_right_line > psix_file_right_num) psix_file_right_line = psix_file_right_num;
  psix_file_draw_right_files();
  
  switch (psix_file_side)
    {
    case FILE_LEFT:
      psix_file_set_state(FILE_FILES_RIGHT);
      break;
    case FILE_RIGHT:
      psix_file_set_state(FILE_FILES_LEFT);
      break;
    }
  
  //psix_draw_stop = 0;
}

void psix_file_delete()
{
  //psix_draw_stop = 1;

	//sceKernelDelayThread(1000000);
	//pspDebugScreenInit();
	//printf("in delete\n");
	//sceKernelDelayThread(2000000);

	char path[512];
	char path_left[512];
	char path_right[512];
	int left_line = psix_file_left_line;
	int right_line = psix_file_right_line;

	switch (psix_file_side)
	{
	case FILE_LEFT:
	  if (psix_file_left_files[psix_file_left_line-1]->type == FILE_DIR)
	    return;
	  strcpy(path,psix_file_left_files[psix_file_left_line-1]->path);
	  break;
	case FILE_RIGHT:
	  if (psix_file_right_files[psix_file_right_line-1]->type == FILE_DIR)
	    return;
	  strcpy(path,psix_file_right_files[psix_file_right_line-1]->path);
	  break;
	default: return;
	}

	strcpy(path_left,psix_file_left_cwd);
	strcpy(path_right,psix_file_right_cwd);

	//printf("path1: %s\n",path);
	//printf("path2: %s\n",path2);
	//sceKernelDelayThread(5000000);

	int usb_enabled=psix_usb_enabled();
	if (usb_enabled)
	  {
	    //printf("disabling usb\n");
	    psix_usb_toggle();
	  }
	//printf("removing file\n");
	sceIoRemove(path);
	if (usb_enabled)
	  {
	    //printf("enabling usb\n");
	    psix_usb_toggle();
	  }

	//sceKernelDelayThread(5000000);

	psix_file_clear_files_left();
	psix_file_left_scan(path_left);
	psix_file_left_line = left_line;
	if (psix_file_left_line > psix_file_left_num) psix_file_left_line = psix_file_left_num;
	psix_file_draw_left_files();

	psix_file_clear_files_right();
	psix_file_right_scan(path_right);
	psix_file_right_line = right_line;
	if (psix_file_right_line > psix_file_right_num) psix_file_right_line = psix_file_right_num;
	psix_file_draw_right_files();

	switch (psix_file_side)
	{
	case FILE_LEFT:
		psix_file_set_state(FILE_FILES_LEFT);
		break;
	case FILE_RIGHT:
		psix_file_set_state(FILE_FILES_RIGHT);
		break;
	}

	//psix_draw_stop = 0;
}

void psix_file_select_left()
{
	psix_file_select(psix_file_left_files[psix_file_left_line-1]);
}

void psix_file_select_right()
{
	psix_file_select(psix_file_right_files[psix_file_right_line-1]);
}


void psix_file_select(void *filep)
{
	struct psix_file_info *file = (struct psix_file_info *)filep;
	switch(file->type)
	{
	case FILE_DIR:
		sdi_print_line_max_compress("file_left_cwd","font_black",file->path,FILE_FILES_LEFT_WIDTH,1,1);     
		char path[1024];
		strcpy(path,file->path);

		switch (psix_file_side)
		{
		case FILE_FILES_LEFT:
			psix_file_clear_files_left();
			psix_file_left_scan(path);
			psix_file_draw_left_files();
			break;
		case FILE_FILES_RIGHT:
			psix_file_clear_files_right();
			psix_file_right_scan(path);
			psix_file_draw_right_files();
			break;
		}
		break;
	}
}

void psix_file_toggle_checkbox()
{
	switch (psix_file_checkbox)
	{
	case CHECKBOX_OFF: 
		psix_file_set_checkbox(CHECKBOX_ON); break;
	case CHECKBOX_OFF_SELECTED:
		psix_file_set_checkbox(CHECKBOX_ON_SELECTED); break;
	case CHECKBOX_ON: 
		psix_file_set_checkbox(CHECKBOX_OFF); break;
	case CHECKBOX_ON_SELECTED:
		psix_file_set_checkbox(CHECKBOX_OFF_SELECTED); break;  
	}
}

void psix_file_set_checkbox(int value)
{
	if (value == CHECKBOX_ON || value == CHECKBOX_OFF || value == CHECKBOX_ON_SELECTED || value == CHECKBOX_OFF_SELECTED)
		psix_file_checkbox = value;

	psix_file_update_checkbox();
}

void psix_file_update_checkbox()
{
	switch(psix_file_checkbox)
	{
	case CHECKBOX_OFF:
		sdi_copy_image("file_checkbox","checkbox_off"); break;
	case CHECKBOX_OFF_SELECTED:
		sdi_copy_image("file_checkbox","checkbox_off_selected"); break;
	case CHECKBOX_ON:
		sdi_copy_image("file_checkbox","checkbox_on"); break;
	case CHECKBOX_ON_SELECTED:
		sdi_copy_image("file_checkbox","checkbox_on_selected"); break;
	}
}


void psix_file_clear_files_left()
{
	int i;
	for (i=0; i < psix_file_left_num; i++)
	{
		free(psix_file_left_files[i]);
		psix_file_left_files[i] = 0;
	}

	psix_file_left_line = 1;
	psix_file_left_num = 0;
	psix_file_left_line_start = 1;
	sdi_clear("file_files_left");
}

void psix_file_add_file_left(char *name,char *path,int type)
{
	/* find the first free spot */
	int i;
	for (i=0;i < FILE_MAX_FILES_LEFT; i++)
		if (!psix_file_left_files[i])
			break;
	if (i == FILE_MAX_FILES_LEFT) return;

	psix_file_left_num = i+1;

	psix_file_left_files[i] = (struct psix_file_info*)malloc(sizeof(struct psix_file_info));


	strncpy(psix_file_left_files[i]->name,name,99);
	strncpy(psix_file_left_files[i]->path,path,1023);
	(psix_file_left_files[i]->path)[1023] = 0;
	(psix_file_left_files[i]->name)[98] = 0;
	psix_file_left_files[i]->type = type;
}

void psix_file_draw_left_files()
{
	int i=psix_file_left_line_start;

	while (i < psix_file_left_line_start + FILE_LEFT_LINE_MAX && i <= psix_file_left_num)
	{
		if (i == psix_file_left_line && psix_file_side == FILE_LEFT)
			sdi_print_line_max_compress("file_files_left","font_blue",psix_file_left_files[i-1]->name,FILE_FILES_LEFT_WIDTH,1,i-psix_file_left_line_start+1);
		else
			sdi_print_line_max_compress("file_files_left","font_black",psix_file_left_files[i-1]->name,FILE_FILES_LEFT_WIDTH,1,i-psix_file_left_line_start+1);
		i++;
	}
}

void psix_file_increment_left_line()
{
	psix_file_left_line++;
	if (psix_file_left_line >= psix_file_left_num)
		psix_file_left_line = psix_file_left_num;

	if (psix_file_left_line > FILE_LEFT_LINE_MAX)
	{
		if (psix_file_left_line_start + FILE_LEFT_LINE_MAX -1 < psix_file_left_num)
		{
			psix_file_left_line_start++;
			sdi_clear("file_files_left");
		}
	}
	psix_file_draw_left_files();
}

void psix_file_decrement_left_line()
{
	psix_file_left_line--;
	if (psix_file_left_line < 1)
		psix_file_left_line = 1;

	if (psix_file_left_line < psix_file_left_line_start)
	{
		if (psix_file_left_line_start > 1)
			psix_file_left_line_start--;
		sdi_clear("file_files_left");
	}
	psix_file_draw_left_files();
}

int psix_file_left_scan(char *path)
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

				psix_file_add_file_left(buf,buf2,FILE_DIR);
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
			psix_file_add_file_left(dirent.d_name,buf,FILE_FILE);
		}
	}

	sceIoDclose(dd);
	
	sdi_print_line_max_compress("file_left_cwd","font_black",path,FILE_LEFT_CWD_WIDTH,1,0);

	strcpy(psix_file_left_cwd,path);

	return 0;
}

void psix_file_clear_files_right()
{
	int i;
	for (i=0; i < psix_file_right_num; i++)
	{
		free(psix_file_right_files[i]);
		psix_file_right_files[i] = 0;
	}

	psix_file_right_line = 1;
	psix_file_right_num = 0;
	psix_file_right_line_start = 1;
	sdi_clear("file_files_right");
}

void psix_file_add_file_right(char *name,char *path,int type)
{
	/* find the first free spot */
	int i;
	for (i=0;i < FILE_MAX_FILES_RIGHT; i++)
		if (!psix_file_right_files[i])
			break;
	if (i == FILE_MAX_FILES_RIGHT) return;

	psix_file_right_num = i+1;

	psix_file_right_files[i] = (struct psix_file_info*)malloc(sizeof(struct psix_file_info));
	strncpy(psix_file_right_files[i]->name,name,99);
	strncpy(psix_file_right_files[i]->path,path,1023);
	(psix_file_right_files[i]->path)[1023] = 0;
	(psix_file_right_files[i]->name)[98] = 0;
	psix_file_right_files[i]->type = type;
}

void psix_file_draw_right_files()
{
	int i=psix_file_right_line_start;

	while (i < psix_file_right_line_start + FILE_RIGHT_LINE_MAX && i <= psix_file_right_num)
	{
		if (i == psix_file_right_line && psix_file_side == FILE_RIGHT)
			sdi_print_line_max_compress("file_files_right","font_blue",psix_file_right_files[i-1]->name,FILE_FILES_RIGHT_WIDTH,1,i-psix_file_right_line_start+1);
		else
			sdi_print_line_max_compress("file_files_right","font_black",psix_file_right_files[i-1]->name,FILE_FILES_RIGHT_WIDTH,1,i-psix_file_right_line_start+1);
		i++;
	}
}

void psix_file_increment_right_line()
{
	psix_file_right_line++;
	if (psix_file_right_line >= psix_file_right_num)
		psix_file_right_line = psix_file_right_num;

	if (psix_file_right_line > FILE_RIGHT_LINE_MAX)
	{
		if (psix_file_right_line_start + FILE_RIGHT_LINE_MAX -1 < psix_file_right_num)
		{
			psix_file_right_line_start++;
			sdi_clear("file_files_right");
		}
	}
	psix_file_draw_right_files();
}

void psix_file_decrement_right_line()
{
	psix_file_right_line--;
	if (psix_file_right_line < 1)
		psix_file_right_line = 1;

	if (psix_file_right_line < psix_file_right_line_start)
	{
		if (psix_file_right_line_start > 1)
			psix_file_right_line_start--;
		sdi_clear("file_files_right");
	}
	psix_file_draw_right_files();
}

int psix_file_right_scan(char *path)
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

				psix_file_add_file_right(buf,buf2,FILE_DIR);
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
			psix_file_add_file_right(dirent.d_name,buf,FILE_FILE);
		}
	}

	sceIoDclose(dd);

	sdi_print_line_max_compress("file_right_cwd","font_black",path,FILE_LEFT_CWD_WIDTH,1,0);

	strcpy(psix_file_right_cwd,path);

	return 0;
}


int psix_file_mouse_on_object()
{
	int x = (int)psix_cursor_x;
	int y = (int)psix_cursor_y;

	if (x >= FILE_FILES_LEFT_LEFT && x <= FILE_FILES_LEFT_LEFT+FILE_FILES_LEFT_WIDTH &&
		y >= FILE_FILES_LEFT_TOP && y <= FILE_FILES_LEFT_TOP+FILE_FILES_LEFT_HEIGHT)
	{
		int line = (y-FILE_FILES_LEFT_TOP)/12 + psix_file_left_line_start;
		if (line <= psix_file_left_num && psix_file_left_line != line)
		{
			psix_file_left_line = line;
			psix_file_draw_left_files();
		}
		return FILE_FILES_LEFT;
	}

	if (x >= FILE_FILES_RIGHT_LEFT && x <= FILE_FILES_RIGHT_LEFT+FILE_FILES_RIGHT_WIDTH &&
		y >= FILE_FILES_RIGHT_TOP && y <= FILE_FILES_RIGHT_TOP+FILE_FILES_RIGHT_HEIGHT)
	{
		int line = (y-FILE_FILES_RIGHT_TOP)/12 + psix_file_right_line_start;
		if (line <= psix_file_right_num && psix_file_right_line != line)
		{
			psix_file_right_line = line;
			psix_file_draw_right_files();
		}
		return FILE_FILES_RIGHT;

	}

	if (x >= FILE_RIGHT_LEFT && x <= FILE_RIGHT_WIDTH+FILE_RIGHT_LEFT && y >= FILE_RIGHT_TOP && y <= FILE_RENAME_TOP+FILE_RENAME_HEIGHT)
	  {
	    
	    if (x >= FILE_LEFT_LEFT && x <= FILE_LEFT_LEFT+FILE_LEFT_WIDTH && y >= FILE_LEFT_TOP && y <= FILE_LEFT_TOP + FILE_LEFT_HEIGHT)
	      return FILE_MOVE_LEFT;
	    
	    if (x >= FILE_RIGHT_LEFT && x <= FILE_RIGHT_LEFT+FILE_RIGHT_WIDTH && y >= FILE_RIGHT_TOP && y <= FILE_RIGHT_TOP + FILE_RIGHT_HEIGHT)
	      return FILE_MOVE_RIGHT;
	    
	    if (x >= FILE_DEL_LEFT && x <= FILE_DEL_LEFT+FILE_DEL_WIDTH && y >= FILE_DEL_TOP && y <= FILE_DEL_TOP + FILE_DEL_HEIGHT)
	      return FILE_DELETE;
	    
	    if (x >= FILE_RENAME_LEFT && x <= FILE_RENAME_LEFT+FILE_RENAME_WIDTH && y >= FILE_RENAME_TOP && y <= FILE_RENAME_TOP + FILE_RENAME_HEIGHT)
	      return FILE_RENAME;
	    
	    
	    
	    
	  }

	return -1;
}

void psix_file_cursor_moved()
{
	int b_state;
	if ((b_state = psix_file_mouse_on_object()) != -1)
		psix_file_set_state(b_state);
}
