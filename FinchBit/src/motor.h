/*
 * motor.h
 *
 * Created: 1/6/2018 6:15:42 PM
 *  Author: raghu
 */ 

#ifndef MOTOR_H_
#define MOTOR_H_

#define LEFT_MOTOR_CH 2
#define LEFT_MOTOR_WO 2

#define RIGHT_MOTOR_CH 3
#define RIGHT_MOTOR_WO 7


void configure_motors();
void set_motor_left(uint8_t dir, uint8_t speed);
void set_motor_right(uint8_t dir, uint8_t speed);
void turn_off_motors();

#endif /* MOTOR_H_ */