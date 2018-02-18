/*
 * PowerOn.c
 *
 * Created: 1/12/2018 12:44:24 PM
 *  Author: raghu
 */ 
#include <asf.h>
#include "PowerOn.h"
void init_power_pin()
{
	struct port_config power_button_config;
	port_get_config_defaults(&power_button_config);
	power_button_config.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(POWER_ON_PIN,&power_button_config);
	
}
void power_button_on()
{
	port_pin_set_output_level(POWER_ON_PIN, true);
}

void power_off()
{
	port_pin_set_output_level(POWER_ON_PIN, true);
}
/*
void check_power_button()
{
	if()
	{
		count_power_button++;
		if(power_button == POWER_OFF_COUNT)
		{
			
		}
	}
}
*/
void power_button_init()
{
	init_power_pin();
	power_button_on();
}