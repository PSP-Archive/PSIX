TARGET = psix
APPOBJS= main.o usb.o power.o input.o pbp.o time.o dashboard.o transition.o page.o audio.o image.o file.o sdi/sdi.o sdi/backends/psp/gu.o me.o umd.o audiolib.o osk.o net.o ezxml.o update.o devhook.o

OBJS = $(APPOBJS)

#BUILD_PRX = 1
#USE_PSPSDK_LIBC = 1

INCDIR = 
CFLAGS = -O2 -G0 -Wall -DPSPGU
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS =
LIBS = -lpspgu -lpsppower -lpspusb -lpspwlan -lpspusbstor -lpspaudiolib -lpspaudio -lpspumd -lc -lpng -lz -lm -ljpeg

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSIX Lite/Pro
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG
PSP_EBOOT_PIC0 = PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

install:
	cp EBOOT.PBP /Volumes/Untitled/psp/game/psix

install-15:
	cp -R psix /Volumes/Untitled\ 1/psp/game
	cp -R psix% /Volumes/Untitled\ 1/psp/game	
install-a:
	sudo umount /media/PSP
	sudo mount -t vfat /dev/sdc1 /mnt/PSP
	sudo cp -dr psix /mnt/PSP/psp/game
	sudo cp -dr psix% /mnt/PSP/psp/game
	sudo umount /mnt/PSP

demo:
	tar cjvf psix-demo.tar.bz2 psix psix%
	scp psix-demo.tar.bz2 cameron@agnoster.net:public_html

beta:
	scp EBOOT.PBP cameron@agnoster.net:psix/release/1.0/psix
	scp psix/EBOOT.PBP cameron@agnoster.net:psix/release/1.5/psix
