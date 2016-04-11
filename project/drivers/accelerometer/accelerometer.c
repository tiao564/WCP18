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
 
#include "accelerometer.h"
#include "i2c_lib.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

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

//Repeated Start Cond
#define START 1
//Stop Condition
#define STOP  0

/*Automatically Increment Register Number*/
#define AUTO_INCREMENT 7

/*Misc. Size Definitions*/
#define BYTE  	  1
#define WORD  	  8
#define REG   	  0
#define VAL   	  1
#define WRITE     2 
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
	//store i2c error codes
	i2c_err status;
	
	status = i2c_init_mt_mode(ACCEL_ADDR);
	/*Set the auto-increment bit*/
	reg |= (1 << AUTO_INCREMENT);
	
	if(status == ENTRY_PASS)
	{
		/*Specify the first reg to read and enable auto-increment*/
		status = i2c_mt_write(&reg,BYTE,START);
		
		if(status == MT_WRITE_PASS)
		{
			/*Enter master receiver mode*/
			status = i2c_init_mr_mode(ACCEL_ADDR);
		
			if(status == ENTRY_PASS)
			{
				/*Read n bytes into the buffer*/
				status = i2c_mr_read(buff,n,STOP);
				
				if(status == MR_READ_PASS)
				{
					return ACCEL_READ_PASS;
				}
			}
		}
	}
	
	return ACCEL_READ_FAIL;
}

/*Write a accelerometer register*/
static bool write_accel_reg(uint8_t reg, uint8_t value)
{
	//store i2c error codes
	i2c_err status;
	
	/*Register and data buffer*/
	uint8_t data[2];
	data[REG] = reg;
	data[VAL] = value;
	
	/*Setup I2C write*/
	status = i2c_init_mt_mode(ACCEL_ADDR);
	
	if(status == ENTRY_PASS)
	{
		/*Write the register*/
		status = i2c_mt_write(data,WRITE,STOP);
		
		if(status == MT_WRITE_PASS)
		{
			return ACCEL_WRITE_PASS;
		}
	}
	
	return ACCEL_WRITE_FAIL;
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
	if(status == ACCEL_WRITE_PASS)
	{
		read_n_consec_regs(&ctrl_val,(uint8_t)CTRL_REG1_A,1);
		/*Return success if control register value is correct*/
		if(ctrl_val == data) return ACCEL_INIT_PASS;
	}

	return ACCEL_INIT_FAIL;
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
	
	/*Read 6 consecutive data regs beginning with OUT_X_L_A and ending OUT_Z_H_A*/
	bool read_status = read_n_consec_regs(buffer,(uint8_t)OUT_X_L_A,n);

	if(read_status == ACCEL_READ_FAIL)
	{
		return ACCEL_READ_FAIL;
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

	return ACCEL_READ_PASS;
}
/*End accelerometer_driver.c*/
