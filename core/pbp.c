#include "psix.h"

#define STRBUFSZ 512

struct PsfSec
{
	u16   label_off;
	u8   rfu001;
	u8   data_type; /* string=2, integer=4, binary=0 */
	u32   datafield_used;
	u32   datafield_size;
	u32   data_off;
};

struct PsfHdr
{
	u8   magic[4];
	u8   rfu000[4];
	u32   label_ptr;
	u32   data_ptr;
	u32   nsects;
}; 

char PBPMagic[] = { 0x00, 0x50, 0x42, 0x50 };
char PsfMagic[] = "\0PSF";
char PsfDefaultFile[] = "PARAM.SFO"; 


typedef struct { char signature[4]; int version; int offset[8]; } HEADER;

char *filename[8] = { "PARAM.SFO", "ICON0.PNG", "ICON1.PMF", "UKNOWN.PNG",
"PIC1.PNG", "SND0.AT3", "UNKNOWN.PSP", "UNKNOWN.PSAR" };

int psix_pbp_read(char *filename, struct psix_pbp_read *pbp_data) 
{
	HEADER header; 
	int loop0, total_size;

	SceIoStat stat;

	sceIoGetstat(filename,&stat);

	int infile = sceIoOpen(filename,PSP_O_RDONLY, 0777);
	if (!infile) return -1;

	total_size = stat.st_size;

	if (sceIoRead(infile, &header, sizeof(HEADER)) < 0) return -1;

	if (memcmp(header.signature,PBPMagic,4))
		return -1;

	for (loop0=0; loop0<8; loop0++) 
	{ 
		void *buffer;
		int size;
		if (loop0 == 7)
			size = total_size - header.offset[loop0];
		else 
			size = header.offset[loop0 + 1] - header.offset[loop0];

		buffer = malloc(size);
		if (!buffer)
			return -1;

		if (sceIoRead(infile,buffer, size) < 0)
			return -1; 

		if (psix_pbp_psf_is(buffer))
		{
			psix_pbp_psf_get_info(buffer,pbp_data->title,pbp_data->region,pbp_data->firmware);
			sceIoClose(infile);
			return 0;
		}

		free(buffer);
	}

	if (sceIoClose(infile) < 0)
		return -1;

	return 0;

}


int psix_pbp_psf_is(void *psf)
{
	if (memcmp(PsfMagic, psf, 4))
		return 0;
	return 1;
}

int psix_pbp_psf_get_info(void *psf,char*title,char*region,char*firmware)
{
	struct PsfHdr   *psfheader;
	struct PsfSec   *psfsections, *sect;
	char   *psflabels;
	char   *psfdata;
	char   strbuf[STRBUFSZ];
	int      dsize, i;  


	if (memcmp(PsfMagic, psf, 4))
		return -1;

	psfheader = (struct PsfHdr*)psf;
	psfsections = (struct PsfSec*)(psf + sizeof(struct PsfHdr));
	psflabels = psf + (psfheader->label_ptr);
	psfdata = psf + (psfheader->data_ptr);

	//psix_draw_stop = 1;

	for (i = 0, sect = psfsections; i < (psfheader->nsects); i++, sect++)
	{
		//pspDebugScreenPrintf("%s\n",&psflabels[(sect->label_off)]);


		if (!strcmp(&psflabels[(sect->label_off)],"TITLE"))
		{
			dsize = (sect->datafield_used) < STRBUFSZ ?
				(sect->datafield_used) : STRBUFSZ-1;
			strncpy(strbuf, &psfdata[(sect->data_off)], dsize);
			strbuf[dsize] = '\0';
			//printf("DATA: %s\n",strbuf);
			strcpy(title,strbuf);
			//return 0;
		}
		else if (!strcmp(&psflabels[(sect->label_off)],"REGION"))
		{
			dsize = (sect->datafield_used) < STRBUFSZ ?
				(sect->datafield_used) : STRBUFSZ-1;
			strncpy(strbuf, &psfdata[(sect->data_off)], dsize);
			strbuf[dsize] = '\0';
			strcpy(region,strbuf);
			//return 0;
		}
		else if (!strcmp(&psflabels[(sect->label_off)],"PSP_SYSTEM_VER"))
		{
			dsize = (sect->datafield_used) < STRBUFSZ ?
				(sect->datafield_used) : STRBUFSZ-1;
			strncpy(strbuf, &psfdata[(sect->data_off)], dsize);
			strbuf[dsize] = '\0';
			strcpy(firmware,strbuf);
			//return 0;
		}
	}
	//sceKernelDelayThread(4000000);

	//psix_draw_stop = 0;
	return 0; 
}
