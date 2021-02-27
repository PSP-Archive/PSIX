#include "psix.h"

#define OSK_WIDTH 128
#define OSK_LEFT 480-OSK_WIDTH
#define OSK_SELECT_LEFT 3
#define OSK_SELECT_TOP 62
#define OSK_SELECT_WIDTH 31
#define OSK_SELECT_HEIGHT 15
#define OSK_TEXT_LEFT 7
#define OSK_TEXT_WIDTH 117
#define OSK_TEXT_TOP 124
#define OSK_TAB_LEFT 46
#define OSK_TAB_WIDTH 80
#define OSK_TEXTMAP_LEFT  5
#define OSK_TEXTMAP_WIDTH 121

char psix_osk_visible;
char psix_osk_row;
char psix_osk_col;
char psix_osk_text[OSK_TEXT_MAX];

char psix_osk_case;

enum psix_osk_text_case {LOWERCASE=0,UPPERCASE};

void psix_osk_init()
{
	psix_osk_visible = 0;
	psix_osk_row = 1;
	psix_osk_col = 1;
	psix_osk_case = LOWERCASE;
}

void psix_osk_keypress(u32 pad)
{
  if (pad & (PSP_CTRL_SELECT|PSP_CTRL_START))
    psix_osk_hide();
  
  if (pad & PSP_CTRL_UP)
    psix_osk_decrement_row();
  
  if (pad & PSP_CTRL_DOWN)
    psix_osk_increment_row();
  
  if (pad & PSP_CTRL_RIGHT)
		psix_osk_increment_col();
  
  if (pad & PSP_CTRL_LEFT)
    psix_osk_decrement_col();
  
  if (pad & (PSP_CTRL_SQUARE | PSP_CTRL_CROSS | PSP_CTRL_CIRCLE))
    psix_osk_char_selected(pad);
  
  if (pad & PSP_CTRL_TRIANGLE)
    psix_osk_char_add(0);

  if (pad & PSP_CTRL_LTRIGGER || pad & PSP_CTRL_RTRIGGER)
    {
      psix_osk_case = !psix_osk_case;
      if (psix_osk_case == LOWERCASE)
	{
	  sdi_set_enabled("osk_text_lower",1);
	  sdi_set_enabled("osk_text_upper",0);
	  sdi_set_enabled("osk_tab_lower",1);
	  sdi_set_enabled("osk_tab_upper",0);
	}
      else
	{
	  sdi_set_enabled("osk_text_lower",0);
	  sdi_set_enabled("osk_text_upper",1);
	  sdi_set_enabled("osk_tab_lower",0);
	  sdi_set_enabled("osk_tab_upper",1);
	}
    }
 

}

void psix_osk_char_selected(u32 pad)
{
	char table[2][4][4][3]={
	  {{{'1','2','3'},{'4','5','6'},{'7','8','9'},{'0','_',' '}},
	  {{'q','w','e'},{'r','t','y'},{'u','i','o'},{'p','[',']'}},
	  {{'a','s','d'},{'f','g','h'},{'j','k','l'},{':',';','@'}},
	   {{'z','x','c'},{'v','b','n'},{'m',',','.'},{'?','>','<'}}},

	  {{{'!','@','#'},{'$','%','^'},{'&','*','('},{')','-','+'}},
	  {{'Q','W','E'},{'R','T','Y'},{'U','I','O'},{'P','{','}'}},
	  {{'A','S','D'},{'F','G','H'},{'J','K','L'},{'=','\'','#'}},
	   {{'Z','X','C'},{'V','B','N'},{'M','/','\\'},{'~','`','-'}}}
	};

	if (psix_osk_row < 1 || psix_osk_row > 4 || 
		psix_osk_col < 1 || psix_osk_col > 4)
		return;

	if (pad & PSP_CTRL_SQUARE)
		psix_osk_char_add(table[(int)psix_osk_case][psix_osk_row-1][psix_osk_col-1][0]);
	if (pad & PSP_CTRL_CROSS)
		psix_osk_char_add(table[(int)psix_osk_case][psix_osk_row-1][psix_osk_col-1][1]);
	if (pad & PSP_CTRL_CIRCLE)
		psix_osk_char_add(table[(int)psix_osk_case][psix_osk_row-1][psix_osk_col-1][2]);

}

void psix_osk_char_add(char c)
{
	if (strlen(psix_osk_text) >= OSK_TEXT_MAX-1)
		return;

	if (c == 0)
	{
		if (strlen(psix_osk_text) > 0)
			psix_osk_text[strlen(psix_osk_text)-1] = 0;
	}
	else
		psix_osk_text[strlen(psix_osk_text)] = c;

	psix_osk_text_update();
}

void psix_osk_text_update()
{
  sdi_clear("osk_text");
  sdi_print_line_max_compress("osk_text","font_black",psix_osk_text,OSK_TEXT_WIDTH,1,0);
  int textwidth = sdi_get_width("osk_text");	
  sdi_set_x("osk_carrot",480-OSK_WIDTH+OSK_TEXT_LEFT+textwidth);
}

void psix_osk_decrement_row()
{  
	psix_osk_row--;
	if (psix_osk_row < 1)
		psix_osk_row = 4;
	psix_osk_update_select();
}

void psix_osk_increment_row()
{  
	psix_osk_row++;
	if (psix_osk_row > 4)
		psix_osk_row = 1;
	psix_osk_update_select();
}

void psix_osk_decrement_col()
{  
	psix_osk_col--;
	if (psix_osk_col < 1)
		psix_osk_col = 4;
	psix_osk_update_select();
}

