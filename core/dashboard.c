#include "psix.h"

#define PSIX_DASHBOARD_TOP             34
#define PSIX_DASHBOARD_LEFT             6
#define PSIX_DASHBOARD_BOX_WIDTH       70
#define PSIX_DASHBOARD_BOX_HEIGHT      70
#define PSIX_DASHBOARD_ICON_WIDTH      32
#define PSIX_DASHBOARD_ICON_HEIGHT     32
#define PSIX_DASHBOARD_NUM_X            6
#define PSIX_DASHBOARD_NUM_Y            3
#define PSIX_DASHBOARD_TEXT_PAD         5
#define PSIX_DASHBOARD_MAX_TEXT_WIDTH  68

#define PSIX_DASHBOARD_VDM_LEFT       378
#define PSIX_DASHBOARD_VDM_TOP        256
#define PSIX_DASHBOARD_VDM_WIDTH       59
#define PSIX_DASHBOARD_VDM_HEIGHT      16

struct psix_dashboard_item *items[PSIX_DASHBOARD_NUM_Y][PSIX_DASHBOARD_NUM_X*5];

int psix_dashboard_state;
int psix_dashboard_show_overlay;
int psix_dashboard_selected_i;
int psix_dashboard_selected_j;
int psix_dashboard_selected_hidden_i;
int psix_dashboard_selected_hidden_j;
int psix_dashboard_visible;
char psix_dashboard_vdm_state;
char psix_dashboard_vdm_drawn;

int psix_dashboard_init()
{
	int i,j;
	for (i=0; i < PSIX_DASHBOARD_NUM_X*5; i++)
		for (j=0; j < PSIX_DASHBOARD_NUM_Y; j++)
			items[j][i] = 0;

	sdi_set_enabled("vdm1",1);
	sdi_set_overlay("vdm1",1);
	sdi_set_overlay("vdm2",1);
	sdi_set_overlay("vdm3",1);
	sdi_set_overlay("vdm4",1);

	/* make the first icon MY PSP */
	struct psix_dashboard_item my_psp;
	sprintf(my_psp.name,"My PSP");
	my_psp.path[0] = 0;
	sprintf(my_psp.img,"icon_mypsp");
	my_psp.w = 26;
	my_psp.h = 32;
	psix_dashboard_add_item(&my_psp);

	/* scan ms0 for executables */
	psix_dashboard_scan_for_exec("ms0:/");

	/* do the initial draw */
	psix_dashboard_vdm_state = 1;
	psix_dashboard_vdm_drawn = 0;
	psix_dashboard_vdm_set(1);  

	psix_dashboard_selected_i = -1;
	psix_dashboard_selected_j = -1;

	psix_dashboard_show_overlay = 0;
	psix_dashboard_state = 1;
	psix_dashboard_visible = 1;


	/* set up the umd tab */
	items[0][PSIX_DASHBOARD_NUM_X*4] = malloc(sizeof(struct psix_dashboard_item));
	sprintf(items[0][PSIX_DASHBOARD_NUM_X*4]->name,"My PSP");
	sprintf(items[0][PSIX_DASHBOARD_NUM_X*4]->img,"icon_mypsp");
	items[0][PSIX_DASHBOARD_NUM_X*4]->w = 26;
	items[0][PSIX_DASHBOARD_NUM_X*4]->h = 32;

	
	return 0;
}

void psix_dashboard_get_next_free_icon_space(int *ii, int *ij)
{
	/* find the first free space */
	int i,j;
	for (i=0; i < PSIX_DASHBOARD_NUM_X*4; i++)
		for (j=0; j < PSIX_DASHBOARD_NUM_Y; j++)
			if (!items[j][i])
			{
				*ii = i;
				*ij = j;
				return;
			}
			*ii = *ij = -1;  
}


int psix_dashboard_add_item(struct psix_dashboard_item *new_item)
{
  if (psix_dashboard_check_extension(new_item->path,".ISO") || psix_dashboard_check_extension(new_item->path,".CSO"))
  {
    int i=PSIX_DASHBOARD_NUM_X*4,j;
      for (j=1; j < PSIX_DASHBOARD_NUM_Y; j++)
	if (!items[j][i])
	  {
	    goto ready;
	  }
    
    if (i == -1 || j == -1)
      return -1;

  ready:
    
    /* copy the data */
    items[j][i] = malloc(sizeof(struct psix_dashboard_item));
    memcpy(items[j][i],new_item,sizeof(struct psix_dashboard_item));
  }
  else
    {
      int i,j;
      psix_dashboard_get_next_free_icon_space(&i,&j);
      
      if (i == -1 || j == -1)
	return -1;
      
      /* copy the data */
      items[j][i] = malloc(sizeof(struct psix_dashboard_item));
      memcpy(items[j][i],new_item,sizeof(struct psix_dashboard_item));
    }
  return 0;
}

