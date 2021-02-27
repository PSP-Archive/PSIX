#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspiofilemgr.h>
#include <pspiofilemgr_dirent.h
#include allformats.c

#include "video.h"

PSP_MODULE_INFO("Video Prx", 0x1000, 1, 1);
PSP_HEAP_SIZE_KB(8192);

#define debug pspDebugScreenPrintf

videoFile* videoPlaylist;
int videoPlaylistCount;
int isPlaying;

int main()
{
	pspDebugScreenInit();
	pspDebugScreenClear();
	debug("Started.\n");
	//Figure out where our home dir is, for now it's hardcoded
	char* curDir = "ms0:/PSP/VIDEO/";

	//Enumerate all video files and add them to the playlist
	int curDirD = sceIoDopen(curDir);
	if (curDirD >= 0) {
		while (1) {
			SceIoDirent fileInfo;
			int ret = sceIoDread(curDirD, &fileInfo);
			if (ret > 0 && FIO_SO_ISREG(fileInfo.d_stat.st_attr)) {
				debug("New file... %s", fileInfo.d_name);
				//There is a file
				if (strstr(fileInfo.d_name, ".avi") != NULL) {
					//It is an AVI, add it to the playlist
					//videoFile newVideoFile = malloc(sizeof(videoFile));
					//videoPlaylist[videoPlaylistCount] = ;
					//videoPlaylistCount++;
					debug(" - It's an AVI!");
				}
				debug("\n");
			} else if (ret == 0) {
				//No more files
				break;
			} else {
				//Error
			}
		}
		sceIoDclose(curDirD);
	} else {
		//Error
	}
	//sceKernelSleepThread();
	sceKernelDelayThread(5000000);
	sceKernelExitGame();
	return 0;
}

void videoPlay(videoFile video, int fullScreen)
{
	if (isPlaying == 0) {
		//Open the file for reading
		video.fd = sceIoOpen(video.fileName, PSP_O_RDONLY, 0777);

		//Decode the video
		while(isPlaying != -1) {
			if (isPlaying == 1) {
				//Decode frames
				int framesLeft = 0;
				//Do something with them
				if (framesLeft <= 0) {
					isPlaying = -1;
				}
			} else {
				//We are paused... wait for it to unpause.
			}
		}

		//Close the file
		sceIoClose(video.fd);
	}
}

void videoPause()
{
	//Pause/unpause decoding
	isPlaying = !isPlaying;
}

void videoStop()
{
	//Stop decoding
	isPlaying = -1;
}
