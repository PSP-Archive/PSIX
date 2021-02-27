// mp3player.c: MP3 Player Implementation in C for Sony PSP
//
////////////////////////////////////////////////////////////////////////////
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspiofilemgr.h>
#include <pspdisplay.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <pspaudiolib.h>
#include "mp3.h"

#define FALSE 0
#define TRUE !FALSE
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define MadErrorString(x) mad_stream_errorstr(x)
#define INPUT_BUFFER_SIZE	(5*8192)
#define OUTPUT_BUFFER_SIZE	2048	/* Must be an integer multiple of 4. */

mad_fixed_t Filter[32];
int DoFilter = 0;

#define NUMCHANNELS 2
u8 *ptr;
long size;
long samplesInOutput = 0;

extern int counter;

struct mad_stream Stream;
struct mad_frame Frame;
struct mad_synth Synth;
mad_timer_t Timer;
signed short OutputBuffer[OUTPUT_BUFFER_SIZE];
unsigned char InputBuffer[INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD],
    *OutputPtr = (unsigned char *) OutputBuffer, *GuardPtr = NULL;
const unsigned char *OutputBufferEnd = (unsigned char *) OutputBuffer + OUTPUT_BUFFER_SIZE * 2;
int Status = 0, i;
unsigned long FrameCount = 0;

static int isPlaying;		// Set to true when a mod is being played
static int myChannel;
static int eos;

#define printf	pspDebugScreenPrintf

static void ApplyFilter(struct mad_frame *Frame)
{
  int Channel, Sample, Samples, SubBand;
  
  Samples = MAD_NSBSAMPLES(&Frame->header);
  if (Frame->header.mode != MAD_MODE_SINGLE_CHANNEL)
    for (Channel = 0; Channel < 2; Channel++)
      for (Sample = 0; Sample < Samples; Sample++)
	for (SubBand = 0; SubBand < 32; SubBand++)
	  Frame->sbsample[Channel][Sample][SubBand] =
	    mad_f_mul(Frame->sbsample[Channel][Sample][SubBand], Filter[SubBand]);
  else
    for (Sample = 0; Sample < Samples; Sample++)
      for (SubBand = 0; SubBand < 32; SubBand++)
	Frame->sbsample[0][Sample][SubBand] = mad_f_mul(Frame->sbsample[0][Sample][SubBand], Filter[SubBand]);
}

static signed short MadFixedToSshort(mad_fixed_t Fixed)
{
  /* Clipping */
  if (Fixed >= MAD_F_ONE)
    return (SHRT_MAX);
  if (Fixed <= -MAD_F_ONE)
    return (-SHRT_MAX);
  
  /* Conversion. */
  Fixed = Fixed >> (MAD_F_FRACBITS - 15);
  return ((signed short) Fixed);
}

static void MP3Callback_Real(void *_buf2, unsigned int numSamples, void *pdata);
#define ucp(address)	((void*)((u32)(address)|0x40000000))

short abuf[500*2*2];
volatile u8 r;
volatile unsigned int n;

void MP3_ME()
{
  volatile u8 *ready = ucp(&r);
  volatile unsigned int *numSamples = ucp(&n);

  *ready = 2;

  while (1)
    {
      while (*ready != 1) {}
      MP3Callback_Real(ucp(abuf), *numSamples, 0);
      *ready = 2;     
    }
}

static void MP3Callback(void *_buf2, unsigned int numSamples, void *pdata)
{
  volatile u8 *ready = ucp(&r);
  volatile unsigned int *num = ucp(&n);

  while (*ready != 2) {}
  *num = numSamples;
  *ready = 1;
  while(*ready != 2) {};

  memcpy((short*)_buf2,ucp(abuf),numSamples*2*2);
}