void psix_osk_increment_col()
{  
	psix_osk_col++;
	if (psix_osk_col > 4)
		psix_osk_col = 1;
	psix_osk_update_select();
}

void psix_osk_update_select()
{
	int y_extra=0;
	switch (psix_osk_row)
	{
	case 1: y_extra = 0; break;
	case 2: y_extra = -3; break;
	case 3: y_extra = -6; break;
	case 4: y_extra = -8; break;
	}

	sdi_set_y("osk_select",(psix_osk_row-1)*OSK_SELECT_HEIGHT+OSK_SELECT_TOP+y_extra);
	sdi_set_x("osk_select",(psix_osk_col-1)*OSK_SELECT_WIDTH+OSK_SELECT_LEFT+OSK_LEFT-psix_osk_col+1);
}

u32 lastpad;
int psix_osk_get_string(char *initial)
{ 
  SceCtrlData pd;
  u32 pad;
  lastpad = 0;
  
  if (!psix_osk_visible)
    {
      int osk_left = 480;
      sdi_set_x("osk",osk_left);
      sdi_set_x("osk_select",osk_left + OSK_SELECT_LEFT);
      sdi_set_x("osk_text",osk_left + OSK_TEXT_LEFT);
      sdi_set_x("osk_text_lower",osk_left+OSK_TEXTMAP_LEFT);
      sdi_set_x("osk_tab_lower",osk_left+OSK_TAB_LEFT);
      sdi_set_x("osk_text_upper",osk_left+OSK_TEXTMAP_LEFT);
      sdi_set_x("osk_tab_upper",osk_left+OSK_TAB_LEFT);


      psix_osk_visible = 1;
      psix_osk_row = 1;
      psix_osk_col = 1;
      sdi_set_enabled("osk",1);
      sdi_set_y("osk_select",OSK_SELECT_TOP);
      sdi_set_enabled("osk_select",1);
      sdi_new("osk_text");
      sdi_create_image("osk_text",148,12);
      sdi_set_enabled("osk_text",1);
      sdi_set_overlay("osk_text",1);
      sdi_set_y("osk_text",OSK_TEXT_TOP);
      memset(psix_osk_text,0,OSK_TEXT_MAX);
      strcpy(psix_osk_text,initial);

      
      //psix_draw_stop = 1; 
      /* hack alert! dont remove this delay */
      sceKernelDelayThread(200000);
      //pspDebugScreenInit();
      //printf("initial: %s\n",initial);
      //printf("psix_osk_text: %s\n",psix_osk_text);
      //sceKernelDelayThread(5000000);
      //psix_draw_stop = 0;
      

      if (psix_osk_case == LOWERCASE)
	{
	  sdi_set_enabled("osk_text_lower",1);
	  sdi_set_enabled("osk_tab_lower",1);
	}
      else
	{
	  sdi_set_enabled("osk_text_upper",1);
	  sdi_set_enabled("osk_tab_upper",1);
	}
  

      
      psix_osk_text_update();
      int textwidth = sdi_get_width("osk_text");
      sdi_set_x("osk_carrot",osk_left+OSK_TEXT_LEFT+textwidth);
      sdi_set_enabled("osk_carrot",1);



      psix_osk_transition_in();
      
      while (psix_osk_visible)
	{
	  sceCtrlPeekBufferPositive(&pd, 1); 
	  pad = pd.Buttons & ~lastpad;
	  if (pad)
	    psix_osk_keypress(pad);
	  sceKernelDelayThread(1000);
	  lastpad = pd.Buttons;
	} 
    }
  return pad & PSP_CTRL_START;
}

void psix_osk_hide()
{
	psix_osk_visible = 0;
	sdi_set_enabled("osk",0);
	sdi_set_enabled("osk_select",0);
	sdi_del("osk_text");
	sdi_set_enabled("osk_text_lower",0);
	sdi_set_enabled("osk_tab_lower",0);
	sdi_set_enabled("osk_text_upper",0);
	sdi_set_enabled("osk_tab_upper",0);
	sdi_set_enabled("osk_carrot",0);
}

void psix_osk_transition_in()
{
	psix_osk_start_transition_thread();
}

int psix_osk_transition_thread()
{
	int osk_left = 480;
	int step_size = -1;
	int textwidth = sdi_get_width("osk_text");

	while(osk_left + OSK_WIDTH > 480)
	{
		osk_left += step_size;
		if (osk_left + OSK_WIDTH < 480) osk_left = 480 - OSK_WIDTH;
		sdi_set_x("osk",osk_left);
		sdi_set_x("osk_select",osk_left+OSK_SELECT_LEFT);
		sdi_set_x("osk_text",osk_left+OSK_TEXT_LEFT);
		sdi_set_x("osk_carrot",osk_left+OSK_TEXT_LEFT+textwidth);
		sdi_set_x("osk_text_lower",osk_left+OSK_TEXTMAP_LEFT);
		sdi_set_x("osk_tab_lower",osk_left+OSK_TAB_LEFT);
		sdi_set_x("osk_text_upper",osk_left+OSK_TEXTMAP_LEFT);
		sdi_set_x("osk_tab_upper",osk_left+OSK_TAB_LEFT);

		sceKernelDelayThread(1000);
	}

	sceKernelExitDeleteThread(0);
	return 0;
}

SceUID psix_thread_osk_transition;
void psix_osk_start_transition_thread()
{
	psix_thread_osk_transition = sceKernelCreateThread("PSIX OSK Transition Thread", psix_osk_transition_thread,0x10, 128 * 1024, THREAD_ATTR_USER, NULL);  
	sceKernelStartThread(psix_thread_osk_transition, 0, NULL);
}
