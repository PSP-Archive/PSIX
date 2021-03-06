#include "psix.h"

#define PSIX_UPDATE_TABLE_MAX 100
#define PSIX_UPDATE_TABLE_ROWS 13
#define PSIX_UPDATE_TABLE_TOP 56
#define PSIX_UPDATE_TABLE_ROW_HEIGHT 11
#define PSIX_UPDATE_TABLE_FILENAME_LEFT 66
#define PSIX_UPDATE_TABLE_FILENAME_WIDTH 107
#define PSIX_UPDATE_TABLE_STATUS_LEFT 176
#define PSIX_UPDATE_TABLE_STATUS_WIDTH 91
#define PSIX_UPDATE_TABLE_SIZE_LEFT 270
#define PSIX_UPDATE_TABLE_SIZE_WIDTH 68
#define PSIX_UPDATE_TABLE_VERSION_LEFT 341
#define PSIX_UPDATE_TABLE_VERSION_WIDTH 79

#define PSIX_UPDATE_TABLE_SELBAR_TOP 54
#define PSIX_UPDATE_TABLE_SELBAR_LEFT 48
#define PSIX_UPDATE_TABLE_SELBAR_HEIGHT 12


char psix_update_initialized;
char psix_update_table_num;
char psix_update_table_selected;

enum psix_update_table_status_states { STATUS_UNINSTALL,
STATUS_INSTALLED,
STATUS_UPDATE,
STATUS_INSTALL,
STATUS_REPAIR };

struct psix_update_table_item *psix_update_table[PSIX_UPDATE_TABLE_MAX];

void psix_update_init()
{
  int i;
  for (i=0; i < PSIX_UPDATE_TABLE_MAX; i++)
    psix_update_table[i] = 0;
  
  sdi_move_below("net_update_main","page_file");
  sdi_move_below("net_update_selbar","page_file");
  
  psix_update_table_num = 0;
  psix_update_table_selected = 0;
  
  /*  struct psix_update_table_item ppro = { "PSIX Pro",
      "1.69",
      STATUS_UPDATE,
      2343400,
      1 };
      psix_update_table_add(&ppro);
      
      struct psix_update_table_item arkanoid = { "Arkanoid",
      "1.00a",
      STATUS_INSTALL,
      835000,
      1 };
      psix_update_table_add(&arkanoid) ;
      struct psix_update_table_item ds = { "Dual Savoir",
      "0.69",
      STATUS_INSTALLED,
      1453000,
      1 };
      psix_update_table_add(&ds) ;
      struct psix_update_table_item messenger = { "PSP Messenger",
      "1.00",
      STATUS_REPAIR,
      923000,
      1 };
      psix_update_table_add(&messenger) ;
      struct psix_update_table_item psposs = { "PSP-OSS",
      "1.00349",
      STATUS_UNINSTALL,
      13000,
      1 };
      psix_update_table_add(&psposs) ;
  */
  psix_update_table_draw();
}

void psix_update_keypress(u32 pad)
{
  if (pad & PSP_CTRL_DOWN)
    {
      psix_update_table_selected++;
      int max = psix_update_table_num;
      if (max > PSIX_UPDATE_TABLE_ROWS) max = PSIX_UPDATE_TABLE_ROWS;
      if (psix_update_table_selected >= max)
	psix_update_table_selected = max-1;
      psix_update_selection_bar_draw();
    }
  
  if (pad & PSP_CTRL_UP)
    {
      psix_update_table_selected--;
      if (psix_update_table_selected < 0)
	psix_update_table_selected = 0;
      
      psix_update_selection_bar_draw();
    }
  
  if (pad & PSP_CTRL_CROSS)
    {
      psix_draw_stop = 1;
      sceKernelDelayThread(500000);
      pspDebugScreenInit();
      
      psix_net_init(0);
      psix_net_connect("67.15.28.13", 80);
      psix_pdc_getxml();
      psix_pdc_parsexml(0);
      
      //psix_pdc_getversion("ms0:/PSP/GAME/psix/eboot.pbp");
      
      int i;
      for (i=0; i < psix_pdc_getpkgcount(); i++)
	{
	  struct package pkg = psix_pdc_getpkg(i);
	  struct psix_update_table_item pkgrow;
	  
	  strcpy(pkgrow.filename,pkg.name);
	  sprintf(pkgrow.version,"%d.%d.%d",pkg.versions[0]->maj,pkg.versions[0]->min,pkg.versions[0]->rev);
	  pkgrow.size = 100000;
	  pkgrow.status = 0;
	  psix_update_table_add(&pkgrow) ;
	  //sceKernelDelayThread(3000000);
	  //psix_draw_stop = 0;
	  
	}
      
      psix_draw_stop = 0;
      psix_update_table_draw();      
      psix_update_show();	/* force a refresh */
    }
  
}