int psix_dashboard_check_extension(char *filename,char *extension)
{
	char *last_dot=0,*dot=filename;

	while (*dot)
	{
		if (*dot == '.')
			last_dot = dot;
		dot++;
	}

	if (last_dot)
	{
		char name[256];
		strcpy(name,last_dot);
		int i;
		for (i=0;i < strlen(name);i++)
			name[i] = toupper(name[i]);
		if (!strcmp(name,extension))
			return 1;
	}
	return 0;
}

void psix_dashboard_scan_for_exec(char *path)
{
	char buf[256];
	char tbuf[256];
	SceIoDirent dirent;
	int dd = sceIoDopen(path);

	memset(&dirent,0,sizeof(SceIoDirent));
	while (sceIoDread(dd,&dirent) > 0)
	{
		sprintf(buf,"%s%s%s",path,(path[strlen(path)-1]=='/')?"":"/",dirent.d_name);
		if (dirent.d_stat.st_attr & FIO_SO_IFDIR)
		{
			/* We have a directory, if it isn't a % but a corresponding % exists, we want to skip it */
			if (dirent.d_name[strlen(dirent.d_name)-1] != '%')
			{
				sprintf(tbuf,"%s%%",buf);
				SceIoStat stat;
				if (sceIoGetstat(tbuf,&stat) >= 0)
					if (stat.st_attr & FIO_SO_IFDIR)
						continue;
			}
			if (strstr(dirent.d_name, "__SCE"))
			{
				//dirent.d_name = dirent.d_name - strstr(dirent.d_name, "__SCE");
			}

			if (dirent.d_name[strlen(dirent.d_name)-1] != '.')
				psix_dashboard_scan_for_exec(buf);
		}
		else if (dirent.d_stat.st_attr & FIO_SO_IFREG)	
			if (psix_dashboard_check_extension(dirent.d_name,".PBP"))
			{
				struct psix_pbp_read pbp_data;
				struct psix_dashboard_item new_app;
				memset(&new_app,0,sizeof(new_app));
				psix_pbp_read(buf,&pbp_data);

				/* we dont really need to see PSIX on the dash */
				if (!strncmp(pbp_data.title,"PSIX",4))
					continue;

				/* cut out the KXPLOIT PSP-DEV team crap */
				if (strlen(pbp_data.title) > 29)
					if (strncmp(&pbp_data.title[strlen(pbp_data.title)-30]," KXPloit Boot by PSP-DEV Team",29))
						pbp_data.title[strlen(pbp_data.title)-29] = 0;

				/* make sure it actually has a name */
				if (strlen(pbp_data.title) == 0)
					continue;

				/* we dont want the space hack either */
				if (pbp_data.title[0] == ' ')
					continue;

				strcpy(new_app.name,pbp_data.title);

				/* now take care of the % business */
				strcpy(tbuf,buf);
				if (path[strlen(path)-1] == '%')
				{
					strcpy(tbuf,path);
					tbuf[strlen(tbuf)-1] = 0;
					while (tbuf[strlen(tbuf)-1] == ' ')
						tbuf[strlen(tbuf)-1] = 0;
					SceIoStat stat;
					if (sceIoGetstat(tbuf,&stat)>=0)
						if (stat.st_attr & FIO_SO_IFDIR)
							sprintf(buf,"%s%s%s",tbuf,(tbuf[strlen(tbuf)-2]=='/')?"":"/",dirent.d_name);
				}

				/* now finish up */
				strcpy(new_app.path,buf);
				/* Here we copy the ICON0 out of the PBP and composite it with icon_default */
				//sdi_new("icon");
				SceIoStat stat;
				char icon_path[512];
				sprintf(icon_path,"%s%s%s",path,(path[strlen(path)-2]=='/')?"":"/","icon.png");

				if (sceIoGetstat(icon_path,&stat) >= 0)
				{
					if (stat.st_attr & FIO_SO_IFREG)
					{
						/* load up a custom icon */
						int i,j;
						psix_dashboard_get_next_free_icon_space(&i,&j);
						char sdi_name[60];
						sprintf(sdi_name,"dashboard_%d_%d_icon",j,i);
						sdi_new(sdi_name);
						sdi_load_image(sdi_name,icon_path);
						sprintf(new_app.img,sdi_name);
					}
				}
				else
				{
				  /* use the default icon */
				  if (path[strlen(path)-1] == '%')
				    sprintf(new_app.img,"icon_default_15");
				  else
				    sprintf(new_app.img,"icon_default_10");
				}

				new_app.w = 26;
				new_app.h = 32;
				psix_dashboard_add_item(&new_app);
			}
			else if (psix_dashboard_check_extension(dirent.d_name,".ISO") || psix_dashboard_check_extension(dirent.d_name,".CSO"))
			{

			  //struct psix_dashboard_item new_app;
			  //memset(&new_app,0,sizeof(new_app));
			  struct psix_dashboard_item new_icon;
			  strcpy(new_icon.name,dirent.d_name);
			  while (new_icon.name[strlen(new_icon.name)-1] != '.')
			    new_icon.name[strlen(new_icon.name)-1] = 0;
			  new_icon.name[strlen(new_icon.name)-1] = 0;
			  strcpy(new_icon.img,"icon_iso");
			  strcpy(new_icon.path,buf);
			  new_icon.w = 26;
			  new_icon.h = 32;
			  psix_dashboard_add_item(&new_icon);
			}

			memset(&dirent,0,sizeof(SceIoDirent));
	}

	sceIoDclose(dd);
}

