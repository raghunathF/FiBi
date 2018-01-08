/*
 * pinout.h
 *
 * Created: 1/6/2018 6:17:49 PM
 *  Author: raghu
 */ 


#ifndef PINOUT_H_
#define PINOUT_H_

//RGB LEDs
#define LEFT_RGB_R_PIN PIN_PA30
#define LEFT_RGB_G_PIN PIN_PA31
#define LEFT_RGB_B_PIN PIN_PA08

//Motor
#define LEFT_MOTOR_CLOCK_PIN	PIN_PA10
#define LEFT_MOTOR_ANTI_PIN		PIN_PA11 
#define RIGHT_MOTOR_CLOCK_PIN   PIN_PA16 
#define RIGHT_MOTOR_ANTI_PIN    PIN_PA17 

//Line Sensor
#define LINE_SENSOR_LED_PIN        PIN_PA27
#define LINE_SENSOR_DETECTOR_PIN   PIN_PA02

//Ultra Sound Sensor
#define US_ECHO_PIN			PIN_PA06
#define US_TRIGGER_PIN		PIN_PA07

//Encoder



#endif /* PINOUT_H_ */