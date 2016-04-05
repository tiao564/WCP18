/*Nick Shanahan*/
 
#define F_CPU 20000000

#include "lcd_driver.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>

/*concatenation macros*/
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

#define BUSY_FLAG (1 << 7)

/*LCD registers*/
#define INSTRUCTION 0
#define DATA 1

#define DDRAM_BASE 	0x80
#define DDRAM_MAX 	0xCF
#define COLUMN_MASK 0x0F
#define ROW_BIT 6

/*********** LCD helper functions ***********/

static void config_rs_as_output(void)
{
	DDR(RS_PORT) |= (1 << RS_LOC);
}

static void config_rw_as_output(void)
{
	DDR(RW_PORT) |= (1 << RW_LOC);
}

static void config_enable_as_output(void)
{
	DDR(ENABLE_PORT) |= (1 << ENABLE_LOC);
}

static void config_db0_as_output(void)
{
	DDR(DB0_PORT) |= (1 << DB0_LOC);
}

static void config_db1_as_output(void)
{
	DDR(DB1_PORT) |= (1 << DB1_LOC);
}

static void config_db2_as_output(void)
{
	DDR(DB2_PORT) |= (1 << DB2_LOC);
}

static void config_db3_as_output(void)
{
	DDR(DB3_PORT) |= (1 << DB3_LOC);
}

static void config_db0_as_input(void)
{
	DDR(DB0_PORT) &= ~(1 << DB0_LOC);
}

static void config_db1_as_input(void)
{
	DDR(DB1_PORT) &= ~(1 << DB1_LOC);
}

static void config_db2_as_input(void)
{
	DDR(DB2_PORT) &= ~(1 << DB2_LOC);
}

static void config_db3_as_input(void)
{
	DDR(DB3_PORT) &= ~(1 << DB3_LOC);
}

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

static void set_db0_bit(void)
{
	PORT(DB0_PORT) |= (1 << DB0_LOC);
}

static void set_db1_bit(void)
{
	PORT(DB1_PORT) |= (1 << DB1_LOC);
}

static void set_db2_bit(void)
{
	PORT(DB2_PORT) |= (1 << DB2_LOC);
}

static void set_db3_bit(void)
{
	PORT(DB3_PORT) |= (1 << DB3_LOC);
}

static void clear_rs_bit(void)
{
	PORT(RS_PORT) &= ~(1 << RS_LOC);
}

/*Sets the LCD register to write mode*/
static void clear_rw_bit(void)
{
	PORT(RW_PORT) &= ~(1 << RW_LOC);
}

static void clear_enable_bit(void)
{
	PORT(ENABLE_PORT) &= ~(1 << ENABLE_LOC);
}

static void clear_db0_bit(void)
{
	PORT(DB0_PORT) &= ~(1 << DB0_LOC);
}

static void clear_db1_bit(void)
{
	PORT(DB1_PORT) &= ~(1 << DB1_LOC);
}

static void clear_db2_bit(void)
{
	PORT(DB2_PORT) &= ~(1 << DB2_LOC);
}

static void clear_db3_bit(void)
{
	PORT(DB3_PORT) &= ~(1 << DB3_LOC);
}

static bool get_db0_bit(void)
{
	return (PIN(DB0_PORT) & (1 << DB0_LOC)); 
}

static bool get_db1_bit(void)
{
	return (PIN(DB1_PORT) & (1 << DB1_LOC)); 
}

static bool get_db2_bit(void)
{
	return (PIN(DB2_PORT) & (1 << DB2_LOC)); 
}

static bool get_db3_bit(void)
{
	return (PIN(DB3_PORT) & (1 << DB3_LOC)); 
}

static void config_bus_as_input(void)
{
	config_db0_as_input();
	config_db1_as_input();
	config_db2_as_input();
	config_db3_as_input();
}

static void config_bus_as_ouput(void)
{
	config_db0_as_output();
	config_db1_as_output();
	config_db2_as_output();
	config_db3_as_output();
}

/** Mid-level helper routines **/

