/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver that enables reading of an ultrasonic ping sensor
 *    such as those made by Polulu or Parallax. The driver utilizes
 *    the pin change interrupt functionality of an AVR micrcontroller
 *    to determine distance of an obstacle. Timer overflow interrupts
 *    are used to provide a timeout feature to the software. 
 *
 **************************************************************/
 
/*Processor Frequency*/
#define F_CPU 8000000UL

#include "ultrasonic.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdfix.h>
#include <util/delay.h>

/*Concatenation Macros*/
#define CONCAT(A,B) (A##B)
#define DDR(letter) CONCAT(DDR,letter)
#define PORT(letter) CONCAT(PORT,letter)
#define PIN(letter) CONCAT(PIN,letter)

/*Pin Change Interrupt Control Reg Bits*/
#define PCIE0 0

/*Timer3 Waveform Generation Bits*/
#define WGM33 4
#define WGM32 3
#define WGM31 1
#define WGM30 0 

/*Timer3 Clock Select Bits*/
#define CS32 2
#define CS31 1
#define CS30 0

/*Timer3 Interrupt Mask*/
#define TOIE3 0

/*Delay Macros*/
#define LOW  1
#define HIGH 10

/*Mask to clear registers*/
#define CLEAR 0x00

/*Max measurable distance in cm*/
#define TIMEOUT_DIST 2247.8505F // ~7.37 feet

/*The speed of sound in centimeters/microsecond*/
#define SPEED_SOUND 0.0343F

/*Represents sensor in use*/
static sensor curr_sensor_id;

/*Echo returned flag*/
volatile bool echo;

/*Timeout Flag*/
volatile bool timeout = false;

/*Width of pulse returned by sensor*/
volatile uint16_t pulse_width = 0;

/*Static Function Prototypes*/
static void set_trigger_a(void);
static void clear_trigger_a(void);
static void set_trigger_b(void);
static void clear_trigger_b(void);
static void pulse_trigger_a(void);
static void pulse_trigger_b(void);
static void start_counter(void);
static void stop_counter(void);
static bool detect_rising_edge(void);
static accum us_to_cm(uint16_t time);

/*Sets trigger a output high*/
static void set_trigger_a(void)
{
	PORT(TRIGGER_A_PORT) |= (1 << TRIGGER_A_POS);
}

/*Clears trigger a output to low*/
static void clear_trigger_a(void)
{
	PORT(TRIGGER_A_PORT) &= ~(1 << TRIGGER_A_POS);
}

/*Sets trigger b output high*/
static void set_trigger_b(void)
{
	PORT(TRIGGER_B_PORT) |= (1 << TRIGGER_B_POS);
}

/*Clears trigger b output low*/
static void clear_trigger_b(void)
{
	PORT(TRIGGER_B_PORT) &= ~(1 << TRIGGER_B_POS);
}

/*Triggers a pulse to be transmitted by the sensor*/
static void pulse_trigger_a(void)
{
	clear_trigger_a();
	_delay_us(LOW); //hold low 1 us
	set_trigger_a();
	_delay_us(HIGH); //hold high 10 us
	clear_trigger_a();
}

/*Triggers a pulse to be transmitted by the sensor*/
static void pulse_trigger_b(void)
{
	clear_trigger_b();
	_delay_us(LOW); //hold low 1 us
	set_trigger_b();
	_delay_us(HIGH); //hold high 10 us
	clear_trigger_b();
}

/*Enables Timer/Counter3*/
static void start_counter(void)
{
	/*With divide by eight prescaler*/
	TCCR3B |= (1 << CS31);
}

/*Shuts off Timer/Counter3*/
static void stop_counter(void)
{
	TCCR3B &= ~((1 << CS32) | (1 << CS31) | (1 << CS30));
}

/*Determines whether or not a rising edge has occurred*/
static bool detect_rising_edge(void)
{
	if(curr_sensor_id == A) 
	{	
		/*Return reading on sensor A echo pin*/
		return (PIN(ECHO_A_PORT) & (1 << ECHO_A_POS));
	}
	/*Return reading on sensor B echo pin*/
	return (PIN(ECHO_B_PORT) & (1 << ECHO_B_POS));	
}

/*Converts time in microseconds to distance in centimeters*/
static accum us_to_cm(uint16_t time)
{
	return (SPEED_SOUND * (accum)time);
}

/*See ultrasonic.h for details*/
void init_ultrasonic_sensors(void)
{
	sei();
	/*Enable pin change interrupts 7:0*/
	PCICR |= (1 << PCIE0);
	/*Enable timer overflow interrupts*/
	TIMSK3 |= (1 << TOIE3);
	/*Configure timer/counter3 to normal mode*/
	TCCR3A &= ~((1 << WGM31) | (1 << WGM30));
	TCCR3B &= ~((1 << WGM33) | (1 << WGM32));
	/*Configure sensor1 ports*/
	DDR(TRIGGER_A_PORT) |= (1 << TRIGGER_A_POS);
	DDR(ECHO_A_PORT) &= ~(1 << ECHO_A_POS);
	/*Configure sensor2 ports*/
	DDR(TRIGGER_B_PORT) |= (1 << TRIGGER_B_POS);
	DDR(ECHO_B_PORT) &= ~(1 << ECHO_B_POS);
}

/*Determines the width of returned pulse*/
ISR(PCINT0_vect)
{	
	/*Rising edge pin change*/
	if(detect_rising_edge())
	{
		TCNT3 = 0;
		start_counter();
		echo = false;
	}
	/*Falling edge pin change*/
	else
	{
		stop_counter();
		pulse_width = TCNT3;
		echo = true;
	}
}

/*Sensor time-out functionality*/
ISR(TIMER3_OVF_vect)
{
	timeout = true;
}

/*See ultrasonic.h for details*/
accum get_obstacle_distance_cm(sensor id)
{
	/*Clear pulse width between calls*/
	pulse_width = 0;
	/*Reset echo flag between calls*/
	echo = false;
	/*Ensures only one sensor can generate interrupt*/
	PCMSK0 &= CLEAR;
	
	/*Sensor A will be used*/
	if(id == A) 
	{
		curr_sensor_id = A;
		PCMSK0 |= (1 << PCINT1);
		pulse_trigger_a();
	}
	/*Sensor B will be used*/
	else 
	{
		curr_sensor_id = B;
		PCMSK0 |= (1 << PCINT3);
		pulse_trigger_b();
	}
	
	/*Poll the echo flag*/
	while(!echo)
	{
		//exit if timeout occurs
		if(timeout) 
		{
			timeout = false;
			return TIMEOUT_DIST;
		}
	}
	
	/*Return the obstacle distince in cm*/
	return us_to_cm(pulse_width);
}
/* End of ultrasonic.c */
