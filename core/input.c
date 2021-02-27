#include "psix.h"

u32 lastpad;
SceCtrlData paddata;
int psix_toptab_state;
int psix_bottab_state;

float psix_cursor_x;
float psix_cursor_y;

#define USB_LEFT    306
#define USB_TOP     256
#define USB_WIDTH   23
#define USB_HEIGHT  16

char avs_pause_on;
char avs_prev_on;
char avs_play_on;
char avs_next_on;
char avs_stop_on;

void psix_input_init()
{
	/* initialize the analog stick */
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG); 

	/* set up the cursor */
	psix_cursor_x = (float)sdi_get_x("cursor");
	psix_cursor_y = (float)sdi_get_y("cursor");

	lastpad = 0;
	psix_toptab_state = 1;
	psix_bottab_state = 0;

	avs_pause_on = 0;
	avs_prev_on = 0;
	avs_play_on = 0;
	avs_next_on = 0;
	avs_stop_on = 0;
}

void psix_input_update_cursor()
{    
	sceCtrlPeekBufferPositive(&paddata, 1); 

	float ystep=0;
	float xstep=0;

	if (paddata.Lx < 90 || paddata.Lx > 170)
	{
		if (paddata.Lx<25)
			xstep = -1;
		else if (paddata.Lx < 50)
			xstep = -.3;
		else if (paddata.Lx < 75)
			xstep = -.1;
		else if (paddata.Lx < 100)
			xstep = -.1;
		else if (paddata.Lx > 225)
			xstep = 1;
		else if (paddata.Lx > 200)
			xstep = .3;
		else if (paddata.Lx > 175)
			xstep = .1;
		else xstep = .1;
	}

	if (paddata.Ly < 100 || paddata.Ly > 150)
	{
		if (paddata.Ly<25)
			ystep = -1;
		else if (paddata.Ly < 50)
			ystep = -.3;
		else if (paddata.Ly < 75)
			ystep = -.1;
		else if (paddata.Ly < 100)
			ystep = -.1;
		else if (paddata.Ly > 225)
			ystep = 1;
		else if (paddata.Ly > 200)
			ystep = .3;
		else if (paddata.Ly > 175)
			ystep = .1;
		else ystep = .1;
	}

	if (ystep == 0.0 && xstep == 0.0)
		return;

	psix_cursor_x += xstep;
	psix_cursor_y += ystep;

	if (psix_cursor_x < 0) psix_cursor_x=0;
	if (psix_cursor_x > 480) psix_cursor_x = 480;
	if (psix_cursor_y < 0) psix_cursor_y=0;
	if (psix_cursor_y > 272) psix_cursor_y = 272;

	sdi_set_x("cursor",(int)psix_cursor_x);
	sdi_set_y("cursor",(int)psix_cursor_y);

	if (!psix_bottab_state)
		psix_dashboard_cursor_moved();

	if (psix_bottab_state)
		switch (psix_bottab_state)
	{
		case 1: psix_audio_cursor_moved(); break;
		case 3: psix_image_cursor_moved(); break;
		case 4: psix_file_cursor_moved(); break;
	}
}

