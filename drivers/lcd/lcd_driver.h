/***************************************************************
 * Nicholas Shanahan (2015)
 *
 * DESCRIPTION:
 *  - 
 *
 **************************************************************/
 
#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

/*Control Signal Ports*/
#define RS_PORT B
#define RW_PORT B
#define ENABLE_PORT B

/*Pin Locations*/
#define RS_LOC 0
#define RW_LOC 1
#define ENABLE_LOC 2

/*Data Bit Ports*/
#define DB0_PORT B
#define DB1_PORT B
#define DB2_PORT B
#define DB3_PORT B

/*Pin Locations*/
#define DB0_LOC 3
#define DB1_LOC 4
#define DB2_LOC 5
#define DB3_LOC 6

/***************************************************************
 *
 * DESCRIPTION:
 *  - Configures LCD I/O pins and performs initialization
 *    sequence.
 *
 **************************************************************/
void initialize_LCD_driver(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Moves LCD cursor to given coordinates. Does not guarantee
 *    cursor will be visible on the display. The user must pass
 *    x and y position values to the function. The "row" input
 *    value should not exceed 1.
 *
 **************************************************************/
void lcd_goto_xy(uint8_t row, uint8_t column);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Clears the entire display.
 *
 **************************************************************/
void lcd_erase(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Writes the specified character to the display at the current
 *    cursor position.
 *
 **************************************************************/
void lcd_putchar(char x);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Prints the specified string to the display beginning at the
 *    present cursor position. Does NOT wrap string to the next
 *    line.
 *
 **************************************************************/
void lcd_puts(char *str);

#endif
/* End of lcd.h */