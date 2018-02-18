/*
 * UART_control.c
 *
 * Created: 1/12/2018 1:46:44 PM
 *  Author: raghu
 */ 
/*
 * UART_control.c
 *
 * Created: 12/12/2017 8:47:35 AM
 *  Author: raghu
 */ 
#include <asf.h>
#include "UART_control.h"
#include "UART.h"
#include "RGB_LED_control.h"
#include "motor.h"

extern volatile uint8_t ring_buffer[MAX_LIMIT_RING_BUFFER];
extern volatile uint8_t tail_ring_buffer;
extern volatile uint8_t head_ring_buffer;
extern volatile bool received_data_updated;


void serial_receive_bytes(uint8_t count_receive ,uint8_t* receive_data)
{
	uint8_t i =0;
	while((head_ring_buffer - tail_ring_buffer) < count_receive )
	{
		if(serial_timeout)
		{
			//serial_timeout = false;
			return;
		}
	}
	for(i=1;i<=count_receive;i++)
	{
		receive_data[i] = ring_buffer[tail_ring_buffer];
		tail_ring_buffer++;
	}
}


void serial_main_loop_1()
{
	volatile uint8_t received_value[MAX_REC_BYTE];
	volatile static uint8_t prev_rec_value[MAX_REC_BYTE];
	volatile uint8_t transmit_value[MAX_TRANSMIT_VALUE];
	volatile int temp=0;
	static volatile bool broadcast= false;

	//usart_read_job(&usart_instance ,)
	uint8_t i =0;
	if(head_ring_buffer != tail_ring_buffer)
	{
		received_value[0] = ring_buffer[tail_ring_buffer];
		tail_ring_buffer++;
		switch(received_value[0])
		{
			//Set all the actuator values
			case 'A':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(ALL_SET_LEN,received_value);
				if(serial_timeout == false)
				{
					
					update_RGB_LED(0x31, received_value[4] , received_value[5] , received_value[6]);
				
					if(received_value[17] < 128)
					{
						set_motor_left( 0 , received_value[17]);
					}
					else
					{
						set_motor_left( 1, received_value[17]);
					}
					if(received_value[18] < 128)
					{
						set_motor_right( 0 , received_value[18]);
					}
					else
					{
						set_motor_right( 1, received_value[18]);
					}
					
					
					for(i=1;i<=ALL_SET_LEN;i++)
					{
						prev_rec_value[i] = received_value[i];
					}
				}
				else
				{
					serial_timeout_count = 0;
				}
				//job_done = SUCCESS;
				break;
			case 'O':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(ORB_SET_LEN,received_value);
				if(serial_timeout == false)
				{
					update_RGB_LED(received_value[1],received_value[2], received_value[3], received_value[4]);
				}
				break;
			
			case 'M':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(MOTORS_SET_LEN,received_value);
				if(serial_timeout == false)
				{
					/*
					if(received_value[2] == 0 && received_value[4] == 0)
					{
						turn_off_motors();
						break;
					}
					*/
					set_motor_left(received_value[1], received_value[2]); // directions and speed are inputs
					set_motor_right(received_value[3], received_value[4]);
				}
				break;
			case 'G':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(FIRMWARE_SET_LEN,received_value);
				if(serial_timeout == false)
				{
					switch(received_value[1])
					{
						case GET_VERSION_NUMBER :
							transmit_value[0] = HARDWARE_MAJOR_VERSION;
							transmit_value[1] = HARDWARE_MINOR_VERSION;
							
							transmit_value[2] = FIRMWARE_MAJOR_VERSION;
							transmit_value[3] = FIRMWARE_MINOR_VERSION;
							transmit_value[4] = FIRMWARE_REVISION;
							usart_write_buffer_wait(&usart_instance, transmit_value ,VERSION_SET_LEN);
						
					}
				}
				break;
			default:
				head_ring_buffer = 0;
				tail_ring_buffer = 0;
				break;
		}
		
		if((head_ring_buffer == tail_ring_buffer) || (head_ring_buffer > MAX_LIMIT_RING_BUFFER) || (tail_ring_buffer > MAX_LIMIT_RING_BUFFER) || (serial_timeout == true))
		{
			head_ring_buffer = 0;
			tail_ring_buffer = 0;
		}
	 }		
	 
}