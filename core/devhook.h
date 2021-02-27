#ifndef __DH_CFG_H__

// flags value
#define DH_FLAG_FLASH_EMU    0x00000010

// umd
#define DH_FLAG_SFO_VER      0x00000004
#define DH_FLAG_USEUMD       0x00000002
#define DH_FLAG_ANYUMD       0x00000001

// for develop
#define DH_FLAG_UMD_MON      0x00000004

typedef struct devhook_config
{
  char magic[4];          // +000:magic 'DH03'
  int flags;              // +004:2setup flags
  int cpu_Mhz;            // +008:cpu speed
  int bus_Mhz;            // +00c:bus speed
  char rsv[0x30];         // +010:reserved
  char flash[0x40];       // +040:lflash device path
  char umd_path[128];     // +080:UMD ISO path
  char reboot_path[128];  // +100:reboot path(old loadexec hook)
}DEVHOOK_CONFIG;

// devhook common directry
#define DEVHOOK_

// default config data path
extern const char devhook_kd_path[];  // "ms0:/dh/kd/";
extern const char devhook_cfg_path[]; // "ms0:/dh/devhook.ini";

extern DEVHOOK_CONFIG dh_config;

int devhook_load_config(const char *path);
int devhook_save_config(const char *path);

#endif
