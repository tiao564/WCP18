#define F_CPU 8000000UL

#include "motor.h"
#include "encoder.h"
#include <avr/io.h>
#include <util/delay.h>

/*Motor Speeds*/
#define ROTAT_DUTY_CYCLE 192 //75%
#define TRANS_DUTY_CYCLE 204 //80%

/*Timing Delays*/
#define MOTOR_DELAY 3500 //5 seconds
#define BRAKE_DELAY 1000 //1 second

//Translational Motor Revolutions
#define TRANS_DIST 35000UL

int main()
{
	init_encoders();
	init_motor_drivers();
	brake_rotational_motor();
	brake_translational_motor();

	/*Clear encoder values*/
	clear_trans_encoder_cnt();
	clear_rotat_encoder_cnt();
	
	/*Set motor speeds*/
	set_rotational_motor_speed(ROTAT_DUTY_CYCLE);
	set_translational_motor_speed(TRANS_DUTY_CYCLE);
	
	while(get_trans_encoder_cnt() < TRANS_DIST)
	{
		/*Test both motors in first direction*/
		rotational_motor_left();
		translational_motor_up();
	}

	//_delay_ms(MOTOR_DELAY);
	brake_rotational_motor();
	brake_translational_motor();
	
	return 0;
}