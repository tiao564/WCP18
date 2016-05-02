/**************************************
 *
 * Nicholas Shanahan (2016)
 *
 * Description:
 *	- Module to test system input interface
 *    driver (system_cntl_if_test.c).
 *
 **************************************/

#include <avr/io.h>

/*Tester Output Pin*/
#define TEST_DATA_PORT D
#define TEST_DATA_POS  5

/*I/O Macros*/
#define CONCAT(A,B) (A##B)
#define DDR(letter) CONCAT(DDR,letter)
#define PORT(letter) CONCAT(PORT,letter)

/*Timer/Counter1 Output*/
#define COM1A1 7
#define COM1A0 6
#define COM1B1 5
#define COM1B0 4

/*Waveform Bits*/
#define WGM13 4
#define WGM12 3
#define WGM11 1
#define WGM10 0

/*Clock Scaling Bits*/
#define CS11 1

/*Mask to Clear Register*/
#define CLEAR (0x00)

/*Function Prototypes*/
void config_tester(void);
void pulse_1000_us(void);
void pulse_1500_us(void);
void pulse_2000_us(void);

/*Main program for tester software*/
int main()
{
	config_tester();
	
	//pulse_1000_us();
	//pulse_1500_us();
	//pulse_2000_us();
	
	OCR1A = 1000;
	TCCR1A |= (1 << COM1A0);
	
	while(1);
	
	return 0;
}

//Configure tester
void config_tester(void)
{
	//Configure tester output pin
	DDR(TEST_DATA_PORT) |= (1 << TEST_DATA_POS);
	//Enable OC1A output
	TCCR1A = ((TCCR1A & CLEAR) | ((1 << WGM11) | (1 << WGM10)));
	//Set timer to Fast PWM mode
	TCCR1B = ((TCCR1B & CLEAR) | ((1 << WGM13) | (1 << WGM12) | (1 << CS11)));	
}

//Output 1,000us pulse
void pulse_1000_us(void)
{
	OCR1A = 1000;
}

//Output 1,500us pulse
void pulse_1500_us(void)
{
	OCR1A = 1500;
} 

//Output 2,000us pulse
void pulse_2000_us(void)
{
	OCR1A = 2000;
}
/* End of system_tester.c */