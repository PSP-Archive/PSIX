/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - A basic example of getting code onto the ME. This was based on
 * the sample code written by crazyc on the pspdev forums.
 *
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 *
 * $Id: main.c 1095 2005-09-27 21:02:16Z jim $
 */
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "mp3.h"

/* Define the module info section */
PSP_MODULE_INFO("memp3", 0x1000, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(0);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

void me_run(void);
void me_end(void);
void MP3_ME();

int main(int argc, char *argv[])
{
	SceCtrlData ctl;

	pspDebugScreenInit();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

	/*Copy our small program into the ME reset vector */
	memcpy((void *)0xbfc00040, me_run, (int)(me_end - me_run));
	sceKernelDcacheWritebackInvalidateAll();

	_sw((u32)MP3_ME, 0xbfc00600);
	sceKernelDcacheWritebackAll();

	sceSysregMeResetEnable();
	sceSysregMeBusClockEnable();
	sceSysregMeResetDisable();
	sceSysregVmeResetDisable();

	pspAudioInit();

	MP3_Init(0);
	pspDebugScreenPrintf("inited\n");
	if (MP3_Load("ms0:/psp/music/tiens.mp3"))
		pspDebugScreenPrintf("loaded\n");
	   MP3_Play();
	   pspDebugScreenPrintf("playing\n");

	while(1)
	{
		pspDebugScreenSetXY(0, 0);
		pspDebugScreenPrintf("ME MP3, press Home to exit\n");
		pspDebugScreenPrintf("Playing ms0:/test.mp3...\n");
		sceCtrlReadBufferPositive(&ctl, 1);
		pspDebugScreenPrintf("readshit\n");
		if(ctl.Buttons & PSP_CTRL_HOME)
		{
			pspDebugScreenPrintf("got SIGINT(2)\n");
		    pspAudioEnd();
			sceKernelExitGame();
		}

		sceDisplayWaitVblankStart();
	}

	return 0;
}
