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

		bool encoder_chk;
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
			//Loops through algorithm as long as no errors, no override, and has not completed 
			while(exit_code != ERROR && complete_flag != ON && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
			{
				//Checks if it is able to start digging
				sensor_chk = check_encoder_sensors(0,1,0,1,ON);
				if(sensor_chk == ERROR || get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE)
				{
					//Turn on error/override light, and set error code
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

					//Checks for override and distance travelled
					while(curr_trans_cnt < SIX_INCHES && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
					{
						//Gets the new counts and compares to previous counts to make sure motors are moving 
						prev_rotat_cnt = curr_rotat_cnt;
						prev_trans_cnt = curr_trans_cnt;
						curr_rotat_cnt = get_rotat_encoder_cnt();
						curr_trans_cnt = get_trans_encoder_cnt();

						//Checks all the sensors for any error conditions 
						sensor_chk = check_encoder_sensors(prev_rotat_cnt,curr_rotat_cnt,prev_trans_cnt,curr_trans_cnt,OFF);
						
						
						//if error sensed or override signal received, stop motors, save encoder counts and get out of downward motion
						if(sensor_chk == ERROR || get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE)
						{
							PORTA |= (1 << LED_ERROR_POS);
							stop_motors();
							error_trans_cnt = get_trans_encoder_cnt();
							motor_flag = MOTOR_OFF;
							exit_code = ERROR;
							break;
						}
					}

					//if error or override, break out of drilling loop before drill starts to go upwards
					if(exit_code == ERROR || get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE) break;
					
					//Otherwise brake motors and switch direction to up
					stop_motors();
					motor_flag = MOTOR_OFF;
					clear_encoders();
					direction = UP;
				}

				//Pulls drill back up and turns off at 6 inches, if no override signal received
				if(motor_flag == MOTOR_OFF && direction == UP && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
				{
					//Signal that drilling was successful and begins upward motion
					PORTA |= (1 << LED_COMPLETE_POS);
					start_motors_up();
					motor_flag = MOTOR_ON;
				}

				//Upward motion loop, if no override signal received
				if(motor_flag == MOTOR_ON && direction == UP && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
				{
					//reset encoder counts
					curr_trans_cnt = 0;
					curr_rotat_cnt = 0;

					//Begin six inch lift
					while(curr_trans_cnt< SIX_INCHES && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
					{
						prev_trans_cnt = curr_trans_cnt;
						prev_rotat_cnt = curr_rotat_cnt;
						curr_trans_cnt = get_trans_encoder_cnt();
						curr_rotat_cnt = get_rotat_encoder_cnt();
						encoder_chk = check_encoders(prev_rotat_cnt,curr_rotat_cnt,prev_trans_cnt,curr_trans_cnt);
						//if override signal recevied, save encoder count, turn off motors and switch complete LED to manual override/error LED, break out of upward motion loop
						if(encoder_chk == ERROR || get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE)
						{
							stop_motors();
							error_trans_cnt = get_trans_encoder_cnt();
							motor_flag = MOTOR_OFF;
							exit_code = ERROR;
							PORTA &= ~(1 << LED_COMPLETE_POS);
							PORTA |= (1 << LED_ERROR_POS);
							break;
						}

					}

					//Brake motors, and set complete flag to know that drilling is done 
					stop_motors();
					motor_flag = MOTOR_OFF;
					clear_encoders();
					direction = DOWN;
					complete_flag = ON;
				}
			}
		}

		//Error during drilling but no manual override
		if(exit_code == ERROR && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
		{

			//Set error LED
			PORTA |= (1 << LED_ERROR_POS);
			clear_encoders();
			start_motors_up();
			motor_flag = MOTOR_ON;
			//Upward drilling and no manual override
			//If manual override received here or motors stop spinning,  then drilling exits completely
			while(curr_trans_cnt < error_trans_cnt && get_sys_cntl_state() != SYS_MANUAL_OVERRIDE_STATE)
			{
					prev_trans_cnt = curr_trans_cnt;
					prev_rotat_cnt = curr_rotat_cnt;
					curr_trans_cnt = get_trans_encoder_cnt();
					curr_rotat_cnt = get_rotat_encoder_cnt();
					encoder_chk = check_encoders(prev_rotat_cnt,curr_rotat_cnt,prev_trans_cnt,curr_trans_cnt);
					if(encoder_chk == ERROR || get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE)
					{
						stop_motors();
						stop_encoders();
						clear_encoders();
						disable(exit_code);
						return 0;
					}
			}
			stop_motors();
			motor_flag = MOTOR_OFF;
		}
		
		//Manual Override Procedure
		//Toggle manual override and then let go and turn enable signal to "OFF" position for complete system kill. Will have 10 seconds to select otherwise next case will occur-- Used if dangerous to turn on the motors again to take out the drill
		//If no kill signal is received after the initial manual override, then drilling will be aborted and the drill will be lifted up. Error/manual override LED will be on
		if(get_sys_cntl_state() == SYS_MANUAL_OVERRIDE_STATE)
		{
			PORTA |= (1 << LED_ERROR_POS);
			//Stop Motors
			stop_motors();
			clear_encoders();

			//Wait 10 seconds to allow for kill signal
			_delay_ms(10000);
			if(get_sys_cntl_state() == SYS_OFF_STATE)
			{
				return 0;
			}

			//No kill signal received, lift drill out 
			else
			{
				clear_encoders();
				motor_flag = MOTOR_ON;
				start_motors_up();
				
				//Counts up to saved count 
				//If motors stop spinning, then program exits completely
				while(curr_trans_cnt < error_trans_cnt)
				{
					prev_trans_cnt = curr_trans_cnt;
					prev_rotat_cnt = curr_rotat_cnt;
					curr_trans_cnt = get_trans_encoder_cnt();
					curr_rotat_cnt = get_rotat_encoder_cnt();
					encoder_chk = check_encoders(prev_rotat_cnt,curr_rotat_cnt,prev_trans_cnt,curr_trans_cnt);
					if(encoder_chk == ERROR)
					{
						stop_motors();
						stop_encoders();
						clear_encoders();
						disable(exit_code);
						return 0;
					}
				}

				//Stops motors, sends error code and disables
				stop_motors();
				motor_flag = MOTOR_OFF;
				exit_code = ERROR;
				disable(exit_code);
				return 0;
			}
		}

		//Sends signal to user to show completion or error occurred, disables everything
		disable(exit_code);
		trip_count += 1;
	}
	return 0;
}
