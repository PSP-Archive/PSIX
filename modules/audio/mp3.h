#include <pspaudiolib.h>
#include <mad.h>

void pMP3_Init(int channel);
void pMP3_End();
int pMP3_Load(char *filename);
int pMP3_Play();
void pMP3_Pause();
int MP3_Stop();
void pMP3_GetTimeString(char *dest);
int pMP3_EndOfStream();

