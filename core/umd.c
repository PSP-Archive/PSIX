#include "psix.h"

#define UMD_LAUNCH_LEFT 144
#define UMD_LAUNCH_TOP 166
#define UMD_LAUNCH_WIDTH 39
#define UMD_LAUNCH_HEIGHT 14

int psix_umd_launch_enabled;
int psix_umd_activated;

void psix_umd_init()
{
	psix_umd_launch_enabled = 0;
	psix_umd_activated = 0;

	sdi_new("umd_title");
	sdi_set_position("umd_title",11,131);
	sdi_create_image("umd_title",172,12);

	sdi_new("umd_region");
	sdi_set_position("umd_region",11,141);
	sdi_create_image("umd_region",172,12);

	sdi_new("umd_required");
	sdi_set_position("umd_required",11,151);
	sdi_create_image("umd_required",172,12);

	sdi_new("umd_icon");
	sdi_set_position("umd_icon",25,48);
	sdi_create_image("umd_icon",144,80);  

}

void psix_umd_activate()
{
	if (psix_umd_activated)
	{
		if (!sceUmdCheckMedium(1))
		{
			psix_umd_activated = 0;
			sdi_clear("umd_icon");
		}
	}
	else
		if (sceUmdCheckMedium(0))//or 0?
		{
			sceUmdActivate(1,"disc0:");
			sceUmdWaitDriveStat(UMD_WAITFORINIT);
			psix_umd_activated = 1;

			/*
			struct psix_pbp_read pbp_data;
			psix_pbp_read("disc0:/PSP_GAME/PARAM.SFO",&pbp_data);
			char buf[256];
			sprintf(buf,"UMD Title: %s",pbp_data.title);
			sdi_print_line_max_compress("umd_title","font_white",buf,172,1,1);
			sprintf(buf,"UMD Region: %s",pbp_data.region);
			sdi_print_line_max_compress("umd_region","font_white",buf,172,1,1);
			sprintf(buf,"UMD Firmware: %s",pbp_data.firmware);
			sdi_print_line_max_compress("umd_required","font_white",buf,172,1,1);
			*/
			sdi_load_image("umd_icon","disc0:/PSP_GAME/ICON0.PNG");
		}  
}


void psix_umd_show()
{
	psix_umd_cursor_moved();
	sdi_set_enabled("umd_window",1);
	if (psix_umd_launch_enabled)
		sdi_set_enabled("umd_launch",1);
	sdi_set_enabled("umd_title",1);
	sdi_set_enabled("umd_region",1);
	sdi_set_enabled("umd_required",1);
	sdi_set_enabled("umd_icon",1);
}

void psix_umd_hide()
{
	sdi_set_enabled("umd_window",0);
	sdi_set_enabled("umd_launch",0);
	sdi_set_enabled("umd_title",0);
	sdi_set_enabled("umd_region",0);
	sdi_set_enabled("umd_required",0);
	sdi_set_enabled("umd_icon",0);
}

char psix_umd_path[] = "UMD.PBP";
void psix_umd_keypress(u32 pad)
{
	psix_umd_activate();
	if (pad & PSP_CTRL_CROSS)
		if (psix_umd_launch_enabled && psix_umd_activated)
		{
			psix_launch_param.argp = psix_umd_path;
			psix_launch_param.args = strlen(psix_launch_param.argp);
			psix_launch_param.key = NULL;
			psix_launch_param.size = sizeof(psix_launch_param) + psix_launch_param.args;
			psix_launch_path = psix_umd_path;
		}
}

void psix_umd_cursor_moved()
{
	int x = (int)psix_cursor_x;
	int y = (int)psix_cursor_y;

	if (x >= UMD_LAUNCH_LEFT && x <= UMD_LAUNCH_LEFT + UMD_LAUNCH_WIDTH && 
		y >= UMD_LAUNCH_TOP && y <= UMD_LAUNCH_TOP + UMD_LAUNCH_HEIGHT)
	{
		psix_umd_launch_enabled = 1;
		sdi_set_enabled("umd_launch",1);
	}
	else
		if (psix_umd_launch_enabled)
		{
			psix_umd_launch_enabled = 0;
			sdi_set_enabled("umd_launch",0);
		}
}


int psix_umd_thread()
{
	while(!psix_done)
	{
		psix_umd_activate();
		sceKernelDelayThread(2000000);
	}
	return 0;
}

SceUID psix_thread_umd;
void psix_umd_start_thread()
{
	psix_thread_umd = sceKernelCreateThread("PSIX UMD Thread", psix_umd_thread,0x10, 128 * 1024, THREAD_ATTR_USER, NULL);  
	sceKernelStartThread(psix_thread_umd, 0, NULL);
}
