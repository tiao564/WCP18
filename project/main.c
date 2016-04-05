/*Logan Friedman

 *Main Algorithm for soil sampling mechanisms. Calls functiosn from algorithm.c
 */


#define F_CPU 8000000UL

#include "algorithm.h"
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <stdint.h>

int main()
{
	//Busy wait for enable signal from user
	check_enable();

	//Initialize intermediate variables
	bool exit_code = COMPLETE;
	bool obstacle;
	bool direction = DOWN; 
	bool motor_flag;
	bool init_code = initialize();
	bool sensor_chk;
	uint16_t rotat_cnt = 0;
	uint16_t trans_cnt = 0;
	
	//If initialization fails, returns error to user
	if(init_code == INIT_FAILED)
	{
		exit_code = ERROR;
	}
	else
	{
		while(exit_code != ERROR)
		{
			//Checks if it is able to start digging
			obstacle = check_obstacle_sensors();
			if(obstacle == OBSTACLE_SENSOR_FAIL)
			{
				exit_code = ERROR;
				break;
			}

			//drill begins downward motion turning to the right, starts the encoders
			if(motor_flag == MOTOR_OFF && direction == DOWN)
			{
				start_motors_down();
				init_encoders();
				motor_flag = MOTOR_ON;
			}

			//While drilling down, checking sensors and getting encoder counts to keep track of distance
			//Stops motors at the appropriate time, or if there is an error 
			if(motor_flag == MOTOR_ON && direction == DOWN)
			{
				while(trans_cnt < SIX_INCHES)
				{
					trans_cnt = get_trans_encoder_cnt();
					sensor_chk = check_sensors();
					if(sensor_chk == ERROR)
					{
						stop_motors();
						motor_flag == MOTOR_OFF;
						exit_code = ERROR;
					}
				}
				stop_motors();
				motor_flag = MOTOR_OFF;
				stop_clear_encoders();
				direction = UP;
			}

			//Pulls drill back up and turns off at 6 inches
			if(motor_flag == MOTOR_OFF && direction == UP)
			{
				init_encoders();
				start_motors_up();
				while(trans_cnt< SIX_INCHES)
				{
					trans_cnt = get_trans_encoder_cnt();
				}
				stop_motors();
				motor_flag = MOTOR_OFF;
				stop_clear_encoders();
				direction = DOWN;
				break;
			}
		}
	}

	//Sends signal to user to show completion or error occurred, disables everything
	send_signal_and_disable(exit_code);
	return 0;
}
