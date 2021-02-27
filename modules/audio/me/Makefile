TARGET = memp3
OBJS = main.o meboot.o mp3.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = 
LIBS = -lpspaudiolib -lpspaudio -lmad -lc -lm

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = ME MP3

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

install:
	sudo mount /dev/sda1 /mnt/psp
	sudo cp EBOOT.PBP /mnt/psp/psp/game/me
	sudo umount /mnt/psp 