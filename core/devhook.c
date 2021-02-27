#include "psix.h"

#define __VSERBOSE__ 0

const char devhook_kd_path[] = "ms0:/dh/kd/";
const char devhook_cfg_path[]    = "ms0:/dh/devhook.ini";

/* static config data */
DEVHOOK_CONFIG dh_config = 
{
  {"DH04"},
  DH_FLAG_ANYUMD , 
  222,
  111,
  {0},     // rsv
  {"ms0:/200"},          // flash name
  {"ms0:/ISO/umd.iso"},  // umd name
  {""}                   // boot path
};

void devhook_set_iso(char *path)
{
  devhook_load_config(devhook_cfg_path);
  strcpy(dh_config.umd_path,path);
  devhook_save_config(devhook_cfg_path);
}


int devhook_load_config(const char *path)
{
  int fd;
  int result;

#if __VSERBOSE__
Kprintf("Load Config File:");
#endif
  result = sceIoAssign("ms0:","msstor0p1:","fatms0:",IOASSIGN_RDWR,NULL,0);

  fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
  if(fd<0)
  {
#if __VSERBOSE__
Kprintf("open err\n");
#endif
    return -1;
  }
  // save : devhook + target + umd + lflash path
  result = sceIoRead(fd, &dh_config, sizeof(dh_config));
  sceIoClose(fd);
	if(result != sizeof(dh_config))
	{
#if __VSERBOSE__
Kprintf("read error\n");
#endif
		return -2;
	}
#if __VSERBOSE__
Kprintf("Loaded\n");
#endif
  return 0;
}

int devhook_save_config(const char *path)
{
  int fd;
  int result;

#if __VSERBOSE__
Kprintf("Save Config File:");
#endif
  result = sceIoAssign("ms0:","msstor0p1:","fatms0:",IOASSIGN_RDWR,NULL,0);
    fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

  if(fd<0)
  {
#if __VSERBOSE__
Kprintf("open err\n");
#endif
    return -1;
  }
  // save : devhook + target + umd + lflash path
  result = sceIoWrite(fd, &dh_config, sizeof(dh_config));
  sceIoClose(fd);
	if(result != sizeof(dh_config))
	{
#if __VSERBOSE__
Kprintf("write error\n");
#endif
		return -2;
	}
#if __VSERBOSE__
Kprintf("Saved\n");
#endif
  return 0;
}

