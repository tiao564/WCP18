/*Processor Clock Frequency*/
#define F_CPU 8000000UL

#include "gyroscope.h"
#include "lcd_driver.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>

int main()
{
	initialize_LCD_driver();
	
	/*Gyroscope output data*/
	gyro_data data;
	data.x = 0;
	data.y = 0;
	data.z = 0;
	
	/*Full-scale range value*/
	gyro_range rng = RANGE_245_DPS;
	/*Initialize the gyroscope*/
	bool init_code = init_gyro(rng);
	
	/*Strings to contain gyroscope data*/
	unsigned char x[9];
	unsigned char y[9];
	unsigned char z[9];
	
	if(init_code == GYRO_INIT_SUCCESS)
	{
		enable_autorange();
		
		while(1)
		{
			bool status = read_gyroscope(&data);
			
			/*Check	device status*/
			if(status == GYRO_READ_ERR)
			{
				lcd_erase();
				lcd_puts("READ ERR");
				break;
			}

			lcd_erase();
			/*Print formatted value into string*/
			dtostrf(data.x,4,3,x);
			dtostrf(data.y,4,3,y);
			dtostrf(data.z,4,3,z);
			/*Terminate strings*/
			x[8] = '\0';
			y[8] = '\0';
			z[8] = '\0';	
			/*Print data to LCD*/
			lcd_puts(x);
			lcd_goto_xy(1,0);
			lcd_puts(y);
			_delay_ms(1000);
			lcd_erase();
			lcd_puts(z);
			_delay_ms(1000);
			/*Clear data between readings*/
			data.x = 0;
			data.y = 0;
			data.z = 0;
		}
	}
	
	return 0;
}
