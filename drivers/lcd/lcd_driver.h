/*Nick Shanahan*/

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

/*Control signal ports*/
#define RS_PORT B
#define RW_PORT B
#define ENABLE_PORT B

/*Pin locations*/
#define RS_LOC 0
#define RW_LOC 1
#define ENABLE_LOC 2

/*Data bit ports*/
#define DB0_PORT B
#define DB1_PORT B
#define DB2_PORT B
#define DB3_PORT B

/*Pin locations*/
#define DB0_LOC 3
#define DB1_LOC 4
#define DB2_LOC 5
#define DB3_LOC 6

/*LCD initialization routine*/
void initialize_LCD_driver(void);

/*Moves cursors to given coordinates*/
void lcd_goto_xy(uint8_t row, uint8_t column);

/*Clears the entire display*/
void lcd_erase(void);

/*Writes specified character at current cursor position*/
void lcd_putchar(unsigned char x);

/*Prints a string starting at current cursor position*/
void lcd_puts(unsigned char *str);

#endif