void psix_input_update()
{
	u32 pad = paddata.Buttons & ~lastpad;

	if (psix_osk_visible)
	{
		psix_osk_keypress(pad);
		lastpad = paddata.Buttons;
		return;
	}

	/* see if they hit the home button */
	if (paddata.Buttons & PSP_CTRL_SELECT && paddata.Buttons & PSP_CTRL_START)
	{
	  char *p = malloc(256);
	  sprintf(p,"ms0:/psp/game/psix/eboot.pbp");
	  psix_launch_param.argp = p;
	  psix_launch_param.args = strlen(psix_launch_param.argp);
	  psix_launch_param.key = NULL;
	  psix_launch_param.size = sizeof(psix_launch_param) + psix_launch_param.args;
	  psix_launch_path = p;
	}

	/* turn on the shoulder buttons when pressed */
	if (paddata.Buttons & PSP_CTRL_LTRIGGER)
		sdi_set_enabled("left_shoulder",1);
	else if (lastpad & PSP_CTRL_LTRIGGER)
		sdi_set_enabled("left_shoulder",0);

	if (paddata.Buttons & PSP_CTRL_RTRIGGER)
		sdi_set_enabled("right_shoulder",1);
	else if (lastpad & PSP_CTRL_RTRIGGER)
		sdi_set_enabled("right_shoulder",0);

	/* left trigger pressed */
	if (pad & PSP_CTRL_LTRIGGER)
	{
		psix_toptab_state++;
		if (psix_toptab_state > 4)
			psix_toptab_state = 1;

		int ms0,umd,mod,net;

		switch (psix_toptab_state)
		{
		case 2: umd = 1; ms0 = mod = net = 0; break;
		case 3: mod = 1; umd = ms0 = net = 0; break;
		case 4: net = 1; umd = ms0 = mod = 0; break;
		case 1: default: ms0 = 1; umd = mod = net = 0; break;
		}

		/* tell the dashboard about the switch */
		//      if (!psix_bottab_state)
		switch(psix_toptab_state)
		{
		case 1:
		  //psix_update_hide();
			break;
		case 4:
		  //psix_update_show();
			break;
		}

		psix_dashboard_set_state(psix_toptab_state);


		sdi_set_enabled("ms0_tab",ms0);
		sdi_set_enabled("umd_tab",umd);
		sdi_set_enabled("mod_tab",mod);
		sdi_set_enabled("net_tab",net);
	}
	/* right trigger pressed */
	if (pad & PSP_CTRL_RTRIGGER && !psix_image_fullscreen)
	{
		psix_bottab_state++;
		if (psix_bottab_state > 4)
			psix_bottab_state = 0;

		int audio,video,image,nss;

		switch (psix_bottab_state)
		{
		case 1: audio = 1; video = image = nss = 0; break;
		case 2: video = 1; audio = image = nss = 0; break;
		case 3: image = 1; audio = video = nss = 0; break;
		case 4: nss = 1; audio = video = image = 0; break;
		case 0: default: audio = video = image = nss = 0; break;
		}

		/* tell the page manager about the update */
		psix_page_set_page(psix_bottab_state);

		sdi_set_enabled("audio_tab",audio);
		sdi_set_enabled("video_tab",video);
		sdi_set_enabled("image_tab",image);
		sdi_set_enabled("file_tab",nss);
	}

	if (pad & PSP_CTRL_CROSS)
	  {
	    int x = psix_cursor_x;
	    int y = psix_cursor_y;
	    if (x >= USB_LEFT && x <= USB_LEFT+USB_WIDTH &&
		y >= USB_TOP && y <= USB_TOP+USB_HEIGHT)
	      psix_usb_toggle();
	    
	    if (psix_avs_enabled)
	      if (y >= AVS_PAUSE_TOP && y <= AVS_PAUSE_TOP+AVS_PAUSE_HEIGHT &&
		  x >= AVS_PAUSE_LEFT && x <= AVS_STOP_LEFT+AVS_STOP_WIDTH)
		{
		  if (x >= AVS_PAUSE_LEFT && x <= AVS_PAUSE_LEFT+AVS_PAUSE_WIDTH)
		    psix_audio_pause();
		  else
		    if (x >= AVS_PREV_LEFT && x <= AVS_PREV_LEFT+AVS_PREV_WIDTH)
		      psix_audio_play_prev();	  
		    else
		      if (x >= AVS_PLAY_LEFT && x <= AVS_PLAY_LEFT+AVS_PLAY_WIDTH)
			psix_audio_pause();  
		      else
			if (x >= AVS_NEXT_LEFT && x <= AVS_NEXT_LEFT+AVS_NEXT_WIDTH)
			  psix_audio_play_next();	  
			else
			  if (x >= AVS_STOP_LEFT && x <= AVS_STOP_LEFT+AVS_STOP_WIDTH)
			    psix_audio_stop();	  
		}
	    
	    if (!psix_bottab_state && !psix_osk_visible)
				switch (psix_toptab_state)
			{
				case 1: psix_dashboard_click(); break;
			case 2: psix_dashboard_click(); break;// psix_umd_keypress(pad); break;
			}
	}

	if (pad)
	{
		switch (psix_bottab_state)
		{
		case 1:
			psix_audio_keypress(pad);
			break;
		case 3:
			psix_image_keypress(pad);
			break;
		case 4:
			psix_file_keypress(pad);
			break;
		}

		if (!psix_bottab_state)
			switch (psix_toptab_state)
		{
		case 4: break;//psix_update_keypress(pad); break;
		}
	}
	lastpad = paddata.Buttons;
}


int psix_input_thread()
{
	while(!psix_done)
	{
		psix_input_update_cursor();
		sceKernelDelayThread(3000);
	}
	return 0;
}

SceUID psix_thread_input;
void psix_input_start_thread()
{
	psix_thread_input = sceKernelCreateThread("PSIX Input Thread", psix_input_thread,0x10, 128 * 1024, THREAD_ATTR_USER, NULL);  
	sceKernelStartThread(psix_thread_input, 0, NULL);
}
