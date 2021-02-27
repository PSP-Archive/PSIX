#include "psix.h"

/* prints text to the screen */
void psix_print(int dx, int dy, char *font_name, char *str)
{
	/* where the letter is on the font image */
	int x,y,h,w;

	/* print the chars one by one */
	while (*str)
	{
		char c = *str;
		if (c >= 'a' && c <= 'z')
		{
			int map[26][4] = { {0,0,12,6},
			{7,0,12,6},
			{13,0,12,5},
			{19,0,12,6},
			{25,0,12,6},
			{32,0,12,4},
			{37,0,12,6},
			{43,0,12,6},
			{50,0,12,3},
			{53,0,12,4},
			{58,0,12,6},
			{65,0,12,3},
			{68,0,12,9},
			{78,0,12,6},
			{84,0,12,6},
			{91,0,12,6},
			{98,0,12,6},
			{104,0,12,5},
			{110,0,12,5},
			{115,0,12,5},
			{121,0,12,6},
			{128,0,12,6},
			{134,0,12,7},
			{142,0,12,6},
			{148,0,12,6},
			{155,0,12,5} };
			x = map[c-'a'][0];
			y = map[c-'a'][1];
			w = map[c-'a'][2];
			h = map[c-'a'][3];
		}
		else if (c >= 'A' && c <= 'Z')
		{
			int map[26][4] = { {161,0,12,6},
			{167,0,12,6},
			{174,0,12,6},
			{180,0,12,6},
			{187,0,12,5},
			{193,0,12,5},
			{198,0,12,6},
			{205,0,12,6},
			{211,0,12,3},
			{215,0,12,6},
			{222,0,12,6},
			{228,0,12,5},
			{234,0,12,7},
			{241,0,12,6},
			{248,0,12,6},
			{255,0,12,6},
			{261,0,12,6},
			{268,0,12,6},
			{274,0,12,6},
			{281,0,12,7},
			{289,0,12,6},
			{295,0,12,7},
			{303,0,12,7},
			{310,0,12,6},
			{317,0,12,7},
			{325,0,12,5} };
			x = map[c-'A'][0];
			y = map[c-'A'][1];
			w = map[c-'A'][2];
			h = map[c-'A'][3];
		}       
		else if (c >= '0' && c <= '9')
		{
			int map[10][4] = { {330,0,12,6},
			{337,0,12,5},
			{342,0,12,6},
			{349,0,12,6},
			{356,0,12,7},
			{363,0,12,6},
			{370,0,12,6},
			{376,0,12,6},
			{383,0,12,6},
			{390,0,12,6} };
			x = map[c-'0'][0];
			y = map[c-'0'][1];
			w = map[c-'0'][2];
			h = map[c-'0'][3];
		}
		else
		{/*
		 switch (c)
		 {
		 case '!':
		 case '"': 
		 case '?':
		 case '$':
		 case '%':
		 case '^':
		 case '&':
		 case '*':
		 case '_':
		 case '+':
		 case '-':
		 case '=':
		 case ';':
		 case ':':
		 case '\'':
		 case '@':
		 case '`':
		 case '#':
		 case '~':
		 case '<':
		 case '>':
		 case ',':
		 case '.':
		 case '/':
		 case '\\':
		 case '?':
		 case '|':
		 case '(':
		 case ')':
		 case '[':
		 case ']':
		 case '{':
		 case '}':
		 case ' ': default:

		 x = y = w = h = 0;
		 }*/
		}

		/* now we know where the image is...blit it */
		sdi_blit_part(x,y,w,h,dx,dy,font_name);

		dx += x;

		str++;
	}

}