/*
 * RGB_LED_control.c
 *
 * Created: 1/12/2018 2:04:39 PM
 *  Author: raghu
 */ 
#include <asf.h>
#include "RGB_LED.h"
#include "RGB_LED_control.h"

void update_RGB_LED(uint8_t led_no, uint8_t led_red , uint8_t led_green, uint8_t led_blue )
{
	if(led_no == 0x31)
	{
		temp_compare_array_2[0] = 255-led_red;//Left -- R
		temp_compare_array_2[1] = 255-led_green;//Left  -- G
		temp_compare_array_2[2] = 255-led_blue;//Left  -- B
		initializing_pin_array();
		increasing_sort_tag();
		transfer_temp();
		update_compare_array = true;
	}
	
}