#define F_CPU 8000000UL

#include "vibration.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd_driver.h"
#include <util/delay.h>

int main()
{
	sei();
	
	initialize_LCD_driver();
	init_vibration_sensors();
	
	while(1)
	{
		if(check_vs_med_a())
		{
			lcd_erase();
			lcd_puts("Detected");
			lcd_goto_xy(1,0);
			lcd_puts("Med A");
			_delay_ms(500);
		}

		if(check_vs_med_b())
		{
			lcd_erase();
			lcd_puts("Detected");
			lcd_goto_xy(1,0);
			lcd_puts("Med B");
			_delay_ms(500);
		}

		if(check_vs_slow_a())
		{
			lcd_erase();
			lcd_puts("Detected");
			lcd_goto_xy(1,0);
			lcd_puts("Slow A");
			_delay_ms(500);
		}

		if(check_vs_slow_b())
		{
			lcd_erase();
			lcd_puts("Detected");
			lcd_goto_xy(1,0);
			lcd_puts("Slow B");
			_delay_ms(500);
		}
	}
	
	return 0;
}
