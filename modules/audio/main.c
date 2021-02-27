#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include "mp3.h"

PSP_MODULE_INFO("Audio Library", 0x0000, 1, 1);
PSP_HEAP_SIZE_KB(32);

int action=0;
int intarg;
char strarg[512];
int res;
int *iparg;
int iarg1;
int iarg2;

enum actions {NONE,INIT,LOAD,PLAY,GETTIME,GETEOS,PAUSE,END,ENDAUDIO,SPECTRUM};

void MP3_Init(int channel)
{
  intarg = channel;
  action = INIT;
  while (action != NONE) {sceDisplayWaitVblankStart();}
}

int MP3_Load(char *path)
{
  strcpy(strarg,path);
  action = LOAD;
  while (action != NONE) {sceDisplayWaitVblankStart();}
  return res;
}

int MP3_Spectrum(int *si, int m, int max)
{
  iparg = si;
  iarg1 = m;
  iarg2 = max;
  action = SPECTRUM;
  while (action != NONE) {sceDisplayWaitVblankStart();}
  return res;
}

void MP3_Play()
{
  pMP3_Play();
  //action = PLAY;
  //while (action != NONE) {sceDisplayWaitVblankStart();}
}

void MP3_End()
{
  action = END;
  while (action != NONE) {sceDisplayWaitVblankStart();}
}

void MP3_GetTimeString(char *str)
{
  pMP3_GetTimeString(str);
}

int MP3_EndOfStream()
{
  return pMP3_EndOfStream();
}

void MP3_Pause()
{
  pMP3_Pause();
  //action = PAUSE;
  //while (action != NONE) {sceDisplayWaitVblankStart();}
}

void MP3_EndAudio()
{
  action = ENDAUDIO;
  while (action != NONE) {sceDisplayWaitVblankStart();}
}

int main(int argc, char **argv)
{
  pspAudioInit();
  while (1)
  {
      while (action == NONE) { sceKernelDelayThread(1000); }

      switch (action)
	{
	case INIT: pMP3_Init(intarg); break;
	case LOAD: res = pMP3_Load(strarg); break;
	case PLAY: pMP3_Play(); break;
	case END: pMP3_End(); break;
	case GETTIME: pMP3_GetTimeString(strarg); break;
	case GETEOS: res = pMP3_EndOfStream(); break;
	case PAUSE: pMP3_Pause(); break;
	case ENDAUDIO: pspAudioEnd(); break;
	case SPECTRUM: pMP3_Spectrum(iparg,iarg1,iarg2);
	}

      action = NONE;
  }
  /*pMP3_Init(0);
  pMP3_Load("ms0:/PSP/MUSIC/10.mp3");
  pMP3_Play();
  pspDebugScreenInit();
  while (1)
  {
  	char *time = malloc(128);
  	pMP3_GetTimeString(time);
  	pspDebugScreenPrintf("Bleh: %s", time);
  	sceKernelDelayThread(1000);
  }*/
  sceKernelSleepThread();
  return 0;
}
