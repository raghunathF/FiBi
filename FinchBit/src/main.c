/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "motor.h"
#include "UltraSound.h"
#include "PowerOn.h"
#include "UART.h"
#include "UART_control.h"
#include "RGB_LED.h"

volatile uint8_t ring_buffer[MAX_LIMIT_RING_BUFFER];
volatile uint8_t tail_ring_buffer=0;
volatile uint8_t head_ring_buffer=0;
volatile bool received_data_updated=false;


volatile uint8_t serial_timeout_count = 0;
volatile bool serial_timeout = false;
volatile uint8_t count_broadcast = 0;

int main (void)
{
	system_init();
	power_button_init();
	delay_init();
	//delay_cycles_ms(4000);
	ORB_init();
	configure_motors();
	configure_ultrasound();
	serial_init();
	enable_USART();
	enable_ORB();
	/* Insert application code here, after the board has been initialized. */
	while(1)
	{ 
		serial_main_loop_1();
	}
}
