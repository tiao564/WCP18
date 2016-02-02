/*Atmega1284p CPU Frequency*/
#define F_CPU 20000000

#include <avr/io.h>
#include <util/delay.h>

/*Concatenation Macros*/
#define CONCAT(A,B) A##B
#define DDR(letter) CONCAT(DDR,letter)
#define PORT(letter) CONCAT(PORT,letter)

/*LED Pin Location
 *May be changed freely*/
#define LED_PORT D
#define LED_LOC  6

/*LED Functions*/
#define LED_ON  1
#define LED_OFF 0

/*Blinking Time Interval*/
#define BLINK 500

/*** LED Blink Test ***/
int main()
{
	/*Configure LED port as output*/
	DDR(LED_PORT) |= (1 << LED_LOC);
	
	while(1)
	{
		/*Turn on LED*/
		PORT(LED_PORT) |= (LED_ON << LED_LOC);
		_delay_ms(BLINK);
		/*Turn of LED*/
		PORT(LED_PORT) &= (LED_OFF << LED_LOC);
		_delay_ms(BLINK);
	}

	return 0;
}
