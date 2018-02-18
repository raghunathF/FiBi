/*
 * ultrasound_new.c
 *
 * Created: 6/22/2017 1:32:54 PM
 *  Author: raghu
 */ 
#include <asf.h>
#include "UltraSound.h"

volatile static uint32_t distance_16b=0;
struct tc_module tc_instance2;

void configure_pins()
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);

	// Set trigger pin to output, initialize to trigger pin to zero
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(US_TRIGGER_PIN, &config_port_pin);
	port_pin_set_output_level(US_TRIGGER_PIN, PIN_OUTPUT_LOW);
	
	//echo pin to input
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(US_ECHO_PIN, &config_port_pin);
}

void configure_tc2()
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT; //16
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64; //64
	//config_tc.counter_16_bit.value = 18661; // Overflows in 62.5ms
	config_tc.count_direction = TC_COUNT_DIRECTION_UP;
	tc_init(&tc_instance2, TC2, &config_tc);
	tc_enable(&tc_instance2);
}


void configure_echo_extint()
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin           = US_ECHO_PIN_EIC;
	config_extint_chan.gpio_pin_mux       = US_ECHO_PIN_MUX;
	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_DOWN;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	extint_chan_set_config(US_ECHO_EIC_LINE, &config_extint_chan);
}

void setup_ultrasound()
{
	//Trigger High-Low 100us
	port_pin_set_output_level(US_TRIGGER_PIN, PIN_OUTPUT_HIGH);
	delay_cycles_us(10); 
	port_pin_set_output_level(US_TRIGGER_PIN, PIN_OUTPUT_LOW);
	//Initialize the counter
	tc_set_count_value(&tc_instance2, 0); 
}

void TC4_OV_callback(struct tc_module *const module_inst)
{
	setup_ultrasound();
}


void new_echo_detection_callback(void)
{
	//static uint16_t one_minute_count = 0;
	//static uint8_t low_battery_adc = 0;
	//bool fade_increase = false;
	//uint8_t i =0;
	//one_minute_count = one_minute_count + 1;
	static uint16_t rising_edge = 0;
	serial_timeout_count = serial_timeout_count + 1;
	//count_broadcast = count_broadcast + 1; 
	
	bool pin_state = port_pin_get_input_level(US_ECHO_PIN);
	if(pin_state)
	{
		if(serial_timeout_count > MAX_SERIAL_TIMEOUT)
		{
			serial_timeout = true;
			serial_timeout_count = 0;
		}
		
		/*
		if(one_minute_count > 690)
		{
			one_minute_count = 0;	 //One Minute
			//check_batterylevel_initial();
		}
		*/
		//check_power_button();
		rising_edge = tc_get_count_value(&tc_instance2);
	}
	else
	{
		distance_16b = tc_get_count_value(&tc_instance2) - rising_edge;
		//ready_distance_digital = true;
	}
	
}

char* get_ultrasound_distance()
{
	char* distance_ultrasound= NULL;
	distance_ultrasound= &distance_16b;
	return distance_ultrasound;
}

void configure_TC2_callback()
{
	tc_register_callback(&tc_instance2, TC4_OV_callback,TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance2, TC_CALLBACK_OVERFLOW);
}

void configure_ext_callback()
{
	extint_register_callback(new_echo_detection_callback,US_ECHO_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(US_ECHO_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
}

void configure_callbacks()
{
	configure_TC2_callback();
	configure_ext_callback();
	
}
void configure_modules()
{
	configure_tc2();
	configure_echo_extint();
}

void configure_ultrasound()
{
	configure_pins();
	configure_modules();
	configure_callbacks();
}