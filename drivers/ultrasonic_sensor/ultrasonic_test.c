#define F_CPU 8000000UL

#include "ultrasonic.h"
#include "lcd_driver.h"
#include <avr/io.h>
#include <stdfix.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH	  7
#define PRECISION 3

int main()
{
	initialize_LCD_driver();
	init_ultrasonic_sensors();
	
	accum distance = 0;
	char dist_str[9];
	
	/*Continually read distance sensor A*/
	while(1)
	{
		distance = get_obstacle_distance_cm(A);
		dtostrf(distance,WIDTH,PRECISION,dist_str);
		dist_str[8] = '\0'; 
		lcd_erase();
		lcd_puts(dist_str);
		_delay_ms(500);
	}
	
	return 0;
}
