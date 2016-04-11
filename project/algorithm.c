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
#include "gyroscope.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//Look at algorithm.h for function descriptions
void stop_motors(void)
{
	brake_rotational_motor();
	brake_translational_motor();
	PORTA &= ~(1 << LED_MOTOR_POS);
}


void initpins_check_enable(void)
{
	bool enable = 0;
	DDRA &= ~(1 << ENABLE_POS);
	DDRA |= (1 << LED_MOTOR_POS);
	DDRA |= (1 << LED_COMPLETE_POS);
	DDRA |= (1 << LED_ERROR_POS);
	while(enable != 1)
	{
		enable = PINA & ENABLE_MASK;
	}
}

bool initialize(void)
{
	sei();
	gyro_range rng;
	bool accel_code = init_accel();
	bool gyro_code = init_gyro(rng);
	if(accel_code == ACCEL_INIT_FAILED || gyro_code == GYRO_INIT_FAIL)
		return INIT_FAILED;
	init_ultrasonic_sensors();
	init_motor_drivers();
	init_vibration_sensors();
	init_encoders();
	stop_motors();
	set_rotational_motor_speed(DUTY_CYCLE);
	set_translational_motor_speed(DUTY_CYCLE);
	set_sampling_rate(ENCODER_SAMPLING_RATE);
	enable_autorange();
	return INIT_SUCCESS;
}

void send_signal_and_disable(bool code)
{
	if(code == ERROR)
	{
		PORTA |= (1 << LED_ERROR_POS);
	}
	else
	{
		PORTA |= (1 << LED_COMPLETE_POS);
	}
	clear_encoders();
	stop_encoders();
	disable_motors();
	disable_autorange();
	disable_vibration_sensors();
}

bool check_obstacle_sensors(void)
{
	accum distance_A,distance_B;
	distance_A = get_obstacle_distance_cm(A);
	distance_B = get_obstacle_distance_cm(B);
	if((distance_A == TIMEOUT || distance_B == TIMEOUT) || (distance_A < LOWER_us_LIMIT) || (distance_A > UPPER_us_LIMIT) || (distance_B < LOWER_us_LIMIT) || (distance_B > UPPER_us_LIMIT))
		return OBSTACLE_SENSOR_FAIL;
	return OBSTACLE_SENSOR_SUCCESS;
}

void start_motors_down(void)
{
	PORTA |= (1 << LED_MOTOR_POS);
	rotational_motor_right();
	translational_motor_down();
}

void start_motors_up(void)
{
	PORTA |= (1 << LED_MOTOR_POS);
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
	if((status == GYRO_READ_FAIL) || (data.x < gy_lower) || (data.x > gy_upper) || (data.y < gy_lower) || (data.y > gy_upper) || (data.z < gz_lower) || (data.z > gz_upper))
	{
		return ERROR;
	}
	return COMPLETE;
}

bool check_encoders(uint16_t prev_rotat,uint16_t rotat,uint16_t prev_trans,uint16_t trans)
{
	if((prev_rotat == rotat) || (prev_trans == trans))
	{
		return ERROR;
	}
	return COMPLETE;
}

bool check_encoder_sensors(uint16_t prev_rotat,uint16_t rotat,uint16_t prev_trans,uint16_t trans)
{
	bool encoder_chk = check_encoders(prev_rotat,rotat,prev_trans,trans);
	if((encoder_chk == ERROR) || (check_obstacle_sensors() == OBSTACLE_SENSOR_FAIL) || (check_gyro == ERROR) || check_vs_med_a() | check_vs_med_b() || check_vs_slow_a() || check_vs_slow_b() || (check_accel() == ERROR))
		return ERROR;
	return COMPLETE;
}

void clear_encoders(void)
{
	clear_trans_encoder_cnt();
	clear_rotat_encoder_cnt();
}