void psix_dashboard_set_state(int state)
{
	switch(state)
	  {
	  case 1: 
	    psix_dashboard_show(); 
	    break;
	  case 2: 
	    //psix_dashboard_hide();
	    //psix_umd_show();
	    psix_dashboard_vdm_set(5);
	    break;
	  case 3:
	    psix_dashboard_vdm_set(psix_dashboard_vdm_state);
	    psix_dashboard_hide();
	    //psix_umd_hide();
	  }

	psix_dashboard_state = state;
	return;

	if (psix_dashboard_state == 1)
	{
		if (psix_dashboard_state != state)
			psix_dashboard_hide();
	}
	else
		if (state == 1)
			psix_dashboard_show();

	psix_dashboard_state = state;
}

void psix_dashboard_hide()
{
	/* run through the list of items and disable them */
	int i,j;
	for (i=(psix_dashboard_vdm_state-1)*PSIX_DASHBOARD_NUM_X; i < (psix_dashboard_vdm_state-1)*PSIX_DASHBOARD_NUM_X+PSIX_DASHBOARD_NUM_X; i++)
		for (j=0; j < PSIX_DASHBOARD_NUM_Y; j++)
			if (items[j][i])
			{
				char buf[256];
				sprintf(buf,"dashboard_%d_%d",j,i);
				sdi_set_enabled(buf,0);
				sprintf(buf,"dashboard_%d_%d_t",j,i);
				sdi_set_enabled(buf,0);
			}
			if (psix_dashboard_show_overlay)
				sdi_set_enabled("icon_selected",0);

			psix_dashboard_visible = 0;
}

void psix_dashboard_show()
{
	/* run through the list of items and enable them */
	int i,j;
	for (i=(psix_dashboard_vdm_state-1)*PSIX_DASHBOARD_NUM_X; i < (psix_dashboard_vdm_state-1)*PSIX_DASHBOARD_NUM_X+PSIX_DASHBOARD_NUM_X; i++)
		for (j=0; j < PSIX_DASHBOARD_NUM_Y; j++)
			if (items[j][i])
			{
				char buf[256];
				sprintf(buf,"dashboard_%d_%d",j,i);
				sdi_set_enabled(buf,1);
				sprintf(buf,"dashboard_%d_%d_t",j,i);
				sdi_set_enabled(buf,1);
			}
			if (psix_dashboard_show_overlay && !psix_bottab_state)
				sdi_set_enabled("icon_selected",1);

			psix_dashboard_visible = 1;
}

