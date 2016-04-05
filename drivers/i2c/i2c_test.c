#include "i2c_lib.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

/*Fake slave device address*/
#define TEST_SLAVE_ADDR 0xFF

/*Number of bytes to write*/
#define BYTES 4

/*End of transmission condition*/
#define REPEATED_START 1
#define STOP_CONDITION 0

/*LED I/O*/ 
#define LED  2

int main()
{
	/*Configure LED as output*/
	DDRC |= (1 << LED);

	/*Response code of MT init function*/
	bool mt_init_code;
	/*Response code of MT write function*/
	uint8_t mt_write_code;
	
	init_i2c();
	
	uint8_t data[BYTES] = {0x01,0x02,0x03,0x04};
	
	mt_init_code = i2c_init_mt_mode(TEST_SLAVE_ADDR);
	
	/*Write the data if initialize successfully*/
	if(mt_init_code == MT_ENTRY_SUCCESS)
	{
		mt_write_code = i2c_mt_write(data,BYTES,STOP_CONDITION);

		if(mt_write_code == MT_WRITE_SUCCESS)
		{
			/*Light LED to indicate success*/
			PORTC |= (1 << LED);
		}
	}
	
	return 0;
}
