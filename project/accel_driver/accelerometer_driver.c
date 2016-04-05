/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Implementation of driver for the Adafruit LSM303 Accelerometer.
 *    Intended for use with an AVR microcontroller. The custom
 *    "i2c_lib.h" API is required. See accelerometer_driver.h for
 *    further details.
 *
 *	  *Based on the Adafruit Industries implementation developed by
 *     Kevin Townsend.
 *
 **************************************************************/
#include "accelerometer_driver.h"
#include "i2c_lib.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

/*Device Address*/
#define ACCEL_ADDR 0x19

/*Register Definitions*/
#define CTRL_REG1_A 0x20
#define OUT_X_L_A	0x28
#define OUT_X_H_A	0x29
#define OUT_Y_L_A	0x2A
#define OUT_Y_H_A	0x2B
#define OUT_Z_L_A	0x2C
#define OUT_Z_H_A	0x2D

/*Control Register1 Bits*/
#define ODR3 7
#define ODR2 6
#define ODR1 5
#define ODR0 4 
#define Z_EN 2
#define Y_EN 1
#define X_EN 0

/*Transmission end selection*/
#define REPEATED_START 1
#define STOP_CONDITION 0

/*Automatically Increment Register Number*/
#define AUTO_INCREMENT 7

/*Misc. Size Definitions*/
#define BYTE  	  1
#define WORD  	  8
#define WRITE 	  2
#define REG   	  0
#define VAL   	  1
#define HALF_WORD 4

/*Buffer positions for read reg functions*/
#define X_LO 0
#define X_HI 1
#define Y_LO 2
#define Y_HI 3
#define Z_LO 4
#define Z_HI 5

/* Static Function Prototypes */
static bool write_accel_reg(uint8_t reg, uint8_t value);
static bool read_n_consec_regs(uint8_t *buff, uint8_t reg, uint8_t n);

/*Read a byte from n consecutive accelerometer registers into a buffer*/
static bool read_n_consec_regs(uint8_t *buff, uint8_t reg, uint8_t n)
{
	bool mt_init = i2c_init_mt_mode(ACCEL_ADDR);
	/*Set the auto-increment bit*/
	uint8_t _reg = (reg | (1 << AUTO_INCREMENT));
	
	if(mt_init == ENTRY_SUCCESS)
	{
		/*Specify the first reg to read and enable auto-increment*/
		uint8_t mt_write = i2c_mt_write(&_reg,BYTE,REPEATED_START);
		
		if(mt_write == MT_WRITE_SUCCESS)
		{
			bool mr_init = i2c_init_mr_mode(ACCEL_ADDR);
		
			if(mr_init == ENTRY_SUCCESS)
			{
				/*Read n bytes into the buffer*/
				uint8_t mr_read = i2c_mr_read(buff,n,STOP_CONDITION);
				
				if(mr_read == MR_READ_SUCCESS)
				{
					/*Indicate read was successful*/
					return ACCEL_READ_SUCCESS;
				}
			}
		}
	}
	
	return ACCEL_READ_ERR;
}

/*Write a accelerometer register*/
static bool write_accel_reg(uint8_t reg, uint8_t value)
{
	/*Register and data buffer*/
	uint8_t data[WRITE];
	data[REG] = reg;
	data[VAL] = value;
	/*Setup I2C write*/
	bool mt_init = i2c_init_mt_mode(ACCEL_ADDR);
	
	if(mt_init == ENTRY_ERR) return ACCEL_WRITE_ERR;
	
	else
	{
		/*Write the register*/
		uint8_t mt_write = i2c_mt_write(data,WRITE,STOP_CONDITION);
		
		if(mt_write == MT_WRITE_SUCCESS)
		{
			return ACCEL_WRITE_SUCCESS;
		}
	}
	
	return ACCEL_WRITE_ERR;
}

/* Accelerometer API */

/*See accelerometer_driver.h for details*/
bool init_accel(void)
{
	uint8_t ctrl_val = 0;
	/*Set up the TWI hardware*/
	init_i2c();
	/*Set Data Rate at 10Hz and enable Z-Axis, Y-Axis, and X-Axis*/
	uint8_t data = ((1 << ODR1) | (1 << Z_EN) | (1 << Y_EN) | (1 << X_EN));
	/*Write the control register and enable accelerometer*/
	bool status = write_accel_reg((uint8_t)CTRL_REG1_A,data);
	/*Read back control register to ensure proper configuration*/
	if(status == ACCEL_WRITE_SUCCESS)
	{
		read_n_consec_regs(&ctrl_val,(uint8_t)CTRL_REG1_A,1);
		/*Return success if control register value is correct*/
		if(ctrl_val == data) return ACCEL_INIT_SUCCESS;
	}

	return ACCEL_INIT_FAILED;
}

/*See accelerometer_driver.h for details*/
bool read_accel(accel_data *data)
{
	uint8_t xl = 0, xh = 0;
	uint8_t yl = 0, yh = 0;
	uint8_t zl = 0, zh = 0;
	/*Number of data register to be read*/
	uint8_t n = 6;
	uint8_t buffer[n];
	
	/*Read 6 consecutive data regs beginning with OUT_X_L_A*/
	bool read_status = read_n_consec_regs(buffer,(uint8_t)OUT_X_L_A,n);

	if(read_status == ACCEL_READ_ERR)
	{
		return ACCEL_READ_ERR;
	}
	/*Assign register values to temp variables*/
	xl = buffer[X_LO];
	xh = buffer[X_HI];
	yl = buffer[Y_LO];
	yh = buffer[Y_HI];
	zl = buffer[Z_LO];
	zh = buffer[Z_HI];
	/*Assemble 16-bit signed integers*/
	data->x = (int16_t)(xl | (xh << WORD)) >> HALF_WORD;
	data->y = (int16_t)(yl | (yh << WORD)) >> HALF_WORD;
	data->z = (int16_t)(zl | (zh << WORD)) >> HALF_WORD;

	return ACCEL_READ_SUCCESS;
}
/*End accelerometer_driver.c*/