/*determines the 4bit data bus value*/
static uint8_t input_nibble_from_bus(void)
{
	uint8_t data = 0;
	
	if(get_db0_bit())
	{
		data |= (1 << 0); //set bit0 of data
	}
	
	if(get_db1_bit())
	{
		data |= (1 << 1); //set bit1 of data
	}
	
	if(get_db2_bit())
	{
		data |= (1 << 2); //set bit2 of data
	}
	
	if(get_db3_bit())
	{
		data |= (1 << 3); //set bit3 of data
	}
	
	return data;
}

/*reads the 4bit data bus value*/
static uint8_t pulse_enable(void)
{
	uint8_t data;
	set_enable_bit();
	_delay_ms(1);
	data = input_nibble_from_bus();
	clear_enable_bit();
	return data;
}

static void output_nibble_to_bus(uint8_t data)
{
	/*determine db0 value*/
	bool bit0 = (data & (1 << 0));
	bool bit1 = (data & (1 << 1));
	bool bit2 = (data & (1 << 2));
	bool bit3 = (data & (1 << 3));
	
	/*determine db0 value*/
	if(bit0)
	{
		set_db0_bit();
	}
	
	else
	{
		clear_db0_bit();
	}
	
	/*determine db1 value*/
	if(bit1)
	{
		set_db1_bit();
	}
	
	else
	{
		clear_db1_bit();
	}
	
	/*determine db2 value*/
	if(bit2)
	{
		set_db2_bit();
	}
	
	else
	{
		clear_db2_bit();
	}
	
	/*determine db3 value*/
	if(bit3)
	{
		set_db3_bit();
	}
	
	else
	{
		clear_db3_bit();
	}
	
	pulse_enable();
}

/*read either the data or instruction reg value*/
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

static void busy_wait(void)
{
	uint8_t reg_value = 0;
	bool busy_flag_bit;
	
	//poll busy flag until 0
	do
	{
		reg_value = read_register(INSTRUCTION);
		busy_flag_bit = (reg_value & BUSY_FLAG);
	}while(busy_flag_bit);
}

/*write either the data or instruction reg*/
static void write_register(uint8_t data, bool reg_select)
{
	busy_wait();
	clear_rw_bit(); //select writing mode
	
	/*select instruction or data reg*/
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
	//output_nibble_to_bus(0x70); //enable pull-up resistors
}

/*********** LCD API ***********/

/*LCD initialization routine*/
void initialize_LCD_driver(void)
{
	clear_rw_bit();
	clear_rs_bit();
	clear_enable_bit();
	
	config_rs_as_output();
	config_rw_as_output();
	config_enable_as_output();
	config_bus_as_ouput();
	
	_delay_ms(15);
	output_nibble_to_bus(0x3);
	_delay_ms(6);
	output_nibble_to_bus(0x3);
	_delay_ms(2);
	output_nibble_to_bus(0x3);
	_delay_ms(2);
	output_nibble_to_bus(0x2);
	busy_wait();
	write_register(0x28,INSTRUCTION);
	write_register(0x08,INSTRUCTION);
	write_register(0x01,INSTRUCTION);
	write_register(0x06,INSTRUCTION);
	
	//turn display on
	write_register(DISPLAY_ON,INSTRUCTION);
}

/*Moves cursors to given coordinates*/
void lcd_goto_xy(uint8_t row, uint8_t column)
{	
	uint8_t ddram_addr = DDRAM_BASE;
	uint8_t row_offset_bit = ROW_BIT;
	
	if(row == 1)
	{
		//set row position to 1
		ddram_addr |= (1 << row_offset_bit);
	}
	
	ddram_addr += column; //add column offset to address
	
	if((ddram_addr >= DDRAM_BASE) && (ddram_addr <= DDRAM_MAX))
	{
		//set the ddram addr in the instruction register
		write_register(ddram_addr,INSTRUCTION);
	}
}

/*Clears the entire display*/
void lcd_erase(void)
{
	write_register(CLEAR_LCD,INSTRUCTION);
}

/*Writes specified character at current cursor position*/
void lcd_putchar(unsigned char x)
{
	uint8_t character = (uint8_t)x; //get ASCII value of character
	write_register(character,DATA); //write ASCII value to CGRAM
}

/*Prints a string starting at current cursor position*/
void lcd_puts(unsigned char *str)
{
	uint8_t i = 0;
	
	/*print characters until null terminator is reached*/
	while(str[i] != '\0')
	{
		lcd_putchar(str[i]);
		i++;
	}
}
