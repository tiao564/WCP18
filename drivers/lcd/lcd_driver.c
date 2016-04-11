/***************************************************************
 * Nicholas Shanahan (2015)
 *
 * DESCRIPTION:
 *  - 
 *
 **************************************************************/
 
/*Processor Frequency*/
#define F_CPU 8000000UL

#include "lcd_driver.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>

/*Concatenation Macros*/
#define CONCAT(A,B) A##B
#define DDR(letter) CONCAT(DDR,letter)
#define PORT(letter) CONCAT(PORT,letter)
#define PIN(letter) CONCAT(PIN,letter)

/*LCD Commands*/
#define RETURN_HOME 	0x02
#define BLINK_CURSOR 	0x0D
#define CLEAR_LCD 	 	0x01
#define WRITE_CHARACTER 0x40
#define DISPLAY_ON 		0x0C

/*Instruction Register bit that 
indicates the LCD is busy */
#define BUSY_FLAG (1 << 7)

/*LCD registers*/
#define INST 0
#define DATA 1

/*DDRAM Macros*/
#define DDRAM_BASE 	0x80
#define DDRAM_MAX 	0xCF

/*DDRAM address bit that determines LCD row*/
#define ROW_BIT 6

/*LCD Low-Level Function Prototypes*/
static void config_rs_as_output(void);
static void config_rw_as_output(void);
static void config_enable_as_output(void);
static void config_bus_as_input(void);
static void config_bus_as_ouput(void);
static void set_rs_bit(void);
static void set_rw_bit(void);
static void set_db0_bit(void);
static void set_db1_bit(void);
static void set_db2_bit(void);
static void set_db3_bit(void);
static void clear_rs_bit(void);
static void clear_rw_bit(void);
static void clear_db0_bit(void);
static void clear_db1_bit(void);
static void clear_db2_bit(void);
static void clear_db3_bit(void);
static bool get_db0_bit(void);
static bool get_db1_bit(void);
static bool get_db2_bit(void);
static bool get_db3_bit(void);

/*LCD Mid-Level Function Prototypes*/
static uint8_t input_nibble_from_bus(void);
static void output_nibble_to_bus(uint8_t data);
static uint8_t pulse_enable(void);
static uint8_t read_register(bool reg_select);
static void write_register(uint8_t data, bool reg_select);
static void busy_wait(void);

/** LCD Low-Level Functions **/

/*Configure Reset pin as output*/
static void config_rs_as_output(void)
{
	DDR(RS_PORT) |= (1 << RS_LOC);
}

/*Configure Read/Write pin as output*/
static void config_rw_as_output(void)
{
	DDR(RW_PORT) |= (1 << RW_LOC);
}

/*Configure Enable pin as output*/
static void config_enable_as_output(void)
{
	DDR(ENABLE_PORT) |= (1 << ENABLE_LOC);
}

/*Configure data bus as output pins*/
static void config_bus_as_input(void)
{
	DDR(DB0_PORT) &= ~(1 << DB0_LOC);
	DDR(DB1_PORT) &= ~(1 << DB1_LOC);
	DDR(DB2_PORT) &= ~(1 << DB2_LOC);
	DDR(DB3_PORT) &= ~(1 << DB3_LOC);
}

/*Configure data bus as input pins*/
static void config_bus_as_ouput(void)
{
	DDR(DB0_PORT) |= (1 << DB0_LOC);
	DDR(DB1_PORT) |= (1 << DB1_LOC);
	DDR(DB2_PORT) |= (1 << DB2_LOC);
	DDR(DB3_PORT) |= (1 << DB3_LOC);
}

/*Set the Resent bit high*/
static void set_rs_bit(void)
{
	PORT(RS_PORT) |= (1 << RS_LOC);
}

/*Sets the LCD register to read mode*/
static void set_rw_bit(void)
{
	PORT(RW_PORT) |= (1 << RW_LOC);
}