void psix_dashboard_cursor_moved()
{/*
	if (psix_dashboard_state == 2)
	{
		psix_umd_cursor_moved();
		return;
	}
 */
	if ((psix_dashboard_state != 1 && psix_dashboard_state != 2) || !psix_dashboard_visible || psix_bottab_state)
		return;

	int x = (int)psix_cursor_x;
	int y = (int)psix_cursor_y;

	int icon_left, icon_right;
	int icon_top, icon_bottom;

	int i = (x - PSIX_DASHBOARD_LEFT)/PSIX_DASHBOARD_BOX_WIDTH;
	int j = (y - PSIX_DASHBOARD_TOP)/PSIX_DASHBOARD_BOX_HEIGHT;

	icon_left = PSIX_DASHBOARD_LEFT + i*PSIX_DASHBOARD_BOX_WIDTH + (PSIX_DASHBOARD_BOX_WIDTH - PSIX_DASHBOARD_ICON_WIDTH)/2;
	icon_right = icon_left + PSIX_DASHBOARD_ICON_WIDTH;
	icon_top = PSIX_DASHBOARD_ICON_HEIGHT + j*PSIX_DASHBOARD_BOX_HEIGHT;
	icon_bottom = icon_top + PSIX_DASHBOARD_ICON_HEIGHT;

	int ii = i + (psix_dashboard_vdm_state-1)*PSIX_DASHBOARD_NUM_X;
	if (psix_dashboard_state == 2)
	  psix_dashboard_vdm_state = 5;

	if (x>=icon_left &&x<=icon_right && y>=icon_top && y<=icon_bottom && i < PSIX_DASHBOARD_NUM_X && j < PSIX_DASHBOARD_NUM_Y)
	{
		if (items[j][ii] && psix_dashboard_selected_i != ii && psix_dashboard_selected_j != j)
		{
			if (psix_dashboard_selected_i != -1)
				psix_dashboard_icon_set_overlay(psix_dashboard_selected_j,psix_dashboard_selected_i,0);

			psix_dashboard_icon_set_overlay(j,ii,1);

			psix_dashboard_selected_i = ii;
			psix_dashboard_selected_j = j; 
		}
	}
	else  
		if (psix_dashboard_selected_i != -1)
		{
			psix_dashboard_icon_set_overlay(psix_dashboard_selected_j,psix_dashboard_selected_i,0);
			psix_dashboard_selected_i = -1;
			psix_dashboard_selected_j = -1;
		}
}

void psix_dashboard_icon_set_overlay(int j, int i, int value)
{
	if (value)
	{
		sdi_set_position("icon_selected",PSIX_DASHBOARD_LEFT+(i-(psix_dashboard_vdm_state-1)*PSIX_DASHBOARD_NUM_X)*PSIX_DASHBOARD_BOX_WIDTH+17,PSIX_DASHBOARD_TOP+j*PSIX_DASHBOARD_BOX_HEIGHT-6);
		sdi_set_enabled("icon_selected",1);
	}
	else
		sdi_set_enabled("icon_selected",0);

	psix_dashboard_show_overlay = value;
}

char psix_path[] = "ms0:/psp/game/psix/eboot.pbp";

void psix_dashboard_vdm_set(int vdm)
{
	int i,j;
	char sdi_name[50],sdi_name2[50];
	int old_vdm = vdm-1;
	if (old_vdm < 1) old_vdm = 4;
	if (vdm == 5) old_vdm = psix_dashboard_vdm_state;
	if (vdm == psix_dashboard_vdm_state) old_vdm = 5;		   

	if (psix_dashboard_vdm_drawn)
	{
		for (i=(old_vdm-1)*PSIX_DASHBOARD_NUM_X; i <(old_vdm-1)*PSIX_DASHBOARD_NUM_X+ PSIX_DASHBOARD_NUM_X; i++)
			for (j=0; j < PSIX_DASHBOARD_NUM_Y; j++)
				if (items[j][i])
				{
					sprintf(sdi_name,"dashboard_%d_%d",j,i);
					sprintf(sdi_name2,"dashboard_%d_%d_t",j,i);
					/* there is an icon/text, so free it*/
					sdi_del(sdi_name);
					sdi_del(sdi_name2);
				}
	}

	/* now create the new icons */
	for (i=(vdm-1)*PSIX_DASHBOARD_NUM_X; i < (vdm-1)*PSIX_DASHBOARD_NUM_X+PSIX_DASHBOARD_NUM_X; i++)
		for (j=0; j < PSIX_DASHBOARD_NUM_Y; j++)
			if (items[j][i])
			{
				sprintf(sdi_name,"dashboard_%d_%d",j,i);
				sdi_new(sdi_name);
				sdi_move_below(sdi_name,"low_tab_overlay");
				sdi_set_x(sdi_name,PSIX_DASHBOARD_LEFT+PSIX_DASHBOARD_BOX_WIDTH*(i-(vdm-1)*PSIX_DASHBOARD_NUM_X)+(PSIX_DASHBOARD_BOX_WIDTH-items[j][i]->w)/2);
				sdi_set_y(sdi_name,PSIX_DASHBOARD_TOP+PSIX_DASHBOARD_BOX_HEIGHT*j);
				sdi_copy_image(sdi_name,items[j][i]->img);
				sdi_set_scale(sdi_name,1);
				sdi_set_display_width(sdi_name,PSIX_DASHBOARD_ICON_WIDTH);
				sdi_set_display_height(sdi_name,PSIX_DASHBOARD_ICON_HEIGHT);
				sdi_set_enabled(sdi_name,1);

				sprintf(sdi_name2,"dashboard_%d_%d_t",j,i);
				sdi_new(sdi_name2);
				sdi_move_below(sdi_name2,"low_tab_overlay");
				sdi_print_wrap_compress(sdi_name2,"font_icon",items[j][i]->name,50,1);
				sdi_set_x(sdi_name2,PSIX_DASHBOARD_LEFT+PSIX_DASHBOARD_BOX_WIDTH*(i-(vdm-1)*PSIX_DASHBOARD_NUM_X)+(PSIX_DASHBOARD_BOX_WIDTH-sdi_get_width(sdi_name2))/2+4);
				sdi_set_y(sdi_name2,PSIX_DASHBOARD_TOP+PSIX_DASHBOARD_BOX_HEIGHT*j+PSIX_DASHBOARD_ICON_HEIGHT+PSIX_DASHBOARD_TEXT_PAD);
				sdi_set_enabled(sdi_name2,1);
			}

			psix_dashboard_vdm_drawn = 1;
}


