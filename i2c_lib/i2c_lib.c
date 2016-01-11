/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Implementation of I2C API that facilitates use of the AVR
 *    microcontroller TWI hardware. See i2c_lib.h for additional
 *    details.
 *
 **************************************************************/
 
#include "i2c_lib.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*Bit Rate Register value*/
#define BIT_RATE 0x5C //92 --> 100KHz (Standard Mode)

/*TWI Status Register Pre-scaler bits*/
#define TWPS1 1
#define TWPS0 0

/*TWI Control Register Bits*/
#define TWINT 7
#define TWEA  6
#define TWSTA 5
#define TWSTO 4
#define TWEN  2

/*Mask off TWSR status bits*/
#define STATUS_MSK 0xF8

/*Apply to slave address*/
#define WRITE 0x01
#define READ  0xFE

/* Static Function Prototypes */
static void send_start_cond(void);
static void send_stop_cond(void);
static void send_repeated_start(void);
static void transmit_byte(void);
static void send_ack(void);
static void send_non_ack(void);
static void clear_twint_flag(void);
static bool get_twint_flag(void);
static void twi_busy_wait(void);

/*Start condition to establish MCU as the master*/
static void send_start_cond(void)
{
	TWCR |= ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN));
}

/*Stop condition to reliquinsh control of bus*/
static void send_stop_cond(void)
{
	TWCR |= ((1 << TWINT) | (1 << TWSTO) | (1 << TWEN));
}

/*Wrapper that re-issues the start condition*/
static void send_repeated_start(void)
{
	send_start_cond();
}

/*Transmit byte by clearing TWINT*/
static void transmit_byte(void)
{
	TWCR |= ((1 << TWINT) | (1 << TWEN));
}

/*Acknowledgement for use in Master Receiver Mode*/
static void send_ack(void)
{
	TWCR |= ((1 << TWINT) | (1 << TWEA) | (1 << TWEN));
}

/*Non-acknowledgement for use in Master Receiver Mode*/
static void send_non_ack(void)
{
	TWCR = (TWCR & ~(1 << TWSTO)) | ((1 << TWINT) | (1 << TWEN));
}

/*Clear TWINT so a new byte may be read*/
static void clear_twint_flag(void)
{
	TWCR |= (1 << TWINT);
}

/*Return the value of the TWINT flag*/
static bool get_twint_flag(void)
{
	return (TWCR & (1 << TWINT));
}

/*Polls the TWI Control Register interrupt flag to see if busy*/
static void twi_busy_wait(void)
{
	while(!(TWCR & (1 << TWINT)));
}

/* API Functions */

/*See i2c_lib.h for details*/
void init_i2c(void)
{
	/*No pre-scaler*/
	TWSR &= ~((1 << TWPS0) | (1 << TWPS1));	
	/*Set SCL frequency*/
	TWBR = BIT_RATE;
	/*Enable TWI operation*/
	TWCR |= (1 << TWEN);
}

/*See i2c_lib.h for details*/
bool i2c_init_mt_mode(uint8_t slave_addr)
{
	/*Establish as master device*/
	send_start_cond();	
	/*Wait for start condition to be sent*/
	twi_busy_wait();	
	
	/*Check that start condition was successful*/
	if((TWSR & STATUS_MSK) != START_ACK)
	{
		return MT_ENTRY_ERR;
	}
	
	/*Load TWDR with SLA+W - assert LSB to indicate write*/
	TWDR |= ((slave_addr << 1) | WRITE);
	transmit_byte();	
	/*Wait for slave address to be sent*/
	twi_busy_wait();	
	
	/*Check that SLA+W was successfully transmitted*/
	if((TWSR & STATUS_MSK) != MT_SLA_ACK)
	{
		return MT_ENTRY_ERR;
	}
	
	return MT_ENTRY_SUCCESS;
}

/*See i2c_lib.h for details*/
uint8_t i2c_mt_write(uint8_t *data, uint8_t bytes, bool repeated_start)
{
	for(uint8_t i = 0; i < bytes; i++)
	{
		/*Write data byte into TWI Data Register*/
		TWDR |= data[i];
		/*Send data*/
		transmit_byte();
		/*Wait for byte to be sent*/
		twi_busy_wait();
		
		/*Check that data byte was acknowledged*/
		if((TWSR & STATUS_MSK) != MT_DATA_ACK)
		{
			return MT_WRITE_ERR;
		}
	}
	
	if(repeated_start)
	{
		/*Maintain control of bus*/
		send_repeated_start();
		/*Wait for repeated start condition to be sent*/
		twi_busy_wait();	
	
		/*Check that repeated start condition was successful*/
		if((TWSR & STATUS_MSK) != REPEATED_START_ACK)
		{
			return MT_REENTRY_ERR;
		}
	}
	
	else
	{
		/*Relinquish control of bus*/
		send_stop_cond();	
	}
	
	return MT_WRITE_SUCCESS;
}

/*See i2c_lib.h for details*/
bool i2c_init_mr_mode(uint8_t slave_addr)
{
	/*Establish as master device*/
	send_start_cond();	
	/*Wait for start condition to be sent*/
	twi_busy_wait();

	/*Check that start condition was successful*/
	if((TWSR & STATUS_MSK) != START_ACK)
	{
		return MR_ENTRY_ERR;
	}
	
	/*Load TWDR with SLA+R - clear LSB to indicate read*/
	TWDR |= ((slave_addr << 1) & READ);
	transmit_byte();
	/*Wait for slave address to be sent*/
	twi_busy_wait();
	
	/*Check that SLA+R was successfully transmitted*/
	if((TWSR & STATUS_MSK) != MR_SLA_ACK)
	{
		return MR_ENTRY_ERR;
	}
	
	return MR_ENTRY_SUCCESS;
}

/*See i2c_lib.h for details*/
uint8_t i2c_mt_read(uint8_t *data, uint8_t bytes, bool repeated_start)
{
	uint8_t bytes_read = 0;
	
	/*Ensure buffer is not null*/
	if(data == NULL)
	{
		return MR_READ_ERR;
	}
	
	while(bytes_read < bytes)
	{
		/*Check if a new byte is available*/
		if(get_twint_flag())
		{
			/*Read byte into data buffer*/
			data[bytes_read] = TWDR;
			/*Track how many bytes have been read*/
			bytes_read++;
			
			/*Issue Non-Acknowledgement after last byte is read*/
			if(bytes_read == bytes)
			{
				send_non_ack();
			}
			/*Acknowledge the received byte*/
			else
			{
				send_ack();
			}
			
			/*Clear TWINT after reading into buffer*/
			clear_twint_flag();
		}
	}
	
	if(repeated_start)
	{
		/*Maintain control of bus*/
		send_repeated_start();
		/*Wait for repeated start condition to be sent*/
		twi_busy_wait();	
	
		/*Check that repeated start condition was successful*/
		if((TWSR & STATUS_MSK) != REPEATED_START_ACK)
		{
			return MR_REENTRY_ERR;
		}	
	}
	
	else
	{
		/*Relinquish control of bus*/
		send_stop_cond();
	}
	
	return MR_READ_SUCCESS;
}
/* End of i2c_lib.c */