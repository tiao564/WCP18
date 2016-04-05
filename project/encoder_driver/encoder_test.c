#define F_CPU 8000000UL

#include "encoder.h"
#include <avr/io.h>
#include <util/delay.h>
#include "lcd_driver.h"
#include <avr/interrupt.h>

#define ROTAT_MAX   5000
#define TRANS_MAX   5000
#define SAMPLE_RATE 200

/*From itoa.c*/
extern char *num_to_str(int i);

int main()
{
	sei();

	char *rotat_cnt_str;
	char *trans_cnt_str;
	uint16_t rotat_cnt = 0;
	uint16_t trans_cnt = 0;

	initialize_LCD_driver();	
	set_sampling_rate(SAMPLE_RATE);
	init_encoders();

	clear_rotat_encoder_cnt();	
	//Read and print the current rotational encoder count
	while(rotat_cnt < ROTAT_MAX)
	{
		rotat_cnt = get_rotat_encoder_cnt();
    }

	lcd_erase();
	lcd_puts("RE_DONE");	

	clear_trans_encoder_cnt();
	//Read and print the current translational encoder count
	while(trans_cnt < TRANS_MAX)
	{
		trans_cnt = get_trans_encoder_cnt();
	}

	lcd_erase();
	lcd_puts("TE_DONE");
	
	stop_encoders();

	return 0;
}
