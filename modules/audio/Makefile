TARGET = audio
OBJS = main.o mp3.o fft.o exports.o 

BUILD_PRX=1
PRX_EXPORTS=exports.exp

USE_PSPSDK_LIBC = 1

LIBS = -lpspaudiolib -lpspaudio -lmad -lpsppower

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

ia:
	cp audio.prx /media/PSP/PSP/GAME/psix/modules/audio.prx
	make clean

install:
	sudo mount /dev/sda1 /mnt/psp
	sudo cp audio.prx /mnt/psp/psp/game/psix/modules/audio.prx
	sudo umount /mnt/psp
