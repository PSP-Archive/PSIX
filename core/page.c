#include "psix.h"

#define PSIX_PAGE_SPEED 4

int psix_page_state;
int psix_page_incoming;
int psix_page_incoming_x;
int psix_page_outgoing;
int psix_page_outgoing_x;

void psix_page_init()
{
	psix_page_state = 0;
	psix_page_incoming = 0;
	psix_page_incoming_x = 0;
	psix_page_outgoing = 0;
	psix_page_outgoing_x = 0;
}

void psix_page_set_page(int state)
{
	if (state != psix_page_state)
	{
		if (psix_page_outgoing)
			switch (psix_page_outgoing)
		{
			case PAGE_AUDIO:
				psix_audio_transition_out(); break;
			case PAGE_VIDEO:
				sdi_set_enabled("page_video",0); break;
			case PAGE_IMAGE:
				psix_image_transition_out(); break;
			case PAGE_FILE:
				psix_file_transition_out(); break;
		}

		psix_page_outgoing = psix_page_state;
		psix_page_incoming = state;
		psix_page_prepare_transition();

		psix_page_state = state;
	}   
}

void psix_page_prepare_transition()
{
	sdi_set_enabled("dashboard_overlay",0);

	switch (psix_page_incoming)
	{
	case PAGE_AUDIO:
		psix_page_incoming_x = -480;
		psix_audio_set_left(psix_page_incoming_x);
		psix_audio_transition_in();
		break; 
	case PAGE_VIDEO:
		psix_page_incoming_x = 480;
		sdi_set_x("page_video",psix_page_incoming_x);
		sdi_set_enabled("page_video",1);
		break;
	case PAGE_IMAGE:
		psix_page_incoming_x = -480;
		psix_image_set_left(psix_page_incoming_x);
		psix_image_transition_in();
		break;
	case PAGE_FILE:
		psix_page_incoming_x = 480;
		psix_file_set_left(psix_page_incoming_x);
		psix_file_transition_in();
		break;
	}

	switch (psix_page_outgoing)
	{
	case PAGE_AUDIO:
		psix_page_outgoing_x = 0;
		break;
	case PAGE_VIDEO:
		psix_page_outgoing_x = 0;
		break;
	case PAGE_IMAGE:
		psix_page_outgoing_x = 0;
		break;
	case PAGE_FILE:
		psix_page_outgoing_x = 0;
		break;
	}
}


void psix_page_update()
{
	if (psix_page_incoming > 0)
	{
		if (psix_page_incoming_x != 0)
		{
			if (psix_page_incoming % 2)
			{
				psix_page_incoming_x += PSIX_PAGE_SPEED;
				if (psix_page_incoming_x > 0)
					psix_page_incoming_x = 0;
			}
			else
			{
				psix_page_incoming_x -= PSIX_PAGE_SPEED;
				if (psix_page_incoming_x < 0)
					psix_page_incoming_x = 0;
			}

			switch (psix_page_incoming)
			{
			case PAGE_AUDIO:
				psix_audio_set_left(psix_page_incoming_x);
				break;
			case PAGE_VIDEO:
				sdi_set_x("page_video",psix_page_incoming_x);
				break;
			case PAGE_IMAGE:
				psix_image_set_left(psix_page_incoming_x);
				break;
			case PAGE_FILE:
				psix_file_set_left(psix_page_incoming_x);
				break;
			}
		}
		else
		{
			psix_page_incoming = 0;
		}
	}

	if (psix_page_outgoing > 0)
	{
		if (psix_page_outgoing % 2)
		{
			if (psix_page_outgoing_x > -480)
			{
				psix_page_outgoing_x -= PSIX_PAGE_SPEED;
				switch (psix_page_outgoing)
				{
				case PAGE_AUDIO:
					psix_audio_set_left(psix_page_outgoing_x);
					break;
				case PAGE_IMAGE:
					psix_image_set_left(psix_page_outgoing_x);
					break;
				}
			}
			else 
			{
				switch(psix_page_outgoing)
				{
				case PAGE_AUDIO:
					psix_audio_transition_out();
					break;
				case PAGE_IMAGE:
					psix_image_transition_out();
					break;
				}
				psix_page_outgoing = 0;
			}
		}
		else
		{
			if (psix_page_outgoing_x < 480)
			{
				psix_page_outgoing_x += PSIX_PAGE_SPEED;
				switch (psix_page_outgoing)
				{
				case PAGE_VIDEO:
					sdi_set_x("page_video",psix_page_outgoing_x);
					break;
				case PAGE_FILE:
					psix_file_set_left(psix_page_outgoing_x);
					break;
				}
			}
			else 
			{
				switch (psix_page_outgoing)
				{
				case PAGE_VIDEO:
					sdi_set_enabled("page_video",0);
					break;
				case PAGE_IMAGE:
					psix_file_transition_out();
					break;
				}
				psix_page_outgoing = 0;
			}
		}
	}
}
