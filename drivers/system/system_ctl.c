/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver for system control interface. The system control input
 *    is PWM signal connected to PC3 of the ATmega1284p MCU. There
 *    are three possible input pulse widths that correspond to three
 *    unique system states: OFF, ENABLE, and MANUAL_OVERRIDE.
 *    This driver uses pin change interrupt 19 to monitor the
 *    system control state continuously. 
 *
 **************************************************************/

/********************************************
* 		          Includes                  *
 ********************************************/
#include "system_ctl.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

/********************************************
* 		           Macros                   *
 ********************************************/
/*Concatenation Macros*/
#define CONCAT(A,B) (A##B)
#define DDR(letter) CONCAT(DDR,letter)
#define PORT(letter) CONCAT(PORT,letter)
#define PIN(letter) CONCAT(PIN,letter)

/*System Control Input Port*/
#define SYS_CNTL_PORT C
/*System Control Input Pin Location*/
#define SYS_CNTL 3

/*Pin Change Interrupt PortA*/
#define PCIE2 2

/*Pin Change Interrupt 19*/
#define PCINT19 3

/*Timer1 Waveform Generation*/
#define WGM13 4
#define WGM12 3
#define WGM11 1
#define WGM10 0

/*Timer1 Clock Select Bits*/
#define CS12 2
#define CS11 1
#define CS10 0

/*System Control Pulse Tolerance Factors*/
#define TOLERANCE 10

/*System Control Pulse Width Definitions
 *		  Base Values			*/
#define SYS_OFF				1000
#define SYS_ENABLE			2000
#define SYS_MANUAL_OVERRIDE	1500

/*		  Upper Bound Values	*/
#define SYS_OFF_UPPER			  (TOLERANCE + SYS_OFF)		
#define SYS_ENABLE_UPPER		  (TOLERANCE + SYS_ENABLE)
#define SYS_MANUAL_OVERRIDE_UPPER (TOLERANCE + SYS_MANUAL_OVERRIDE)

/*		 Lower Bound Values		*/
#define SYS_OFF_LOWER		      (SYS_OFF - TOLERANCE)		
#define SYS_ENABLE_LOWER		  (SYS_ENABLE - TOLERANCE)
#define SYS_MANUAL_OVERRIDE_LOWER (SYS_MANUAL_OVERRIDE - TOLERANCE)

/********************************************
* 		      Global Variables              *
 ********************************************/
/*Store Width of System Control Input Pulse*/
volatile uint16_t sys_ctl_pulse_width = 0;

/*Holds Current System Control State*/
int8_t sys_cntl_state = SYS_OFF_STATE;

/********************************************
 * 		Static Function Prototypes          *
 ********************************************/
static bool rising_edge(void);
static void start_counter(void);
static void determine_sys_state(void);

/*Determines if a rising edge occurs*/
static bool rising_edge(void)
{
	return (PIN(SYS_CNTL_PORT) & (1 << SYS_CNTL));
}

/*Turn on Timer/Counter1*/
static void start_counter(void)
{
	//Turn on & set divide by 8 prescaler
	TCCR1B |= (1 << CS11); //1 MHz clk
}

/*Disable Timer/Counter1*/
static void stop_counter(void)
{
	TCCR1B &= ~((1 << CS21) | (1 << CS11) | (1 << CS10));
}

/*Assess system control pulse width to determine state*/
static void determine_sys_state(void)
{
	//Enter Shutdown State
	if((sys_ctl_pulse_width >= SYS_OFF_LOWER) &&
	   (sys_ctl_pulse_width <= SYS_OFF_UPPER))
	{
		sys_cntl_state = SYS_OFF_STATE;
	}
	
	//Enter Activation State
	else if((sys_ctl_pulse_width >= SYS_ENABLE_LOWER) &&
	        (sys_ctl_pulse_width <= SYS_ENABLE_UPPER))
	{
		sys_cntl_state = SYS_ENABLE_STATE;
	}
	
	//Enter Emergency Shutdown State
	else if((sys_ctl_pulse_width >= SYS_MANUAL_OVERRIDE_LOWER) &&
		    (sys_ctl_pulse_width <= SYS_MANUAL_OVERRIDE_UPPER))
	{
		sys_cntl_state = SYS_MANUAL_OVERRIDE_STATE;
	}
	
	//Retain Current State
	else
	{
		sys_cntl_state = sys_cntl_state;
	}
}

/********************************************
 * 		 Interrupt Service Routines         *
 ********************************************/
/*Determines width of input control pulse*/
ISR(PCINT2_vect)
{
	//turn off interrupts
	cli();
	
	if(rising_edge())
	{
		TCNT1 = 0;
		start_counter();
	}
	
	else
	{
		stop_counter();
		sys_ctl_pulse_width = TCNT1;
		determine_sys_state();
		/*TODO: Add shutdown code here??*/
	}
	//restore interrupts
	sei();
}

/********************************************
 * 		        API Functions               *
 ********************************************/
/*See system_ctl.h for details*/
void init_system_cntl(void)
{
	sei();
	//Configure system control pin as input
	DDR(SYS_CNTL_PORT) &= ~(1 << SYS_CNTL); 
	//Enable pin change interrupts on PortA
	PCICR |= (1 << PCIE2);
	//Eanble pin change interrupt for Enable input
	PCMSK2 |= (1 << PCINT19);
	//Set Timer1 to normal mode
	TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));
}

/*See system_ctl.h for details*/
int8_t get_sys_cntl_state(void)
{
	return sys_cntl_state;
}
/* End of system_ctl.c */
