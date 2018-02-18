/*
 * motor.c
 *
 * Created: 1/6/2018 6:15:25 PM
 *  Author: raghu
 */ 
#include <asf.h>
#include "motor.h"
static struct tcc_module tcc_instance0;
uint8_t old_left_dir = 0;
uint8_t old_right_dir = 0;

void config_timer_waveform()
{
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC0);
	struct port_config config_port_pin;
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	
	//
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV256;
	config_tcc.counter.period = 0x00FF;
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	
	//
	config_tcc.compare.wave_polarity[LEFT_MOTOR_CH]		= TCC_WAVE_POLARITY_0;
	config_tcc.compare.wave_polarity[RIGHT_MOTOR_CH]	= TCC_WAVE_POLARITY_0;
	
	
	//
	config_tcc.compare.match[LEFT_MOTOR_CH]				= 0x00;
	config_tcc.pins.enable_wave_out_pin[LEFT_MOTOR_WO]  = true;
	config_tcc.pins.wave_out_pin[LEFT_MOTOR_WO]         = PIN_PA10F_TCC0_WO2; //this is no longer correct--- should be now
	config_tcc.pins.wave_out_pin_mux[LEFT_MOTOR_WO]     = PINMUX_PA10F_TCC0_WO2; // this is no longer correct --- should be now
	
	
	//
	config_tcc.compare.match[RIGHT_MOTOR_CH]			= 0x00;
	config_tcc.pins.enable_wave_out_pin[RIGHT_MOTOR_WO] = true;
    config_tcc.pins.wave_out_pin[RIGHT_MOTOR_WO]        = PIN_PA17F_TCC0_WO7; // this is no longer correct --- should be now
	config_tcc.pins.wave_out_pin_mux[RIGHT_MOTOR_WO]    = PINMUX_PA17F_TCC0_WO7; // this is no longer correct --- should be now
	
	
	tcc_init(&tcc_instance0, TCC0, &config_tcc);
	tcc_enable(&tcc_instance0);
	
	/*
	port_pin_set_config(PIN_PA11, &config_port_pin);
	port_pin_set_output_level(PIN_PA11, true);
	port_pin_set_config(PIN_PA16, &config_port_pin);
	port_pin_set_output_level(PIN_PA16, true);
	*/
}

void set_motor_left(uint8_t dir, uint8_t speed)
{
	static uint8_t prev_channel = 0; 
	static bool init_left = false;
	speed = 0xFF - speed;
	dir = (~dir) & 0x01 ;
	if((old_left_dir != dir) || (init_left == false))
	{
		init_left = true;
		struct system_pinmux_config pin_config;
		struct port_config config_port_pin;
		config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
		system_pinmux_get_config_defaults(&pin_config);
		pin_config.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
		// need to configure PWM with opposite pins
		if(dir == 0)
		{
			pin_config.mux_position = (uint8_t)PINMUX_PA10F_TCC0_WO2;
			system_pinmux_pin_set_config(PIN_PA10F_TCC0_WO2, &pin_config);
			port_pin_set_config(PIN_PA11, &config_port_pin);
			port_pin_set_output_level(PIN_PA11, true);
			tcc_set_compare_value(&tcc_instance0, LEFT_MOTOR_CH, speed);
			prev_channel = LEFT_MOTOR_CH;
		}
		else
		{
			pin_config.mux_position = (uint8_t)PINMUX_PA11F_TCC0_WO3;
			system_pinmux_pin_set_config(PIN_PA11F_TCC0_WO3, &pin_config);
			port_pin_set_config(PIN_PA10, &config_port_pin);
			port_pin_set_output_level(PIN_PA10, true);
			tcc_set_compare_value(&tcc_instance0, RIGHT_MOTOR_CH, speed);
			prev_channel = RIGHT_MOTOR_CH;
		}
		 
	}
	else
	{
		tcc_set_compare_value(&tcc_instance0, prev_channel, speed);
	}
	
	old_left_dir = dir;
}

// PWM is on AIN1/AIN2 - PA12 and PA20
void set_motor_right(uint8_t dir, uint8_t speed)
{
	static uint8_t prev_channel = 0; 
	static bool init = false;
	speed = 0xFF - speed;
	dir = (~dir) & 0x01 ;
	if((old_right_dir != dir) || (init == false))
	{
		init = true;
		struct system_pinmux_config pin_config;
		struct port_config config_port_pin;
		pin_config.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
		config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
		system_pinmux_get_config_defaults(&pin_config);
		// need to configure PWM with opposite pins
		if(dir == 0)
		{
			pin_config.mux_position = (uint8_t)PINMUX_PA17F_TCC0_WO7;
			system_pinmux_pin_set_config(PIN_PA17F_TCC0_WO7, &pin_config);
			port_pin_set_config(PIN_PA16, &config_port_pin);
			port_pin_set_output_level(PIN_PA16, true);
			tcc_set_compare_value(&tcc_instance0, RIGHT_MOTOR_CH, speed);
			prev_channel = RIGHT_MOTOR_CH;
		}
		else
		{
			pin_config.mux_position = (uint8_t)PINMUX_PA16F_TCC0_WO6;
			system_pinmux_pin_set_config(PIN_PA16F_TCC0_WO6, &pin_config);
			port_pin_set_config(PIN_PA17, &config_port_pin);
			port_pin_set_output_level(PIN_PA17, true);
			tcc_set_compare_value(&tcc_instance0, LEFT_MOTOR_CH, speed);
			prev_channel = LEFT_MOTOR_CH;
			
		}
	}
	else
	{
		tcc_set_compare_value(&tcc_instance0, prev_channel, speed);
	}
	old_right_dir = dir;
}

void turn_off_motors()
{
	// disable the chip to save power -
	set_motor_left(0,0);
	set_motor_right(0,0);
}

void configure_motors()
{
	config_timer_waveform();
}