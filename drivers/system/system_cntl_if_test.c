/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Test file for the system control interface.
 *
 **************************************************************/

#include "system_ctl.h"
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

#define LED_A 0 //SYS_OFF_STATE LED
#define LED_B 1 //SYS_ENABLE_STATE LED
#define LED_C 2 //SYS_MANUAL_OVERRIDE_STATE LED

//Mask to clear port
#define CLEAR (0x00)

int main()
{
	sei();
	
	int8_t sys_state = SYS_OFF_STATE;
	//Init system control interface
	init_system_cntl();
	
	//Initialize LEDs
	DDRA |= ((1 << LED_A) | (1 << LED_B) | (1 << LED_C));
	
	while(1)
	{
		sys_state = get_sys_cntl_state();
		
		switch(sys_state)
		{
			case SYS_OFF_STATE:
				PORTA = ((PORTA & CLEAR) | (1 << LED_A));
				break;
				
			case SYS_ENABLE_STATE:
				PORTA = ((PORTA & CLEAR) | (1 << LED_B));
				break;
			
			case SYS_MANUAL_OVERRIDE_STATE:
				PORTA = ((PORTA & CLEAR) | (1 << LED_C));
				break;
			
			default:
				PORTA & CLEAR;
		}
	}
	
	return 0;
}