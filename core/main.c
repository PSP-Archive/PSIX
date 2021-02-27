#include "psix.h"

PSP_MODULE_INFO("PSIX", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0); 
PSP_HEAP_SIZE_KB( 1024 * 20);
PSP_MAIN_THREAD_STACK_SIZE_KB(64);

void psix_version_display();
int psix_main();
void psix_start_drawing();
void psix_launch_wait();
void me_init();

struct buffer_t {
	unsigned char const *start;
	unsigned long length;
};

char psix_path[256];
char *psix_launch_path;
struct SceKernelLoadExecParam psix_launch_param;
unsigned char *psix_mac;
char* psix_macsto = "MACSHERE";

int psix_draw_stop=0;

__attribute__ ((constructor))
void psix_loader_init()
{
	pspKernelSetKernelPC();
	pspSdkInstallNoDeviceCheckPatch();
	pspDebugInstallKprintfHandler(NULL);
	pspDebugInstallErrorHandler(NULL);
	pspDebugInstallStdoutHandler(pspDebugScreenPrintData);
	pspSdkInstallNoPlainModuleCheckPatch();
}

SceUID sceKernelLoadModuleBufferUsbWlan(SceSize len, void *buf, int flags, SceKernelLMOption *option);
int retVal;

SceUID modid;
int psix_done=0;
SceUID psix_thread_main;
int main(int args, char **argp)
{
	scePowerSetClockFrequency(266, 266, 133);

	pspDebugScreenInit();
	pspDebugScreenClear();


	int err = pspSdkLoadInetModules();
	//if (err < 0)
	//  return;
	//pspDebugScreenPrintf("Modules Loaded.\n");


	psix_audio_load_module();

	psix_thread_main = sceKernelCreateThread("PSIX Main Thread", psix_main,0x10, 64 * 1024, THREAD_ATTR_USER, NULL);  
	sceKernelStartThread(psix_thread_main, 0, NULL);

	psix_launch_wait();

	return 0;
}

int psix_main()
{
	psix_mac = malloc(7);
	memset(psix_mac,0,7);
	sceWlanGetEtherAddr(psix_mac);

	/* initialize SDI */
	sdi_init();
	sdi_load_state("ms0:/psp/game/psix/psix.theme");

	/* make sure back exists */
	//if (!sdi_get_image("back")) sdi_wtf();

	/* look for a background */
	sdi_load_png("wall","ms0:/psp/game/psix/background.png");
	sdi_load_jpeg("wall","ms0:/psp/game/psix/background.jpg");  

	/* initialize some systems */
	psix_usb_init();
	psix_dashboard_init();
	psix_input_init();
	psix_power_init();
	psix_page_init();
	psix_time_init();
	psix_audio_init();
	psix_image_init();
	psix_umd_init();
	psix_file_init();
	psix_osk_init();
	psix_update_init();

	// enable usb
	psix_usb_toggle();

	/* display the version number */
	psix_version_display();

	/* put the cursor on the top */
	sdi_move_top("bar_lower");
	sdi_move_top("cursor");

	psix_power_start_thread();
	psix_input_start_thread();
	psix_time_start_thread();
	psix_umd_start_thread();
	psix_start_drawing();

	/* we need that fancy transition */
	psix_transition_in();

	while (!psix_done)
	{
		psix_input_update();
		psix_page_update();
		sceKernelDelayThread(1000);
	}

	return 0;
}

void psix_version_display()
{
	sdi_new("version");
	sdi_set_x("version",155);
	sdi_set_y("version",0);
	sdi_set_enabled("version",1);

	sdi_new("version_number");
	sdi_set_y("version_number",0);
	sdi_set_enabled("version_number",1);

	sdi_new("version_internal");
	sdi_set_y("version_internal",0);
	sdi_set_enabled("version_internal",1);

	sdi_print("version","font_white","Version");
	sdi_print("version_number","font_orange",PSIX_VERSION);
	sdi_print("version_internal","font_white"," Beta");

	sdi_set_x("version_number",sdi_get_width("version")+sdi_get_x("version")+4);
	sdi_set_x("version_internal",sdi_get_width("version_number")+sdi_get_x("version_number"));
}

int psix_draw_thread()
{
	while(!psix_done)
	{
		if (!psix_draw_stop)
			sdi_draw();
		sceDisplayWaitVblankStart();
	}
	return 0;
}

SceUID psix_thread_draw;
void psix_start_drawing()
{
	psix_thread_draw = sceKernelCreateThread("PSIX Drawing Thread", psix_draw_thread,0x10,256 * 1024,THREAD_ATTR_USER | THREAD_ATTR_VFPU, NULL);  
	sceKernelStartThread(psix_thread_draw, 0, NULL);
}

void MP3_EndAudio();
void psix_shutdown()
{
	psix_done = 1;
	sceKernelWaitThreadEnd(psix_thread_input,NULL);
	sceKernelWaitThreadEnd(psix_thread_audio,NULL);
	sceKernelWaitThreadEnd(psix_thread_time,NULL);
	sceKernelWaitThreadEnd(psix_thread_power,NULL);
	sceKernelWaitThreadEnd(psix_thread_draw,NULL);
	MP3_EndAudio();
	psix_usb_shutdown();
	sdi_shutdown();
}

char psix_eboot[] = "ms0:/psp/game/psix/eboot.pbp";
void psix_launch_wait()
{
  psix_launch_path = 0;
  memset(&psix_launch_param,0,sizeof(struct SceKernelLoadExecParam));
  
  //sceKernelLoadExec("ms0:/psp/game/psix/modules/dh.elf",&psix_launch_param);
  
  sprintf(psix_path,"ms0:/psp/game/psix/eboot.php");
  while (1)
    {
      if (paddata.Buttons & PSP_CTRL_START)
	{
	  psix_launch_param.argp = psix_eboot;
	  psix_launch_param.args = strlen(psix_launch_param.argp);
	  psix_launch_param.key = NULL;
	  psix_launch_param.size = sizeof(psix_launch_param) + psix_launch_param.args;
	  psix_launch_path = psix_eboot;
	}
      
      
      if (psix_launch_path)
	{
	  psix_draw_stop = 1;
	  psix_shutdown();
	  
	  if (psix_dashboard_check_extension(psix_launch_path,".ISO") || psix_dashboard_check_extension(psix_launch_path,".CSO"))
	    {
	      devhook_set_iso(psix_launch_path);
	      sceKernelLoadExec("ms0:/psp/game/psix/modules/dh.elf",&psix_launch_param);
	    }
	  else
	    {
	      sceKernelLoadExec("ms0:/psp/game/psix/modules/loader.elf",&psix_launch_param);
	    }
	}
      sceKernelDelayThread(500000);
    }
}
