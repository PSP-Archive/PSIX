	.set noreorder

#include "pspstub.s"

	STUB_START "audiolib",0x00090000,0x00090005
	STUB_FUNC  0xA45EB9EA,MP3_Init
	STUB_FUNC  0x2CC7CBA5,MP3_Load
	STUB_FUNC  0x1ACC475D,MP3_Play
	STUB_FUNC  0x45B8745A,MP3_End
	STUB_FUNC  0x6C09C4EA,MP3_GetTimeString
	STUB_FUNC  0x6B9B3338,MP3_EndOfStream
	STUB_FUNC  0x993FB51E,MP3_Pause
	STUB_FUNC  0xFD6A7428,MP3_EndAudio
	STUB_FUNC  0x268B8E77,MP3_Spectrum
	STUB_END
