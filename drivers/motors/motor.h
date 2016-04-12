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
 
#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>

/*Translational Motor Port Locations*/
#define TRANS_MOTOR_PWM_PORT D
#define TRANS_MOTOR_DIR_PORT D
#define TRANS_MOTOR_PWM_POS  7 //OC2A
#define TRANS_MOTOR_DIR_POS  2

/*Rotational Motor Port Locations*/
#define ROTAT_MOTOR_PWM_PORT D
#define ROTAT_MOTOR_DIR_PORT D
#define ROTAT_MOTOR_PWM_POS  6 //OC2B
#define ROTAT_MOTOR_DIR_POS  3

/***************************************************************
 *
 * DESCRIPTION:
 *  - Initializes the 8-bit Timer2 and configures PWM and directional
 *    ports for both the rotational and translational motors as
 *    outputs.
 *
 **************************************************************/
void init_motor_drivers(void);

/* Rotational Motor Functions */

/***************************************************************
 *
 * DESCRIPTION:
 *  - Sets the duty cycle of the rotational motor, thus setting
 *    the motor speed. The "speed" input should be some value
 *    between 140 and 255. If the input value is less than 140,
 *    the speed will default to 140. This ensures the duty cycle
 *    will be large enough to avoid damaging the motor (~55%).
 *
 **************************************************************/
void set_rotational_motor_speed(uint8_t speed);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Turns the rotational motor to the right at the define speed.
 *    This function should always be preceeded by a call to the
 *    brake_rotational_motor() function.
 *
 **************************************************************/
void rotational_motor_right(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Turns the rotational motor to the left at the define speed.
 *    This function should always be preceeded by a call to the
 *    brake_rotational_motor() function.
 *
 **************************************************************/
void rotational_motor_left(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Forces the rotational motor to brake.
 *
 **************************************************************/
void brake_rotational_motor(void);

/* Translational Motor Functions */

/***************************************************************
 *
 * DESCRIPTION:
 *  - Sets the duty cycle of the translational motor, thus setting
 *    the motor speed. The "speed" input should be some value
 *    between 77 and 255. If the input value is less than 77,
 *    the speed will default to 77. This ensures the duty cycle
 *    will be large enough to avoid damaging the motor (~30%).
 *
 **************************************************************/
void set_translational_motor_speed(uint8_t speed);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Turns motor the translational motor at the defined speed
 *    such that the attached object raises. This function should 
 *    always be preceeded by a call to the brake_translational_motor() 
 *    function.
 *
 **************************************************************/
void translational_motor_up(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Turns motor the translational motor at the defined speed
 *    such that the attached object lowers. This function should 
 *    always be preceeded by a call to the brake_translational_motor() 
 *    function.
 *
 **************************************************************/
void translational_motor_down(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Forces the translational motor to brake.
 *
 **************************************************************/
void brake_translational_motor(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Disables use of the motors by turning off Timer/Counter2.
 *
 **************************************************************/
void disable_motors(void);

#endif
/*End of motor.h*/