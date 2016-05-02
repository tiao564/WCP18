/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver for Adafruit vibration sensor switches. Utilizies the
 *    pin change interrupt functionality of AVR microcontrollers
 *    to determine when the sensor has tripped. When the vibration
 *    threshold of a sensor is exceeded, the the switch closes,
 *    triggering the pin change ISR.
 *
 **************************************************************/
 
/********************************************
 * 		          Includes                  *
 ********************************************/ 
#include "vibration.h"
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdbool.h>

/********************************************
 * 		           Macros                   *
 ********************************************/
#define CONCAT(A,B) (A##B)
#define DDR(letter) CONCAT(DDR,letter)
#define PIN(letter) CONCAT(PIN,letter)

/*Pin Change Interrupt Control Reg*/
#define PCIE1 1

/*Pin Change Mask Reg*/
#define PCINT8  0
#define PCINT9  1
#define PCINT10 2
#define PCINT11 3

/*Sensor Flag Masks*/
#define MED_A_FLAG  3
#define MED_B_FLAG  2
#define SLOW_A_FLAG 1
#define SLOW_B_FLAG 0

/********************************************
 * 	          Global Variables              *
 ********************************************/
/*Vibration Sensor Status Variable*/
volatile uint8_t vibration_status = 0;

/********************************************
 * 	    Static Function Prototypes          *
 ********************************************/
static void set_med_a_flag(void);
static void set_med_b_flag(void);
static void set_slow_a_flag(void);
static void set_slow_b_flag(void);
static void clear_med_a_flag(void);
static void clear_med_b_flag(void);
static void clear_slow_a_flag(void);
static void clear_slow_b_flag(void);
static bool get_med_a(void);
static bool get_med_b(void);
static bool get_slow_a(void);
static bool get_slow_b(void);

/*Returns the value of sensor Medium A*/
static bool get_med_a(void)
{
	return (PIN(MED_A_PORT) & (1 << MED_A_POS));
}

/*Returns the value of sensor Medium B*/
static bool get_med_b(void)
{
	return (PIN(MED_B_PORT) & (1 << MED_B_POS));
}

/*Returns the value of sensor Slow A*/
static bool get_slow_a(void)
{
	return (PIN(SLOW_A_PORT) & (1 << SLOW_A_POS));
}

/*Returns the value of sensor Slow B*/
static bool get_slow_b(void)
{
	return (PIN(SLOW_B_PORT) & (1 << SLOW_B_POS));
}

/*Sets the MED_A flag in status variable*/
static void set_med_a_flag(void)
{
	vibration_status |= (1 << MED_A_FLAG);
}

/*Sets the MED_B flag in status variable*/
static void set_med_b_flag(void)
{
	vibration_status |= (1 << MED_B_FLAG);
}

/*Sets the SLOW_A flag in status variable*/
static void set_slow_a_flag(void)
{
	vibration_status |= (1 << SLOW_A_FLAG);
}

/*Sets the SLOW_B flag in status variable*/
static void set_slow_b_flag(void)
{
	vibration_status |= (1 << SLOW_B_FLAG);
}

/*Clears the MED_A flag in status variable*/
static void clear_med_a_flag(void)
{
	vibration_status &= ~(1 << MED_A_FLAG);
}

/*Clears the MED_B flag in status variable*/
static void clear_med_b_flag(void)
{
	vibration_status &= ~(1 << MED_B_FLAG);
}

/*Clears the SLOW_A flag in status variable*/
static void clear_slow_a_flag(void)
{
	vibration_status &= ~(1 << SLOW_A_FLAG);
}

/*Clears the SLOW_B flag in status variable*/
static void clear_slow_b_flag(void)
{
	vibration_status &= ~(1 << SLOW_B_FLAG);
}

/********************************************
 * 	     Interrupt Service Routines         *
 ********************************************/
/*Sets flags in status variable if a vibration sensor trips*/
ISR(PCINT1_vect)
{
	/*Check Medium A vibration sensor*/
	if(get_med_a()) set_med_a_flag();
	/*Check Medium B vibration sensor*/
	if(get_med_b()) set_med_b_flag();
	/*Check Slow A vibration sensor*/
	if(get_slow_a()) set_slow_a_flag();
	/*Check Slow B vibration sensor*/
	if(get_slow_b()) set_slow_b_flag();
}

/********************************************
 * 		        API Functions               *
 ********************************************/
/*See vibration.h for details*/
void init_vibration_sensors(void)
{
	sei();
	/*Configure sensor ports as inputs*/
	DDR(MED_A_PORT) &= ~(1 << MED_A_POS);
	DDR(MED_B_PORT) &= ~(1 << MED_B_POS);
	DDR(SLOW_A_PORT) &= ~(1 << SLOW_A_POS);
	DDR(SLOW_B_PORT) &= ~(1 << SLOW_B_POS);
	/*Enable pin change interrupts on port B*/
	PCICR |= (1 << PCIE1);
	/*Enable vibration sensors to trigger interrupts*/
	PCMSK1 |= ((1 << PCINT11) | (1 << PCINT10) | (1 << PCINT9) | (1 << PCINT8));
}

/*See vibration.h for details*/
bool check_vs_med_a(void)
{
	/*Read flag value*/
	bool flag = (vibration_status & (1 << MED_A_FLAG));
	/*Manually clear flag after reading*/
	clear_med_a_flag();	
	return flag;
}

/*See vibration.h for details*/
bool check_vs_med_b(void)
{
	/*Read flag value*/
	bool flag = (vibration_status & (1 << MED_B_FLAG));
	/*Manually clear flag afer reading*/
	clear_med_b_flag();
	return flag;
}

/*See vibration.h for details*/
bool check_vs_slow_a(void)
{
	/*Read flag value*/
	bool flag = (vibration_status & (1 << SLOW_A_FLAG));
	/*Manually clear flag afer reading*/
	clear_slow_a_flag();
	return flag;
}

/*See vibration.h for details*/
bool check_vs_slow_b(void)
{
	/*Read flag value*/
	bool flag = (vibration_status & (1 << SLOW_B_FLAG));
	/*Manually clear flag afer reading*/
	clear_slow_b_flag();
	return flag;
}

/*See vibration.h for details*/
void disable_vibration_sensors(void)
{
	/*Turn of pin change interrupts*/
	PCICR &= ~(1 << PCIE1);
	/*Disable individual pins*/
	PCMSK1 &= ~((1 << PCINT11) | (1 << PCINT10) | (1 << PCINT9) | (1 << PCINT8));
} 
/* End of vibration.c */