/*Enables to LCD reg to be read or written*/
static void set_enable_bit(void)
{
	PORT(ENABLE_PORT) |= (1 << ENABLE_LOC);
}

/*Write a 1 to Data Bit0*/
static void set_db0_bit(void)
{
	PORT(DB0_PORT) |= (1 << DB0_LOC);
}

/*Write a 1 to Data Bit1*/
static void set_db1_bit(void)
{
	PORT(DB1_PORT) |= (1 << DB1_LOC);
}

/*Write a 1 to Data Bit2*/
static void set_db2_bit(void)
{
	PORT(DB2_PORT) |= (1 << DB2_LOC);
}

/*Write a 1 to Data Bit3*/
static void set_db3_bit(void)
{
	PORT(DB3_PORT) |= (1 << DB3_LOC);
}

/*Clear the Reset bit*/
static void clear_rs_bit(void)
{
	PORT(RS_PORT) &= ~(1 << RS_LOC);
}

/*Sets the LCD register to write mode*/
static void clear_rw_bit(void)
{
	PORT(RW_PORT) &= ~(1 << RW_LOC);
}

/*Clear the Enbable bit*/
static void clear_enable_bit(void)
{
	PORT(ENABLE_PORT) &= ~(1 << ENABLE_LOC);
}

/*Write a 0 to Data Bit0*/
static void clear_db0_bit(void)
{
	PORT(DB0_PORT) &= ~(1 << DB0_LOC);
}

/*Write a 0 to Data Bit1*/
static void clear_db1_bit(void)
{
	PORT(DB1_PORT) &= ~(1 << DB1_LOC);
}

/*Write a 0 to Data Bit2*/
static void clear_db2_bit(void)
{
	PORT(DB2_PORT) &= ~(1 << DB2_LOC);
}

/*Write a 0 to Data Bit3*/
static void clear_db3_bit(void)
{
	PORT(DB3_PORT) &= ~(1 << DB3_LOC);
}

/*Read Data Bit0 from bus*/
static bool get_db0_bit(void)
{
	return (PIN(DB0_PORT) & (1 << DB0_LOC)); 
}

/*Read Data Bit1 from bus*/
static bool get_db1_bit(void)
{
	return (PIN(DB1_PORT) & (1 << DB1_LOC)); 
}

/*Read Data Bit2 from bus*/
static bool get_db2_bit(void)
{
	return (PIN(DB2_PORT) & (1 << DB2_LOC)); 
}

/*Read Data Bit3 from bus*/
static bool get_db3_bit(void)
{
	return (PIN(DB3_PORT) & (1 << DB3_LOC)); 
}

/** LCD Mid-Level Functions **/

/*Determines the 4 bit data bus value*/
static uint8_t input_nibble_from_bus(void)
{
	uint8_t data = 0;
	
	if(get_db0_bit())
	{
		//set bit0 of data
		data |= (1 << 0);
	}
	
	if(get_db1_bit())
	{
		//set bit1 of data
		data |= (1 << 1);
	}
	
	if(get_db2_bit())
	{
		//set bit2 of data
		data |= (1 << 2);
	}
	
	if(get_db3_bit())
	{
		//set bit2 of data
		data |= (1 << 3);
	}
	
	return data;
}

/*Reads the 4 bit data bus value*/
static uint8_t pulse_enable(void)
{
	uint8_t data = 0;
	set_enable_bit();
	_delay_ms(1);
	data = input_nibble_from_bus();
	clear_enable_bit();
	return data;
}

/*Write 4 bit nibble to data bus*/
static void output_nibble_to_bus(uint8_t data)
{
	/*determine db0 value*/
	bool bit0 = (data & (1 << 0));
	bool bit1 = (data & (1 << 1));
	bool bit2 = (data & (1 << 2));
	bool bit3 = (data & (1 << 3));
	
	/*determine db0 value*/
	(bit0) ? set_db0_bit() : clear_db0_bit();
	
	/*determine db1 value*/
	(bit1) ? set_db1_bit() : clear_db1_bit();
	
	/*determine db2 value*/
	(bit2) ? set_db2_bit() : clear_db2_bit();
	
	/*determine db3 value*/
	(bit3) ? set_db3_bit() : clear_db3_bit();
	
	pulse_enable();
}

