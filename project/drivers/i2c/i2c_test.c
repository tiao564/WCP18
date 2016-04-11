#include "i2c_lib.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

/*Fake slave device address*/
#define TEST_SLAVE_ADDR 0xFF

/*Number of bytes to write*/
#define BYTES 4

/*End of transmission condition*/
#define START 1
#define STOP  0

/*LED I/O*/ 
#define LED  2

int main()
{
	/*Configure LED as output*/
	DDRC |= (1 << LED);

	//Store i2c error codes
	i2c_err status;
	
	init_i2c();
	
	uint8_t data[BYTES] = {0x01,0x02,0x03,0x04};
	
	status = i2c_init_mt_mode(TEST_SLAVE_ADDR);
	
	/*Write the data if initialize successfully*/
	if(status == ENTRY_PASS)
	{
		status = i2c_mt_write(data,BYTES,STOP);

		if(status == MT_WRITE_PASS)
		{
			/*Light LED to indicate success*/
			PORTC |= (1 << LED);
		}
	}
	
	return 0;
}