static void MP3Callback_Real(void *_buf2, unsigned int numSamples, void *pdata)
{
  short *_buf = (short *)_buf2;
  unsigned long samplesOut = 0;
  volatile u32 *count = (u32*) 0xBFC00060;
  int *ip = ucp(&isPlaying);

  if (*ip == TRUE) {  
    *count = 0;
    if (samplesInOutput > 0) {
      if (samplesInOutput > numSamples) {
	memcpy((char *) _buf, (char *) OutputBuffer, numSamples * 2 * 2);
	samplesOut = numSamples;
	samplesInOutput -= numSamples;
      } else {
	memcpy((char *) _buf, (char *) OutputBuffer, samplesInOutput * 2 * 2);
	samplesOut = samplesInOutput;
	samplesInOutput = 0;
      }
    }
    while (samplesOut < numSamples) {
      if (Stream.buffer == NULL || Stream.error == MAD_ERROR_BUFLEN) {
	mad_stream_buffer(&Stream, ptr, size);
	Stream.error = 0;
      }
      
      if (mad_frame_decode(&Frame, &Stream)) {
	if (MAD_RECOVERABLE(Stream.error)) {
	  if (Stream.error != MAD_ERROR_LOSTSYNC || Stream.this_frame != GuardPtr) {
	  }
	  return;  
	} else if (Stream.error == MAD_ERROR_BUFLEN) {
	  eos = 1;
	  return;	
	} else {
	  Status = 1;
	  MP3_Stop();	
	}
      }
      
      FrameCount++;
      mad_timer_add(&Timer, Frame.header.duration);
      
      if (DoFilter)
	ApplyFilter(&Frame);
      
      mad_synth_frame(&Synth, &Frame);
      
      for (i = 0; i < Synth.pcm.length; i++) {
	signed short Sample;
	if (samplesOut < numSamples) {
	  Sample = MadFixedToSshort(Synth.pcm.samples[0][i]);
	  _buf[samplesOut * 2] = Sample;
	  
	  if (MAD_NCHANNELS(&Frame.header) == 2)
	    Sample = MadFixedToSshort(Synth.pcm.samples[1][i]);
	  
	  _buf[samplesOut * 2 + 1] = Sample;
	  samplesOut++;
	} else {
	  Sample = MadFixedToSshort(Synth.pcm.samples[0][i]);
	  OutputBuffer[samplesInOutput * 2] = Sample;
	  if (MAD_NCHANNELS(&Frame.header) == 2)
	    Sample = MadFixedToSshort(Synth.pcm.samples[1][i]);
	  OutputBuffer[samplesInOutput * 2 + 1] = Sample;
	  samplesInOutput++;
	}
	
      }
    }
  } else {
    {
      int count;
      for (count = 0; count < numSamples * 2; count++)
	*(_buf + count) = 0;
    }
  }
}

void MP3_Init(int channel)
{
  myChannel = channel;
  isPlaying = FALSE;
  pspAudioSetChannelCallback(myChannel, MP3Callback,0);
  
  mad_stream_init(&Stream);
  mad_frame_init(&Frame);
  mad_synth_init(&Synth);
  mad_timer_reset(&Timer);
}


void MP3_FreeTune()
{
  if (ptr)
    free(ptr);
  
  mad_synth_finish(&Synth);
  mad_frame_finish(&Frame);
  mad_stream_finish(&Stream);
  
  return;
}

void MP3_End()
{
  MP3_Stop();
  sceKernelDelayThread(50000);
  pspAudioSetChannelCallback(myChannel, 0,0);
  sceKernelDelayThread(50000);
  MP3_FreeTune();
}

int MP3_Load(char *filename)
{
  int fd;
  eos = 0;
  if ((fd = sceIoOpen(filename, PSP_O_RDONLY, 0777)) > 0) {
    size = sceIoLseek(fd, 0, PSP_SEEK_END);
    sceIoLseek(fd, 0, PSP_SEEK_SET);
    ptr = ucp((unsigned char *) malloc(size + 8));
    memset(ptr, 0, size + 8);
    if (ptr != 0) {	      
      sceIoRead(fd, ptr, size);
    } else {
      //printf("Error allocing\n");
      sceIoClose(fd);
      return 0;
    }
    sceIoClose(fd);
  } else {
    return 0;
  }
  //SetMasterVolume(64);
  isPlaying = FALSE;
  return 1;
}

int MP3_Play()
{
  if (isPlaying)
    return FALSE;
  
  isPlaying = TRUE;
  return TRUE;
}

void MP3_Pause()
{
  isPlaying = !isPlaying;
}

int MP3_Stop()
{
  if (isPlaying == FALSE)
    return FALSE;

  isPlaying = FALSE;
  
  memset(OutputBuffer, 0, OUTPUT_BUFFER_SIZE);
  OutputPtr = (unsigned char *) OutputBuffer;
  
  return TRUE;
}

void MP3_GetTimeString(char *dest)
{
  mad_timer_string(Timer, dest, "%02u:%02u", MAD_UNITS_MINUTES, MAD_UNITS_SECONDS, 0);
}

int MP3_EndOfStream()
{
  if (eos == 1)
    return 1;
  return 0;
}
