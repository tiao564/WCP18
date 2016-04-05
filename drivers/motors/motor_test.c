#define F_CPU 8000000UL

#include "motor.h"
#include <avr/io.h>
#include <util/delay.h>

/*Motor Speed --> 50% currently*/
#define DUTY_CYCLE 128

/*Timing Delays*/
#define MOTOR_DELAY 5000 //5 seconds
#define BRAKE_DELAY 1000 //1 second

int main()
{
	init_motor_drivers();
	brake_rotational_motor();
	brake_translational_motor();
	
	/*Set motor speeds*/
	set_rotational_motor_speed(DUTY_CYCLE);
	set_translational_motor_speed(DUTY_CYCLE);
	
	while(1)
	{
		/*Test both motors in first direction*/
		rotational_motor_right();
		translational_motor_up();
		_delay_ms(MOTOR_DELAY);
		brake_rotational_motor();
		brake_translational_motor();
		_delay_ms(BRAKE_DELAY);
		
		/*Test both motors in second direction*/
		rotational_motor_left();
		translational_motor_down();
		_delay_ms(MOTOR_DELAY);
		brake_rotational_motor();
		brake_translational_motor();
		_delay_ms(BRAKE_DELAY);
	}
	
	return 0;
}
