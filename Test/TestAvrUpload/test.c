#include <avr/io.h>

int main()
{
  DDRB |= (1 << 0);
  PORTB |= (1 << 0);

  return 0;
}
