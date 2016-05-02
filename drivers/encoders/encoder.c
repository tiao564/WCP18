/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver to interface DC motor 2 channel encoders. Intended
 *    for an AVR microprocessor. Utilizes 8bit Timer/Counter0 
 *	  interrupt capabilities to sample the encoder values at fixed
 *    intervals defined by the user. The API has been design to
 *    specifically interface to independent encoders.
 *
 **************************************************************/

/********************************************
 * 		          Includes                  *
 ********************************************/ 
#include "encoder.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>

/********************************************
 * 		           Macros                   *
 ********************************************/
/*Concatenation Macros*/
#define CONCAT(A,B) (A##B)
#define DDR(letter) CONCAT(DDR,letter)
#define PIN(letter) CONCAT(PIN,letter)

/*Mask to clear registers*/
#define CLEAR 0x00

/*Timer0 Interrupt Mask Register Bits*/
#define OCIE0A 1

/*TCCR0A Control Bits*/
#define WGM01 1

/*TCCR0B Control Bits*/
#define CS02 2
#define CS01 1
#define CS00 0

/*Encoder Sampling Rate*/
#define SAMPLING_RATE 200 // 5000 samples/sec

/*Misc.*/
#define A 1
#define B 0

/********************************************
 * 	          Global Variables              *
 ********************************************/
/*Variables to hold translation encoder values*/
volatile uint8_t curr_trans_encoder = 0;
volatile uint8_t prev_trans_encoder = 0;
volatile uint16_t trans_encoder_cnt = 0;
/*Variables to hold rotational encoder values*/
volatile uint8_t curr_rotat_encoder = 0;
volatile uint8_t prev_rotat_encoder = 0;
volatile uint16_t rotat_encoder_cnt = 0;

/********************************************
 * 	    Static Function Prototypes          *
 ********************************************/
static bool read_trans_encoder_a(void);
static bool read_trans_encoder_b(void);
static bool read_rotat_encoder_a(void);
static bool read_rotat_encoder_b(void);
static void set_curr_trans_encoder_val(bool a, bool b); 
static void set_curr_rotat_encoder_val(bool a, bool b); 

/*Get value of translation encoder A*/
static bool read_trans_encoder_a(void)
{
	return (PIN(TRANS_ENCODER_A_PORT) & (1 << TRANS_ENCODER_A_POS));
}

/*Get value of translation encoder B*/
static bool read_trans_encoder_b(void)
{
	return (PIN(TRANS_ENCODER_B_PORT) & (1 << TRANS_ENCODER_B_POS));
}

/*Get value of rotational encoder A*/
static bool read_rotat_encoder_a(void)
{
	return (PIN(ROTAT_ENCODER_A_PORT) & (1 << ROTAT_ENCODER_A_POS));
}

/*Get value of rotational encoder B*/
static bool read_rotat_encoder_b(void)
{
	return (PIN(ROTAT_ENCODER_B_PORT) & (1 << ROTAT_ENCODER_B_POS));
}

/*Form a 2bit value from encoder inputs*/
static void set_curr_trans_encoder_val(bool a, bool b)
{
	curr_trans_encoder = 0;
	/*Set bit 1 if a is a = 1*/
	if(a) curr_trans_encoder |= (1 << A);
	/*Set bit 0 if b is b = 1*/
	if(b) curr_trans_encoder |= (1 << B);
}

/*Form a 2bit value from encoder inputs*/
static void set_curr_rotat_encoder_val(bool a, bool b)
{
	curr_rotat_encoder = 0;
	/*Set bit 1 if a is a = 1*/
	if(a) curr_rotat_encoder |= (1 << A);
	/*Set bit 0 if b is b = 1*/
	if(b) curr_rotat_encoder |= (1 << B);
}

/********************************************
 * 	     Interrupt Service Routines         *
 ********************************************/
/*ISR to sample the encoders at fixed intervals*/
ISR(TIMER0_COMPA_vect)
{
	/*Read translation encoder inputs*/
	bool tA = read_trans_encoder_a();
	bool tB = read_trans_encoder_b();
	set_curr_trans_encoder_val(tA,tB);
	/*Read rotational encoder inputs*/
	bool rA = read_rotat_encoder_a();
	bool rB = read_rotat_encoder_b();
	set_curr_rotat_encoder_val(rA,rB);
	
	/*Update translation encoder count*/
	if(curr_trans_encoder != prev_trans_encoder)
	{
		trans_encoder_cnt++;
		prev_trans_encoder = curr_trans_encoder;
	}
	
	/*Update rotational encoder count*/
	if(curr_rotat_encoder != prev_rotat_encoder)
	{
		rotat_encoder_cnt++;
		prev_rotat_encoder = curr_rotat_encoder;
	}
}

/********************************************
 * 		        API Functions               *
 ********************************************/
/*See encoder.h for details*/
void init_encoders(void)
{
	sei();
	/*Configure translation encoder ports as inputs*/
	DDR(TRANS_ENCODER_A_PORT) &= ~(1 << TRANS_ENCODER_A_POS);
	DDR(TRANS_ENCODER_B_PORT) &= ~(1 << TRANS_ENCODER_B_POS);
	/*Configure rotational encoder ports as inputs*/
	DDR(ROTAT_ENCODER_A_PORT) &= ~(1 << ROTAT_ENCODER_A_POS);
	DDR(ROTAT_ENCODER_B_PORT) &= ~(1 << ROTAT_ENCODER_B_POS);
	/*Set CTC mode*/
	TCCR0A = ((TCCR0A & CLEAR) | (1 << WGM01));
	TCCR0B = TCCR0B & CLEAR;
	/*Set fixed encoder sampling rate*/
	OCR0A = SAMPLING_RATE;
}

/*See encoder.h for details*/
uint8_t get_sampling_rate(void)
{
	return OCR0A;
}

/*See encoder.h for details*/
uint16_t get_trans_encoder_cnt(void)
{
	return trans_encoder_cnt;
}

/*See encoder.h for details*/
uint16_t get_rotat_encoder_cnt(void)
{
	return rotat_encoder_cnt;
}

/*See encoder.h for details*/
void clear_trans_encoder_cnt(void)
{
	trans_encoder_cnt = 0;
}

/*See encoder.h for details*/
void clear_rotat_encoder_cnt(void)
{
	rotat_encoder_cnt = 0;
}

/*See encoder.h for details*/
void start_encoders(void)
{
	/*Enable timer interrupts*/
	TIMSK0 |= (1 << OCIE0A);
	/*Start counter with divide by 8 prescaler*/
	TCCR0B |= (1 << CS01);
}

/*See encoder.h for details*/
void stop_encoders(void)
{
	/*Stop the timer to disable encoders*/
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
	/*Turn of timer interrupts*/
	TIMSK0 &= ~(1 << OCIE0A);
}
/* End of encoder.c */
