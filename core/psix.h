#ifndef __PSIX_H
#define __PSIX_H

#define PSIX_VERSION "1.9"

/* sdi */
#include "sdi/sdi.h"

#include "page.h"
#include "input.h"
#include "usb.h"
#include "power.h"
#include "pbp.h"
#include "dashboard.h"
#include "transition.h"
#include "time_.h"
#include "me.h"
#include "audio.h"
#include "image.h"
#include "umd.h"
#include "file.h"
#include "osk.h"
#include "update.h"
#include "devhook.h"

/* codecs */
#include "codec/mp3/mp3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <malloc.h>
#include <mad.h>
#ifndef PC
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspaudiolib.h>
#include <psprtc.h>
#include <pspiofilemgr.h>
#include <pspumd.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <psppower.h>
#include <pspiofilemgr.h>
#include <pspiofilemgr_fcntl.h>
#include <pspsdk.h>
#include <pspsysmem.h>
#include <pspwlan.h>
#include <pspnet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <pspnet_inet.h>
#include <psputility_netconf.h>
#include <psputility_netparam.h>
#endif

#include "net.h"

#define printf  pspDebugScreenPrintf

extern int psix_done;
extern char *psix_launch_path;
extern struct SceKernelLoadExecParam psix_launch_param;
extern char psix_path[256];
extern char psix_eboot[256];
extern int psix_draw_stop;
extern unsigned char *psix_mac;

void psix_shutdown();

#endif