/*Read either the data or instruction reg value*/
static uint8_t read_register(bool reg_select)
{
	uint8_t data_byte = 0;
	uint8_t lower4_bits = 0;
	uint8_t upper4_bits = 0;
	
	if(reg_select)
	{
		set_rs_bit();
	}
	
	else 
	{
		clear_rs_bit();
	}
	
	config_bus_as_input();
	set_rw_bit(); //select reading mode
	
	upper4_bits = pulse_enable(); //first 4 bits are MSBs
	lower4_bits = pulse_enable(); //second 4 bits are LSBs
	data_byte |= (upper4_bits << 4); //set upper bits
	data_byte |= lower4_bits; //set lower bits
	
	return data_byte;
}

/*Poll Instruction register busy flag*/
static void busy_wait(void)
{
	uint8_t reg_value = 0;
	bool busy_flag_bit;
	
	//poll busy flag until 0
	do
	{
		reg_value = read_register(INST);
		busy_flag_bit = (reg_value & BUSY_FLAG);
	}while(busy_flag_bit);
}

/*write either the data or instruction reg*/
static void write_register(uint8_t data, bool reg_select)
{
	busy_wait();
	//select writing mode
	clear_rw_bit();
	
	//select instruction or data register
	if(reg_select)
	{
		set_rs_bit();
	}
	
	else
	{
		clear_rs_bit();
	}
	
	config_bus_as_ouput();
	output_nibble_to_bus(data >> 4); //output 4 MSBs
	output_nibble_to_bus(data);		 //output 4 LSBs
	config_bus_as_input();
}

/** LCD API **/

/*See lcd.h for details*/
void initialize_LCD_driver(void)
{
	clear_rw_bit();
	clear_rs_bit();
	clear_enable_bit();
	
	config_rs_as_output();
	config_rw_as_output();
	config_enable_as_output();
	config_bus_as_ouput();
	
	//Initialization sequence
	_delay_ms(15);
	output_nibble_to_bus(0x03);
	_delay_ms(6);
	output_nibble_to_bus(0x03);
	_delay_ms(2);
	output_nibble_to_bus(0x03);
	_delay_ms(2);
	output_nibble_to_bus(0x02);
	busy_wait();
	
	//Configure instruction register
	write_register(0x28,INST);
	write_register(0x08,INST);
	write_register(0x01,INST);
	write_register(0x06,INST);
	
	//Turn display on
	write_register(DISPLAY_ON,INST);
}

/*See lcd.h for details*/
void lcd_goto_xy(uint8_t row, uint8_t column)
{	
	uint8_t ddram_addr = DDRAM_BASE;
	uint8_t row_offset_bit = ROW_BIT;
	
	if(row == 1)
	{
		//set row position to 1
		ddram_addr |= (1 << row_offset_bit);
	}
	
	//add column offset to address
	ddram_addr += column;
	
	/*Check that DDRAM Address values within bounds*/
	if((ddram_addr >= DDRAM_BASE) && (ddram_addr <= DDRAM_MAX))
	{
		write_register(ddram_addr,INST);
	}
}

/*See lcd.h for details*/
void lcd_erase(void)
{
	write_register(CLEAR_LCD,INST);
}

/*See lcd.h for details*/
void lcd_putchar(char x)
{
	//Get ASCII value of character
	uint8_t character = (uint8_t)x;
	//Write ASCII value to CGRAM	
	write_register(character,DATA); 
}

/*See lcd.h for details*/
void lcd_puts(char *str)
{
	uint8_t i = 0;
	/*print characters until null terminator is reached*/
	while(str[i] != '\0')
	{
		lcd_putchar(str[i]);
		i++;
	}
}
/* End of lcd.c */