void psix_dashboard_vdm_click()
{
  if (psix_dashboard_state == 2)
    return;

	switch (psix_dashboard_vdm_state)
	{
	case 1: sdi_set_enabled("vdm1",0); break;
	case 2: sdi_set_enabled("vdm2",0); break;
	case 3: sdi_set_enabled("vdm3",0); break;
	case 4: sdi_set_enabled("vdm4",0); break;
	}

	int psix_dashboard_vdm_state2 = psix_dashboard_vdm_state + 1;
	if (psix_dashboard_vdm_state2 > 4)
		psix_dashboard_vdm_state2 = 1;

	psix_dashboard_vdm_set(psix_dashboard_vdm_state2);

	psix_dashboard_vdm_state = psix_dashboard_vdm_state2;

	switch (psix_dashboard_vdm_state)
	{
	case 1: sdi_set_enabled("vdm1",1); break;
	case 2: sdi_set_enabled("vdm2",1); break;
	case 3: sdi_set_enabled("vdm3",1); break;
	case 4: sdi_set_enabled("vdm4",1); break;
	}
}

void psix_dashboard_click()
{
	if (!psix_dashboard_visible)
		return;

	int x = psix_cursor_x;
	int y = psix_cursor_y;

	int icon_left, icon_right;
	int icon_top, icon_bottom;

	int i = (x - PSIX_DASHBOARD_LEFT)/PSIX_DASHBOARD_BOX_WIDTH;
	int j = (y - PSIX_DASHBOARD_TOP)/PSIX_DASHBOARD_BOX_HEIGHT;

	icon_left = PSIX_DASHBOARD_LEFT + i*PSIX_DASHBOARD_BOX_WIDTH + (PSIX_DASHBOARD_BOX_WIDTH - PSIX_DASHBOARD_ICON_WIDTH)/2;
	icon_right = icon_left + PSIX_DASHBOARD_ICON_WIDTH;
	icon_top = PSIX_DASHBOARD_ICON_HEIGHT + j*PSIX_DASHBOARD_BOX_HEIGHT;
	icon_bottom = icon_top + PSIX_DASHBOARD_ICON_HEIGHT;

	if (x >= PSIX_DASHBOARD_VDM_LEFT && 
		x <= PSIX_DASHBOARD_VDM_LEFT+PSIX_DASHBOARD_VDM_WIDTH &&
		y >= PSIX_DASHBOARD_VDM_TOP && 
		y <= PSIX_DASHBOARD_VDM_TOP +PSIX_DASHBOARD_VDM_HEIGHT)
	{
		/* clicked the vdm */
		psix_dashboard_vdm_click();
		return;
	}

	if (!(x>=icon_left &&x<=icon_right && y>=icon_top && y<=icon_bottom))
		return;

	i += (psix_dashboard_vdm_state-1)*PSIX_DASHBOARD_NUM_X;

	if (i >= PSIX_DASHBOARD_NUM_X*5 || j >= PSIX_DASHBOARD_NUM_Y)
		return;
	if (j == 0 && i == 0)
	{
		psix_launch_param.argp = psix_eboot;
		psix_launch_param.args = strlen(psix_launch_param.argp);
		psix_launch_param.key = NULL;
		psix_launch_param.size = sizeof(psix_launch_param) + psix_launch_param.args;
		psix_launch_path = psix_eboot;
		return;
	}

	if (items[j][i])
	{

		psix_launch_param.argp = items[j][i]->path;
		psix_launch_param.args = strlen(psix_launch_param.argp);
		psix_launch_param.key = NULL;
		psix_launch_param.size = sizeof(psix_launch_param) + psix_launch_param.args;
		psix_launch_path = items[j][i]->path;
	}
}
