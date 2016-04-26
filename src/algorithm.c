//Logan Friedman
//
//Description:
//Function descriptions of soil sampling algorithm


#include "algorithm.h"
#include "accelerometer.h"
#include "system_ctl.h"
#include "encoder.h"
#include "motor.h"
#include "ultrasonic.h"
#include "vibration.h"
#include "gyroscope.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//Look at algorithm.h for function descriptions
void stop_motors(void)
{
	//Brake motors
	stop_encoders();
	brake_rotational_motor();
	brake_translational_motor();
}


void init_pins_check_enable(void)
{
	//Set input and output pins for enable and leds respectively
	init_system_cntl();
	DDRA |= (1 << LED_MOTOR_POS);
	DDRA |= (1 << LED_COMPLETE_POS);
	DDRA |= (1 << LED_ERROR_POS);

	//busy wait for enable signal
	while(get_sys_cntl_state() != SYS_ENABLE_STATE)
	{
	}
	PORTA |= (1 << LED_MOTOR_POS);
}

bool initialize(void)
{
	sei();

	//*******IMU Board Initialization****
	//initialize IMU board
	bool accel_code = init_accel();
	bool gyro_code = init_gyro(RANGE_2000_DPS);
	disable_autorange();
	//Make sure IMU initialization was successful
	if(accel_code == ACCEL_INIT_FAIL || gyro_code == GYRO_INIT_FAIL)
		return INIT_FAILED;
	
	//****Sensor Initialization******
	init_ultrasonic_sensors();
	init_motor_drivers();
	init_vibration_sensors();
	init_encoders();

	//****Brake Motors and Set Motor Speeds
	stop_motors();
	set_rotational_motor_speed(DUTY_CYCLE);
	set_translational_motor_speed(DUTY_CYCLE);
	return INIT_SUCCESS;
}

void disable(bool code)
{
	PORTA &= ~(1 << LED_MOTOR_POS);
	//Turn on Error LED or Complete LED depending on exit code
	if(code == ERROR)
	{
		PORTA &= ~(1 << LED_ERROR_POS);
	}
	else
	{
		PORTA &= ~(1 << LED_COMPLETE_POS);
	}

	//Turn off encoders,motors, sensors
	clear_encoders();
	stop_encoders();
	disable_motors();
	disable_vibration_sensors();
}

bool check_obstacle_sensors(void)
{
	accum distance_A,distance_B;
	//Get distances of the two ultrasonic sensors
	distance_A = get_obstacle_distance_cm(A);
	distance_B = get_obstacle_distance_cm(B);

	//If either sensors timesout or is not in range, return a 0. If both in range, then return a 1
	if((distance_A == ULTRASONIC_TIMEOUT || distance_B == ULTRASONIC_TIMEOUT) || (distance_A < LOWER_US_LIMIT) || (distance_A > UPPER_US_LIMIT) || (distance_B < LOWER_US_LIMIT) || (distance_B > UPPER_US_LIMIT))
		return OBSTACLE_SENSOR_FAIL;
	return OBSTACLE_SENSOR_SUCCESS;
}

void start_motors_down(void)
{
	//start motor turning right and going down
	//start_encoders();
	rotational_motor_right();
	translational_motor_down();
	
}

void start_motors_up(void)
{
	//start motor turning left and going up
	//start_encoders();
	rotational_motor_left();
	translational_motor_up();
}
bool check_accel(void)
{
	//Check accelerometer and make sure data was read and all data is in range
	accel_data data;
	data.x = 0;
	data.y = 0;
	data.z = 0;
	bool status = read_accel(&data);
	if((status == ACCEL_READ_FAIL) || (data.x < AX_LOWER) || (data.x > AX_UPPER) || (data.y < AY_LOWER) || (data.y > AY_UPPER) || (data.z < AZ_LOWER) || (data.z > AZ_UPPER))
	{
		return ERROR;
	}
	return COMPLETE;
}

bool check_gyro(void)
{
	//Check gyro and make sure data was read and  all data is in range
	gyro_data data;
	data.x = 0;
	data.y = 0;
	data.z = 0;
	bool status = read_gyroscope(&data);
	if((status == GYRO_READ_FAIL) || (data.x < GY_LOWER) || (data.x > GY_UPPER) || (data.y < GY_LOWER) || (data.y > GY_UPPER) || (data.z < GZ_LOWER) || (data.z > GZ_UPPER))
	{
		return ERROR;
	}
	return COMPLETE;
}

bool check_encoders(uint16_t prev_rotat,uint16_t rotat,uint16_t prev_trans,uint16_t trans)
{
	//Checks that motors are moving, returns 0 if no movement, 1 otherwise
	if((prev_rotat == rotat) || (prev_trans == trans))
	{
		return ERROR;
	}
	return COMPLETE;
}

bool check_encoder_sensors(uint16_t prev_rotat,uint16_t rotat,uint16_t prev_trans,uint16_t trans)
{
	//takes info from other checks and gives an error if any encoder or sensor fails 
	bool encoder_chk = check_encoders(prev_rotat,rotat,prev_trans,trans);
	if((encoder_chk == ERROR) || (check_obstacle_sensors() == OBSTACLE_SENSOR_FAIL) || (check_gyro() == ERROR) || check_vs_med_a() || check_vs_med_b() || check_vs_slow_a() || check_vs_slow_b() || (check_accel() == ERROR))
		return ERROR;
	return COMPLETE;
}

void clear_encoders(void)
{
	//Clear the encoder counts
	clear_trans_encoder_cnt();
	clear_rotat_encoder_cnt();
}
