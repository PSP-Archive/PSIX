TARGET = video
INCDIR = ../libavformat ../libavcodec ../libavutil

#BUILD_PRX=1
#PRX_EXPORTS=exports.exp
#USE_PSPSDK_LIBC = 1

CFLAGS = -O2 -G0 -Wall #-DHAVE_AV_CONFIG_H
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

OBJS = video.o #main.o video.o exports.o

LIBDIR = /usr/local/pspdev/psp/sdk/include
LDFLAGS = 
LIBS = -lm -lavcodec -lavformat -lpspaudio -lpsprtc -lpsppower

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

copy:
	cp video/EBOOT.PBP /media/PSP/PSP/GAME/video/EBOOT.PBP
