//Logan Friedman
//
//Description:
//Function descriptions of soil sampling algorithm


#include "i2c_lib.h"
#include "algorithm.h"
#include "accelerometer_driver.h"
#include "encoder.h"
#include "motor.h"
#include "ultrasonic.h"
#include "vibration.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

//Look at algorithm.h for function descriptions
void stop_motors(void)
{
	brake_rotational_motor();
	brake_translational_motor();
}

void check_enable(void)
{
	int enable = 1;
	//SET CORRECT PIN/PORTS FOR ENABLE SIGNAL
	while(1)
	{
		if(enable == 1)
		{
			break;
		}
	}
}

bool initialize(void)
{
	gyro_range rng;
	bool accel_code = init_accel();
	bool gyro_code = init_gyro(rng);
	if(accel_code == ACCEL_INIT_FAILED || gyro_code == GYRO_INIT_FAIL))
		return INIT_FAILED;
	init_ultrasonic_sensors();
	init_motor_drivers();
	init_vibration_sensors();
	stop_motors();
	set_rotational_motor_speed(DUTY_CYCLE);
	set_translational_motor_speed(DUTY_CYCLE);
	enable_autorange();
	return INIT_SUCCESS;
}

void send_signal_and_disable(bool code)
{
	/*
	   if(code == ERROR)
	{
		//SET ERROR CONDITION TO ALERT USER
	}
	//Successful Dig
	else
	{
		//SEND COMPLETION TO USER
	}
	*/
	stop_clear_encoders();
	disable_motors();
	disable_autorange();
	disable_vibration_sensors();
}

bool check_obstacle_sensors(void)
{
	accum distance_A,distance_B;
	distance_A = get_obstacle_distance_cm(A);
	distance_B = get_obstacle_distance_cm(B);
	if(((distance_A < UPPER_us_LIMIT) && (distance_A > LOWER_us_LIMIT)) && ((distance_B < UPPER_us_LIMIT) && (distance_B > LOWER_us_LIMIT)))
		return OBSTACLE_SENSOR_SUCCESS;
	return OBSTACLE_SENSOR_FAIL;
}

void start_motors_down(void)
{
	rotational_motor_right();
	translational_motor_down();
}

void start_motors_up(void)
{
	rotational_motor_left();
	translational_motor_up();
}
bool check_accel(void)
{
	accel_data data;
	data.x = 0;
	data.y = 0;
	data.z = 0;
	bool status = read_accel(&data);
	if((status == ACCEL_READ_ERR) || (data.x < ax_lower) || (data.x > ax_upper) || (data.y < ay_lower) || (data.y > ay_upper) || (data.z < az_lower) || (data.z > az_upper))
	{
		return ERROR;
	}
	return COMPLETE;
}

bool check_gyro(void)
{
	gyro_data data;
	data.x = 0;
	data.y = 0;
	data.z = 0;
	bool status = read_gyroscope(&data);
	if(status == GYRO_READ_FAIL) || (data.x < gy_lower) || (data.x > gy_upper) || (data.y < gy_lower) || (data.y > gy_upper) || (data.z < gz_lower) || (data.z > gz_upper))
	{
		return ERROR;
	}
	return COMPLETE;
}
bool check_sensors(void)
{
	if((check_gyro == ERROR) || check_vs_med_a() | check_vs_med_b() || check_vs_slow_a() || check_vs_slow_b() || (check_accel() == ERROR))
		return ERROR;
	return COMPLETE;
}

void stop_clear_encoders(void)
{
	stop_encoders();
	clear_trans_encoder_cnt();
	clear_rotat_encoder_cnt();
}
