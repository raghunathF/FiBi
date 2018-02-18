/*
 * UltraSound.h
 *
 * Created: 1/6/2018 6:16:28 PM
 *  Author: raghu
 */ 


#ifndef ULTRASOUND_H_
#define ULTRASOUND_H_

void configure_ultrasound();


extern volatile bool serial_timeout;
extern volatile uint8_t serial_timeout_count;


#define US_ECHO_PIN_EIC  PIN_PA06A_EIC_EXTINT6
#define US_ECHO_PIN_MUX  PINMUX_PA06A_EIC_EXTINT6
#define US_ECHO_EIC_LINE 6

#define MAX_SERIAL_TIMEOUT 250 

#define PIN_OUTPUT_LOW  false
#define PIN_OUTPUT_HIGH true

#define TIME_62_5_MS 18661




#endif /* ULTRASOUND_H_ */