/*Processor Clock Frequency*/
#define F_CPU 8000000UL

#include "accelerometer.h"
#include "lcd_driver.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "i2c_lib.h"

int main()
{
	initialize_LCD_driver();

	/*Accelerometer data structure*/
	accel_data data;
	data.x = 0;
	data.y = 0;
	data.z = 0;
	
	/*Initialize the accelerometer*/
	bool init_code = init_accel();

	/*Strings to contain accel data*/
	unsigned char x[9];
	unsigned char y[9];
	unsigned char z[9];

	if(init_code == ACCEL_INIT_SUCCESS)
	{
		while(1)
		{
			bool status = read_accel(&data);
			
			/*Check	device status*/
			if(status == ACCEL_READ_ERR)
			{
				lcd_erase();
				lcd_puts("READ ERR");
				break;
			}

			lcd_erase();
			/*Print formatted value into string*/
			sprintf(x,"x:%x",data.x);
			sprintf(y,"y:%x",data.y);
			sprintf(z,"z:%x",data.z);
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
