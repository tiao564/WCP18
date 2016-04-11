/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver for Pololu High-Power Motor Driver 18v25 (hardware). 
 *    Provides directional control of DC brushed motors. This API
 *    utilizes the 8-bit Timer/Counter2 hardware internal to AVR
 *    microcontrollers to provide a pulse-width modulated (PWM)
 *    signal the hardware motor driver.
 *
 **************************************************************/
 
#include <avr/io.h>
#include <stdint.h>
#include "motor.h"

/*Concatentation Macros*/
#define CONCAT(A,B) A##B
#define DDR(letter) CONCAT(DDR,letter)
#define PORT(letter) CONCAT(PORT,letter)

/*TCCR2A Control Bits*/
#define COM2A1 7
#define COM2A0 6
#define COM2B1 5
#define COM2B0 4
#define WGM21  1
#define WGM20  0

/*TCCR2B Control Bits*/
#define CS22 2
#define CS21 1
#define CS20 0   

/*Mask to clear registers*/
#define CLEAR 0x00

/*Minimum PWM value allowable by motors*/
#define TRANS_SAFETY_PWM 77  // ~30% --> 3.6V
#define ROTAT_SAFETY_PWM 140 // ~55% --> 6.6V

/*See motor.h for additional details*/
void init_motor_drivers(void)
{
	/*Setup rotational motor output ports*/
	DDR(ROTAT_MOTOR_PWM_PORT) |= (1 << ROTAT_MOTOR_PWM_POS);
	DDR(ROTAT_MOTOR_DIR_PORT) |= (1 << ROTAT_MOTOR_DIR_POS);
	/*Setup translational motor output ports*/
	DDR(TRANS_MOTOR_PWM_PORT) |= (1 << TRANS_MOTOR_PWM_POS);
	DDR(TRANS_MOTOR_DIR_PORT) |= (1 << TRANS_MOTOR_DIR_POS);
	/*Enable Fast PWM mode w/ TOP as 0xFF*/
	TCCR2A = ((TCCR2A & CLEAR) | (1 << WGM21) | (1 << WGM20));
	/*Enable divide by 8 prescaler to reduce switching frequency*/
	TCCR2B = ((TCCR2B & CLEAR) | (1 << CS21));
}

/** Rotational Motor API **/

/*See motor.h for additional details*/
void set_rotational_motor_speed(uint8_t speed)
{
	OCR2B = (speed < ROTAT_SAFETY_PWM) ? ROTAT_SAFETY_PWM : speed;
}

/*See motor.h for additional details*/
void rotational_motor_right(void)
{
	PORT(ROTAT_MOTOR_DIR_PORT) &= ~(1 << ROTAT_MOTOR_DIR_POS);
	/*Enable Clear on Compare Match mode*/
	TCCR2A |= (1 << COM2B1); 
}

/*See motor.h for additional details*/
void rotational_motor_left(void)
{
	PORT(ROTAT_MOTOR_DIR_PORT) |= (1 << ROTAT_MOTOR_DIR_POS);
	/*Enable Clear on Compare Match mode*/
	TCCR2A |= (1 << COM2B1);
}

/*See motor.h for additional details*/
void brake_rotational_motor(void)
{
	PORT(ROTAT_MOTOR_DIR_PORT) &= ~(1 << ROTAT_MOTOR_DIR_POS);
	/*Disable PWM to stop motors*/
	TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));
}

/** Translational Motor API **/

/*See motor.h for additional details*/
void set_translational_motor_speed(uint8_t speed)
{
	OCR2A = (speed < TRANS_SAFETY_PWM) ? TRANS_SAFETY_PWM : speed;
}

/*See motor.h for additional details*/
void translational_motor_up(void)
{
	PORT(TRANS_MOTOR_DIR_PORT) &= ~(1 << TRANS_MOTOR_DIR_POS);
	/*Enable Clear on Compare Match mode*/
	TCCR2A |= (1 << COM2A1);
}

/*See motor.h for additional details*/
void translational_motor_down(void)
{
	PORT(TRANS_MOTOR_DIR_PORT) |= (1 << TRANS_MOTOR_DIR_POS);
	/*Enable Clear on Compare Match mode*/
	TCCR2A |= (1 << COM2A1);
}

/*See motor.h for additional details*/
void brake_translational_motor(void)
{
	PORT(TRANS_MOTOR_DIR_PORT) &= ~(1 << TRANS_MOTOR_DIR_POS);
	/*Disable PWM to stop motors*/
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));
}

/*See motor.h for additional details*/
void disable_motors(void)
{
	TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
}
/*End of motor.c*/