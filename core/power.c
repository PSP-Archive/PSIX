#include "psix.h"

int psix_power_charging;
int psix_power_charging_count;
int psix_power_charging_state;
int psix_power_battery_old_state;
int psix_power_battery_old_percent;
int psix_power_battery_old_percent2;
int psix_power_cpu_old_speed;

void psix_power_init()
{
	psix_power_charging = 0;
	psix_power_charging_state = 0;
	psix_power_charging_count = 0;
	psix_power_battery_old_state = 3;
	psix_power_battery_old_percent = -1;
	psix_power_battery_old_percent2 = -1;
	psix_power_cpu_old_speed = -1;

	sdi_new("battery_percent");
	sdi_set_x("battery_percent",18);
	sdi_set_y("battery_percent",0);
	sdi_set_enabled("battery_percent",1);

	sdi_new("cpu_speed");
	sdi_set_x("cpu_speed",61);
	sdi_set_y("cpu_speed",0);
	sdi_set_enabled("cpu_speed",1);
}

void psix_power_update()
{
	int percent = scePowerGetBatteryLifePercent();
	int speed = scePowerGetCpuClockFrequency();

	if (percent > 100 || percent < 0)
		percent = 100;

	if (speed != psix_power_cpu_old_speed)
	{
		psix_power_cpu_old_speed = speed;
		char buf[10];
		sprintf(buf,"%d MHZ",speed);
		sdi_print("cpu_speed","font_gray",buf);
	}

	if (percent != psix_power_battery_old_percent)
	{
		psix_power_battery_old_percent = percent;

		/* update the percentage indicator */
		char buf[5];
		sprintf(buf,"%d%%",percent);
		sdi_print("battery_percent","font_gray",buf);
	}

	if (scePowerIsBatteryCharging())
	{
		psix_power_charging = 1;

		psix_power_charging_count++;

		if (psix_power_charging_count > 1)
		{
			psix_power_charging_count = 0;
			psix_power_charging_state++;
			if (psix_power_charging_state > 9)
				psix_power_charging_state = 0;
		}

		switch (psix_power_charging_state)
		{
		case 9: percent = 95; break;
		case 8: percent = 85; break;
		case 7: percent = 75; break;
		case 6: percent = 65; break;
		case 5: percent = 55; break;
		case 4: percent = 45; break;
		case 3: percent = 35; break;
		case 2: percent = 25; break;
		case 1: percent = 15; break;
		case 0: 
		default: percent = 5; break;
		}
	}
	else
		if (psix_power_charging)
		{
			psix_power_charging = 0;
			psix_power_charging_state = 0;
		}

		if (percent >= 95) percent = 95;
		if (percent < 95 && percent >= 85) percent = 85;
		if (percent < 85 && percent >= 75) percent = 75;
		if (percent < 75 && percent >= 65) percent = 65;
		if (percent < 65 && percent >= 55) percent = 55;
		if (percent < 55 && percent >= 45) percent = 45;
		if (percent < 45 && percent >= 35) percent = 35;
		if (percent < 35 && percent >= 25) percent = 25;
		if (percent < 25 && percent >= 15) percent = 15;
		if (percent < 15) percent = 5;

		if (psix_power_battery_old_percent2 != percent)
		{
			psix_power_battery_old_percent2 = percent;
			sdi_set_enabled("battery_5",percent==5);
			sdi_set_enabled("battery_15",percent==15);
			sdi_set_enabled("battery_25",percent==25);
			sdi_set_enabled("battery_35",percent==35);
			sdi_set_enabled("battery_45",percent==45);
			sdi_set_enabled("battery_55",percent==55);
			sdi_set_enabled("battery_65",percent==65);
			sdi_set_enabled("battery_75",percent==75);
			sdi_set_enabled("battery_85",percent==85);
			sdi_set_enabled("battery_95",percent==95);
		}
}

int psix_power_thread()
{
	while(!psix_done)
	{
		psix_power_update();
		sceKernelDelayThread(50000);
	}
	return 0;
}

SceUID psix_thread_power;
void psix_power_start_thread()
{
	psix_thread_power = sceKernelCreateThread("PSIX Power Thread", psix_power_thread,0x10, 32 * 1024, THREAD_ATTR_USER, NULL);  
	sceKernelStartThread(psix_thread_power, 0, NULL);
}

