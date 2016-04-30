/******************************************
 *
 * Nicholas Shanahan (2016)
 *
 * Description
 *  - Test application for drilling system.
 *    Utilizes system input, motor, and encoder
 *    driver software.
 *
 ******************************************/

//Processor Frequency
#define F_CPU 8000000UL

#include "motor.h"
#include "encoder.h"
#include "system_ctl.h"
#include <avr/io.h>
#include <util/delay.h>

//Motor Speeds
#define ROTAT_DUTY_CYCLE 192 //75%
#define TRANS_DUTY_CYCLE 204 //80%

//Timing Delays
#define MOTOR_DELAY 3500 //5 seconds
#define BRAKE_DELAY 1000 //1 second

//Translational Motor Revolutions
#define TRANS_DIST 32000UL

//Red LED location
#define GREEN_LED 6
#define RED_LED   5
#define BLUE_LED  4

int main()
{
	uint16_t trans_enc_cnt = 0;
	uint16_t rotat_enc_cnt = 0;
	uint16_t ret_dist = 0;

	bool override = false;

	//Configure LED output ports
	DDRA |= ((1 << GREEN_LED) | (1 << RED_LED) | (1 << BLUE_LED));

	//Initialize drivers
	init_encoders();
	init_motor_drivers();
	init_system_cntl();
	brake_rotational_motor();
	brake_translational_motor();

	//Clear encoder values
	clear_trans_encoder_cnt();
	clear_rotat_encoder_cnt();

	start_encoders();
	
	//Set motor speeds
	set_rotational_motor_speed(ROTAT_DUTY_CYCLE);
	set_translational_motor_speed(TRANS_DUTY_CYCLE);

	//Poll for input signal
	while(!get_sys_cntl_state());

	PORTA |= (1 << BLUE_LED);
	
	while(trans_enc_cnt < TRANS_DIST)
	{
		trans_enc_cnt = get_trans_encoder_cnt();

		//Manual override
		if(get_sys_cntl_state())
		{
			ret_dist = trans_enc_cnt;
			override = true;
			goto MANUAL_OVERRIDE;
		}

		//Test both motors in first direction
		rotational_motor_right();
		translational_motor_down();
		
	}

	brake_translational_motor();
	_delay_ms(2000);
	//Dig additional soil
	brake_rotational_motor();
	PORTA |= (1 << GREEN_LED);
	_delay_ms(1000);

	clear_trans_encoder_cnt();
	clear_rotat_encoder_cnt();
	trans_enc_cnt = 0;

	while(get_trans_encoder_cnt() < TRANS_DIST)
	{
		//Manual override
		if(get_sys_cntl_state())
		{
			override = true;
			goto MANUAL_OVERRIDE;
		}
	
		//Test both motors in first direction
		rotational_motor_left();
		translational_motor_up();
	}

	brake_rotational_motor();
	brake_translational_motor();

	//Blink LED when drilling completes
	for(uint8_t i = 0; i < 3; i++)
	{
		_delay_ms(500);
		PORTA &= ~(1 << BLUE_LED);
		_delay_ms(500);
		PORTA |= (1 << BLUE_LED);
	}

/* Executed if manual override is activated */
MANUAL_OVERRIDE:

	//Manual override
	if(override)
	{ 
		PORTA = ((PORTA & ~(1 << BLUE_LED)) | (1 << RED_LED));
		brake_rotational_motor();
		brake_translational_motor();

		//Wait for second button press
		while(1)
		{
			if(get_sys_cntl_state())
			{
				while(get_trans_encoder_cnt() < ret_dist)
				{
					rotational_motor_left();
					translational_motor_up();
				}

				brake_translational_motor();
				brake_rotational_motor();
				break;
			}
		}
	}
		
	return 0;
}
/* End of control.c */
