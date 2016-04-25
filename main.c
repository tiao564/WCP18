/*Logan Friedman

 *Main Algorithm for soil sampling mechanisms. Calls functions from algorithm.c
 */

//CPU clock speed
#define F_CPU 8000000UL

//Include files
#include "algorithm.h"
#include "system_ctl.h"
#include "encoder.h"
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <stdint.h>

int main()
{
	uint8_t trip_count = 0;
	while(1)
	{
		//Busy wait for enable signal from user
		init_pins_check_enable();


		//Code to return to user, COMPLETE if no errors, ERROR if drilling failed
		bool exit_code = COMPLETE;
		//Direction of translation motor
		bool direction = DOWN; 
		//Flag to tell if motor is on
		bool motor_flag = MOTOR_OFF;
		//Checks if initialized correctly
		bool init_code = initialize();

		//Used to break out of loop, when complete
		bool complete_flag = OFF;
		//return of  check_encoder_sensors(), tells if a sensor was not in range
		bool sensor_chk;

		//Encoder counts
		uint16_t curr_rotat_cnt = 0;
		uint16_t curr_trans_cnt = 0;
		uint16_t prev_rotat_cnt = 0;
		uint16_t prev_trans_cnt = 0;
		uint16_t error_trans_cnt = 0;
		//If initialization fails, returns error to user
		if(init_code == INIT_FAILED)
		{
			exit_code = ERROR;
		}
		else
		{
			while(exit_code != ERROR && complete_flag != ON && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
			{
				//Checks if it is able to start digging
				sensor_chk = check_encoder_sensors(0,1,0,1);
				if(sensor_chk == ERROR || get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE)
				{
					PORTA |= (1 << LED_ERROR_POS);
					exit_code = ERROR;
					break;
				}

				//drill begins downward motion turning to the right, starts the encoders
				if(motor_flag == MOTOR_OFF && direction == DOWN && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
				{
					start_motors_down();
					motor_flag = MOTOR_ON;
				}

				//While drilling down, checking sensors and getting encoder counts to keep track of distance
				//Stops motors at the appropriate time, or if there is an error 
				if(motor_flag == MOTOR_ON && direction == DOWN && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
				{
					curr_rotat_cnt = 0;
					curr_trans_cnt = 0;
					while(curr_trans_cnt < SIX_INCHES && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
					{
						
						prev_rotat_cnt = curr_rotat_cnt;
						prev_trans_cnt = curr_trans_cnt;
						curr_rotat_cnt = get_rotat_encoder_cnt();
						curr_trans_cnt = get_trans_encoder_cnt();
						sensor_chk = check_encoder_sensors(prev_rotat_cnt,curr_rotat_cnt,prev_trans_cnt,curr_trans_cnt);
						if(sensor_chk == ERROR || get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE)
						{
							PORTA &= ~(1 << LED_ERROR_POS);
							stop_motors();
							error_trans_cnt = get_trans_encoder_cnt();
							motor_flag = MOTOR_OFF;
							exit_code = ERROR;
							break;
						}
					}
					if(exit_code == ERROR || get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE) break;
					stop_motors();
					motor_flag = MOTOR_OFF;
					clear_encoders();
					direction = UP;
				}

				//Pulls drill back up and turns off at 6 inches
				if(motor_flag == MOTOR_OFF && direction == UP && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
				{
					PORTA |= (1 << LED_COMPLETE_POS);
					start_motors_up();
					motor_flag = MOTOR_ON;
				}
				if(motor_flag == MOTOR_ON && direction == UP && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
				{
					curr_trans_cnt = 0;
					curr_rotat_cnt = 0;
					while(curr_trans_cnt< SIX_INCHES && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
					{
						curr_trans_cnt = get_trans_encoder_cnt();
					}
					stop_motors();
					motor_flag = MOTOR_OFF;
					clear_encoders();
					direction = DOWN;
					complete_flag = ON;
				}
			}
		}
		if(get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE)
		{
			stop_motors();
			motor_flag = MOTOR_OFF;
			exit_code = ERROR;
		}
		if(exit_code == ERROR && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
		{
			PORTA |= (1 << LED_ERROR_POS);
			clear_encoders();
			curr_trans_cnt = 0;
			start_motors_up();
			motor_flag = MOTOR_ON;
			while(curr_trans_cnt < error_trans_cnt && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
			{
				curr_trans_cnt = get_trans_encoder_cnt();
			}
			stop_motors();
			motor_flag = MOTOR_OFF;
		}
		//Sends signal to user to show completion or error occurred, disables everything
		disable(exit_code);
		trip_count += 1;
	}
	return 0;
}