void psix_update_table_draw()
{
  int i;
  for (i=0; i < PSIX_UPDATE_TABLE_ROWS; i++)
    if (psix_update_table[i])
      {
	char sdi_name[50];
	char font_color[15];
	char status[25];
	int width;
	
	/* draw the filename */
	sprintf(sdi_name,"update_file_%d",i);
	sdi_new(sdi_name);
	sdi_print_wrap_compress(sdi_name,"font_black",psix_update_table[i]->filename,PSIX_UPDATE_TABLE_FILENAME_WIDTH,0);
	sdi_set_position(sdi_name,PSIX_UPDATE_TABLE_FILENAME_LEFT,PSIX_UPDATE_TABLE_TOP+(i*PSIX_UPDATE_TABLE_ROW_HEIGHT));
	sdi_move_below(sdi_name,"page_file");
	sdi_set_overlay(sdi_name,1);
	
	/* draw the status */
	sprintf(sdi_name,"update_status_%d",i);
	sdi_new(sdi_name);
	
	switch(psix_update_table[i]->status)
	  {
	  case STATUS_UNINSTALL:
	    sprintf(font_color,"font_red"); 
	    sprintf(status,"Uninstall"); 
	    break;
	  case STATUS_INSTALLED:
	    sprintf(font_color,"font_black");
	    sprintf(status,"Installed"); 
	    break;
	  case STATUS_UPDATE:
	    sprintf(font_color,"font_green"); 
	    sprintf(status,"Update"); 
	    break;
	  case STATUS_INSTALL:
	    sprintf(font_color,"font_orange"); 
	    sprintf(status,"Install"); 
	    break;
	  case STATUS_REPAIR:
	  default:
	    sprintf(font_color,"font_gray");
	    sprintf(status,"Repair"); 
	  }
	
	sdi_print_wrap_compress(sdi_name,font_color,status,PSIX_UPDATE_TABLE_STATUS_WIDTH,0);
	width = sdi_get_width(sdi_name);
	sdi_set_position(sdi_name,PSIX_UPDATE_TABLE_STATUS_LEFT+(PSIX_UPDATE_TABLE_STATUS_WIDTH-width)/2,PSIX_UPDATE_TABLE_TOP+(i*PSIX_UPDATE_TABLE_ROW_HEIGHT)-1);
	sdi_move_below(sdi_name,"page_file");
	sdi_set_overlay(sdi_name,1);
	
	/* draw the size */
	sprintf(sdi_name,"update_size_%d",i);
	sdi_new(sdi_name);
	char size[14];
	sprintf(size,"%d KB",psix_update_table[i]->size / 1000);
	sdi_print_wrap_compress(sdi_name,"font_black",size,PSIX_UPDATE_TABLE_SIZE_WIDTH,0);
	width = sdi_get_width(sdi_name);
	sdi_set_position(sdi_name,PSIX_UPDATE_TABLE_SIZE_LEFT+(PSIX_UPDATE_TABLE_SIZE_WIDTH-width)/2,PSIX_UPDATE_TABLE_TOP+(i*PSIX_UPDATE_TABLE_ROW_HEIGHT));
	sdi_move_below(sdi_name,"page_file");
	sdi_set_overlay(sdi_name,1);
	
	/* draw the version */
	sprintf(sdi_name,"update_version_%d",i);
	sdi_new(sdi_name);
	sdi_print_wrap_compress(sdi_name,"font_black",psix_update_table[i]->version,PSIX_UPDATE_TABLE_VERSION_WIDTH,0);
	width = sdi_get_width(sdi_name);
	sdi_set_position(sdi_name,PSIX_UPDATE_TABLE_VERSION_LEFT+(PSIX_UPDATE_TABLE_VERSION_WIDTH-width)/2,PSIX_UPDATE_TABLE_TOP+(i*PSIX_UPDATE_TABLE_ROW_HEIGHT));
	sdi_move_below(sdi_name,"page_file");
	sdi_set_overlay(sdi_name,1);
	
      } 
  
  /* draw the selection bar */
  psix_update_selection_bar_draw();
}

void psix_update_selection_bar_draw()
{
  sdi_set_position("net_update_selbar",PSIX_UPDATE_TABLE_SELBAR_LEFT,PSIX_UPDATE_TABLE_SELBAR_TOP+PSIX_UPDATE_TABLE_SELBAR_HEIGHT*psix_update_table_selected - psix_update_table_selected);
}

int psix_update_table_add(struct psix_update_table_item *item)
{
  int i=0;
  while (psix_update_table[i] && i < PSIX_UPDATE_TABLE_MAX) {i++;}
  
  if (i >= PSIX_UPDATE_TABLE_MAX) return -1;
  
  psix_update_table[i] = malloc(sizeof(struct psix_update_table_item));
  memcpy(psix_update_table[i],item,sizeof(struct psix_update_table_item));
  
  psix_update_table_num++;

  return 0;
}

void psix_update_table_set_enabled(int enabled)
{
  int i;
  for (i=0; i < PSIX_UPDATE_TABLE_MAX && i < psix_update_table_num; i++)
    {
      char sdi_name[50];
      sprintf(sdi_name,"update_file_%d",i);
      sdi_set_enabled(sdi_name,enabled);
      sprintf(sdi_name,"update_status_%d",i);
      sdi_set_enabled(sdi_name,enabled);
      sprintf(sdi_name,"update_size_%d",i);
      sdi_set_enabled(sdi_name,enabled);
      sprintf(sdi_name,"update_version_%d",i);
      sdi_set_enabled(sdi_name,enabled);
    }
}

void psix_update_show()
{
  sdi_set_enabled("net_update_main",1);
  sdi_set_enabled("net_update_selbar",1);
  psix_update_table_set_enabled(1);
}

void psix_update_hide()
{
  sdi_set_enabled("net_update_main",0);
  sdi_set_enabled("net_update_selbar",0);
  psix_update_table_set_enabled(0);
}
