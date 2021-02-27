#ifndef __CODECMP3_H_
#define __CODECMP3_H_

#include <pspaudiolib.h>
#include <pspaudio.h>
#include <mad.h>
//#include "../../codec.h"

#ifdef __cplusplus
extern "C" {
#endif

  //    void MP3setStubs(codecStubs * stubs);

    void MP3_Init(int channel);
    int MP3_Play();
    void MP3_Pause();
    int MP3_Stop();
    void MP3_End();
    int MP3_Load(char *filename);
    void MP3_GetTimeString(char *dest);
    int MP3_EndOfStream();

#ifdef __cplusplus
}
#endif
#endif
