#include <avr/io.h>
#include <util/delay.h>

int main()
{
	while(1)
	{
		DDRD |= (1 << 5);
		PORTD |= (1 << 5);
		_delay_ms(1000);
		PORTD &= ~(1 << 5);
	}
}
