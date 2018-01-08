/*
 * RGB_LED.c
 *
 * Created: 1/6/2018 6:13:20 PM
 *  Author: raghu
 */ 
/*
 * ORB.c
 *
 * Created: 12/6/2017 11:20:25 AM
 *  Author: raghu
 */ 

#include <asf.h>
#include "RGB_LED.h"

struct tc_module orb_tc_instance;

uint8_t compare_array[TOTAL_LEDS];
uint8_t compare_array_ID;

uint8_t pin_array[TOTAL_LEDS];
uint8_t pin_array_ID;

uint8_t temp_compare_array_2[TOTAL_LEDS];
uint8_t temp_compare_array[TOTAL_LEDS];
uint8_t temp_pin_array[TOTAL_LEDS];

bool update_compare_array = false;

static uint8_t N_valid_compares = TOTAL_LEDS;

#define CLEAR_ORB 0xC0000080 //8,30,31


void increasing_sort_tag()
{
	uint8_t i,j ,temp;
	transfer_temp_2();
	for(i=0; i< TOTAL_LEDS-1 ;i++)
	{
		for(j=0;j<TOTAL_LEDS-i-1;j++)
		{
			if(temp_compare_array[j]>temp_compare_array[j+1])
			{
				temp = temp_compare_array[j];
				temp_compare_array[j] = temp_compare_array[j+1];
				temp_compare_array[j+1]= temp;
				
				temp = temp_pin_array[j];
				temp_pin_array[j] = temp_pin_array[j+1];
				temp_pin_array[j+1] = temp;
				
			}
		}
	}
}

void ORB_leds_off()
{
	//Switch off LEDs single statement
	PortGroup *const port_base = port_get_group_from_gpio_pin(LEFT_RGB_R_PIN);
	port_base->OUTCLR.reg = CLEAR_ORB ;
	
}

void transfer_temp()
{
	uint8_t i;
	N_valid_compares = 0;
	for(i=0;i<TOTAL_LEDS;i++)
	{
		if(temp_compare_array[i] != 255)
		{
			N_valid_compares++;
		}
		compare_array[i] = temp_compare_array[i] ;
		pin_array[i]  = temp_pin_array[i];
	}
}

void transfer_temp_2()
{
	uint8_t i;
	for(i=0;i<TOTAL_LEDS;i++)
	{
		temp_compare_array[i] = temp_compare_array_2[i] ;
	}
}

void ORB_timer_init()
{
	struct tc_config orb_tc_config;
	tc_get_config_defaults(&orb_tc_config);
	//rgb_led_config.clock_source = GCLK_GENERATOR_1;
	orb_tc_config.clock_prescaler = TC_CLOCK_PRESCALER_DIV256;
	orb_tc_config.counter_size = TC_COUNTER_SIZE_8BIT;
	orb_tc_config.counter_8_bit.period = 0XFF;
	orb_tc_config.counter_8_bit.compare_capture_channel[0] = 0;
	
	tc_init(&orb_tc_instance, TC1, &orb_tc_config);
	tc_enable(&orb_tc_instance);
}

void set_drivestrength_ORB()
{
	PortGroup *const port_base = port_get_group_from_gpio_pin(LEFT_RGB_R_PIN);
	system_pinmux_group_set_output_strength(port_base,CLEAR_ORB,SYSTEM_PINMUX_PIN_STRENGTH_HIGH);
}

void ORB_setup_pins()
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LEFT_RGB_R_PIN, &config_port_pin);
	port_pin_set_config(LEFT_RGB_G_PIN, &config_port_pin);
	port_pin_set_config(LEFT_RGB_B_PIN, &config_port_pin);
	set_drivestrength_ORB();
	ORB_leds_off();
	
}

void tc_callback_OF(struct tc_module *const module_inst)
{
	uint8_t compare_value=0;
	ORB_leds_off();
	//Clear interrupts
	//tc_clear_interrupts(&tc_instance);
	
	if(update_compare_array == true)
	{
		tc_enable_callback(&orb_tc_instance, TC_CALLBACK_CC_CHANNEL0);
		transfer_temp();
		update_compare_array = false;
	}
	compare_array_ID = 0;
	pin_array_ID  = 0;
	
	compare_value = compare_array[0];
	if(compare_value != 255)
	{
		tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, compare_value);
	}
	else
	{
		tc_disable_callback(&orb_tc_instance, TC_CALLBACK_CC_CHANNEL0);
	}
	//tc_set_inital_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, 0);
}

void tc_callback_PWM(struct tc_module *const module_inst)
{
	static uint8_t compare_value=0;
	static uint8_t compare_value_last=0;
	static uint8_t compare_value_current=0;
	static bool first_time = true;
	volatile uint32_t count;
	
	if(first_time == false)
	{
		port_pin_set_output_level(pin_array[pin_array_ID++], RGB_ON);
		
		if(compare_array_ID < N_valid_compares-1 )
		{
			compare_value_last = compare_array[compare_array_ID];
			compare_value_current = compare_array[++compare_array_ID];
			while(compare_value_last == compare_value_current && compare_array_ID <= N_valid_compares - 1)
			{
				//Enable the LED
				port_pin_set_output_level(pin_array[pin_array_ID++],RGB_ON);
				compare_value_last = compare_array[compare_array_ID];
				compare_value_current = compare_array[++compare_array_ID];
			}
			if(compare_value_current != 255)
			{
				tc_set_count_value(module_inst, compare_value_last);
				tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, compare_value_current);
			}
			else
			{
				tc_set_count_value(module_inst, compare_value_last);
				tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, 0);
			}
		}
		else
		{
			tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, 0);
		}
		
	}
	else
	{
		first_time = false;
		tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, 0);
	}
}

void ORB_timer_callbacks_init()
{
	tc_register_callback(&orb_tc_instance, tc_callback_OF,TC_CALLBACK_OVERFLOW);
	tc_register_callback(&orb_tc_instance, tc_callback_PWM,TC_CALLBACK_CC_CHANNEL0);
}

void enable_ORB()
{
	tc_enable_callback(&orb_tc_instance, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&orb_tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

void disable_ORB()
{
	//clear all RGB LEDS
	ORB_leds_off();
	tc_disable_callback(&orb_tc_instance, TC_CALLBACK_OVERFLOW);
	tc_disable_callback(&orb_tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

void initializing_pin_array()
{
	temp_pin_array[0]  = LEFT_RGB_R_PIN;
	temp_pin_array[1]  = LEFT_RGB_G_PIN;
	temp_pin_array[2]  = LEFT_RGB_B_PIN;
}

void initializing_compare_array()
{
	temp_compare_array_2[0] = 255;//Left -- R
	temp_compare_array_2[1] = 255;//Left  -- G
	temp_compare_array_2[2] = 255;//Left  -- B
}


void ORB_init_array()
{
	initializing_pin_array();
	initializing_compare_array();
	transfer_temp_2();
	increasing_sort_tag();
	transfer_temp();
}

void ORB_init()
{
	//Timer Initialization
	ORB_timer_init();
	ORB_setup_pins();
	ORB_init_array();
	//Initialize the timer callbacks
	ORB_timer_callbacks_init();

}