#include "psix.h"
#include <time.h>

void psix_time_init()
{
	sdi_new("time");
	sdi_set_x("time",333);
	sdi_set_y("time",0);
	sdi_set_enabled("time",1);
}

int psix_time_thread()
{
	int old_min=0;
	struct tm *loctime; 
	time_t curtime;

	while(!psix_done)
	{
		curtime = time(NULL);
		loctime = gmtime(&curtime);

		if (loctime->tm_min != old_min)
		{
			char buffer[256];
			old_min = loctime->tm_min;
			strftime (buffer, 256, "%I:%M%P %B %d, %Y", loctime);
			sdi_print("time","font_gray",buffer);
			int w = sdi_get_width("time");
			sdi_set_x("time",331+((480-331)-w)/2);
		}

		sceKernelDelayThread(1000000);
	}
	return 0;
}

SceUID psix_thread_time;
void psix_time_start_thread()
{
	psix_thread_time = sceKernelCreateThread("PSIX Time Thread", psix_time_thread,0x10, 32 * 1024, THREAD_ATTR_USER, NULL);  
	sceKernelStartThread(psix_thread_time, 0, NULL);
}

