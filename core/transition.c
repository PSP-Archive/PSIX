#include "psix.h"

int psix_transition_thread()
{
	int dxt=-27;
	int dxb=37;

	/* top */
	int bar_top = sdi_get_y("bar_top");
	int time = sdi_get_y("time");
	int battery_percent = sdi_get_y("battery_percent");
	int cpu_speed = sdi_get_y("cpu_speed");
	int battery_5 = sdi_get_y("battery_5");
	int battery_15 = sdi_get_y("battery_15");
	int battery_25 = sdi_get_y("battery_25");
	int battery_35 = sdi_get_y("battery_35");
	int battery_45 = sdi_get_y("battery_45");
	int battery_55 = sdi_get_y("battery_55");
	int battery_65 = sdi_get_y("battery_65");
	int battery_75 = sdi_get_y("battery_75");
	int battery_85 = sdi_get_y("battery_85");
	int battery_95 = sdi_get_y("battery_95");
	int version = sdi_get_y("version");  
	int version_internal = sdi_get_y("version_internal");
	int version_number = sdi_get_y("version_number");
	int top_tab_overlay = sdi_get_y("top_tab_overlay");
	int ms0_tab = sdi_get_y("ms0_tab");
	int umd_tab = sdi_get_y("umd_tab");
	int mod_tab = sdi_get_y("mod_tab");
	int net_tab = sdi_get_y("net_tab");

	/* bottom */
	int bar_lower = sdi_get_y("bar_lower");
	int low_tab_overlay = sdi_get_y("low_tab_overlay");
	int shoulder_overlay = sdi_get_y("shoulder_overlay");
	int audio_tab = sdi_get_y("audio_tab");
	int video_tab = sdi_get_y("video_tab");
	int image_tab = sdi_get_y("image_tab");
	int nss_tab = sdi_get_y("nss_tab");
	int left_shoulder = sdi_get_y("left_shoulder");
	int right_shoulder = sdi_get_y("right_shoulder");
	int vdm1 = sdi_get_y("vdm1");
	int usb_off = sdi_get_y("usb_off");
	int usb_on = sdi_get_y("usb_on");
	int avs = sdi_get_y("avs");
	int vb0 = sdi_get_y("vis_bar_0");
	int vb1 = sdi_get_y("vis_bar_1");
	int vb2 = sdi_get_y("vis_bar_2");
	int vb3 = sdi_get_y("vis_bar_3");
	int vb4 = sdi_get_y("vis_bar_4");
	int vb5 = sdi_get_y("vis_bar_5");
	int vb6 = sdi_get_y("vis_bar_6");
	int vb7 = sdi_get_y("vis_bar_7");
	int vb8 = sdi_get_y("vis_bar_8");
	int vb9 = sdi_get_y("vis_bar_9");
	int vb10 = sdi_get_y("vis_bar_10");
	int vb11 = sdi_get_y("vis_bar_11");
	int vb12 = sdi_get_y("vis_bar_12");
	int vb13 = sdi_get_y("vis_bar_13");
	int vb14 = sdi_get_y("vis_bar_14");


	int sw = 480;

	int *img = malloc(16*16*4);
	int i,j;
	for (i=0; i < 16; i++)
		for (j=0; j < 16; j++)
			img[i*16 + j] = 0xFF000000;

	sdi_new("black");
	sdi_set_image("black",img,16,16,16,16);
	sdi_set_display_width("black",sw);
	sdi_set_display_height("black",272);
	sdi_set_scale("black",1);
	sdi_move_below("black","bar_top");
	sdi_set_enabled("black",1);

	free(img);

	while (dxt <=0 || dxb >= 0 || sw > 0)
	{
		sdi_set_y("bar_top",dxt+bar_top);
		sdi_set_y("cpu_speed",dxt+cpu_speed);
		sdi_set_y("time",dxt+time);
		sdi_set_y("battery_percent",dxt+battery_percent);
		sdi_set_y("cpu_speed",dxt+cpu_speed);
		sdi_set_y("battery_5",dxt+battery_5);
		sdi_set_y("battery_15",dxt+battery_15);
		sdi_set_y("battery_25",dxt+battery_25);
		sdi_set_y("battery_35",dxt+battery_35);
		sdi_set_y("battery_45",dxt+battery_45);      
		sdi_set_y("battery_55",dxt+battery_55);
		sdi_set_y("battery_65",dxt+battery_65);
		sdi_set_y("battery_75",dxt+battery_75);
		sdi_set_y("battery_85",dxt+battery_85);
		sdi_set_y("battery_95",dxt+battery_95);
		sdi_set_y("version",dxt+version);
		sdi_set_y("version_number",dxt+version_number);
		sdi_set_y("version_internal",dxt+version_internal);
		sdi_set_y("top_tab_overlay",dxt+top_tab_overlay);
		sdi_set_y("ms0_tab",dxt+ms0_tab);
		sdi_set_y("umd_tab",dxt+umd_tab);
		sdi_set_y("mod_tab",dxt+mod_tab);
		sdi_set_y("net_tab",dxt+net_tab);

		/* bottom */
		sdi_set_y("bar_lower",dxb+bar_lower);
		sdi_set_y("low_tab_overlay",dxb+low_tab_overlay);
		sdi_set_y("shoulder_overlay",dxb+shoulder_overlay);
		sdi_set_y("audio_tab",dxb+audio_tab);
		sdi_set_y("video_tab",dxb+video_tab);
		sdi_set_y("image_tab",dxb+image_tab);
		sdi_set_y("nss_tab",dxb+nss_tab);
		sdi_set_y("left_shoulder",dxb+left_shoulder);
		sdi_set_y("right_shoulder",dxb+right_shoulder);
		sdi_set_y("vdm1",dxb+vdm1);
		sdi_set_y("usb_off",dxb+usb_off);
		sdi_set_y("usb_on",dxb+usb_on);
		sdi_set_y("avs",dxb+avs);
		sdi_set_y("vis_bar_0",dxb+vb0);
		sdi_set_y("vis_bar_1",dxb+vb1);
		sdi_set_y("vis_bar_2",dxb+vb2);
		sdi_set_y("vis_bar_3",dxb+vb3);
		sdi_set_y("vis_bar_4",dxb+vb4);
		sdi_set_y("vis_bar_5",dxb+vb5);
		sdi_set_y("vis_bar_6",dxb+vb6);
		sdi_set_y("vis_bar_7",dxb+vb7);
		sdi_set_y("vis_bar_8",dxb+vb8);
		sdi_set_y("vis_bar_9",dxb+vb9);
		sdi_set_y("vis_bar_10",dxb+vb10);
		sdi_set_y("vis_bar_11",dxb+vb11);
		sdi_set_y("vis_bar_12",dxb+vb12);
		sdi_set_y("vis_bar_13",dxb+vb13);
		sdi_set_y("vis_bar_14",dxb+vb14);

		/* resize black */
		sw -= 20;
		if (sw < 0)
			sw = 0;

		sdi_set_display_width("black",sw);
		sdi_set_x("black",(480-sw)/2);


		if (dxt < 0) dxt++;
		if (dxb > 0) dxb--;
		sceKernelDelayThread(30000);
	} 
	sdi_del("black");
	sceKernelExitDeleteThread(0);
	return 0;
}

void psix_transition_in()
{
	SceUID psix_thread_transition = sceKernelCreateThread("PSIX Transition Thread", psix_transition_thread,0x10,32 * 1024,THREAD_ATTR_USER | THREAD_ATTR_VFPU, NULL);  
	sceKernelStartThread(psix_thread_transition, 0, NULL);
}
