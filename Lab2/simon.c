/*
Codigo para Simon Says del Laboratorio 2
*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Banderas de botones
int yellow = 0;
int red = 0;
int green = 0;
int blue = 0;

// recieve inputs
ISR(INT0_vect)
{
  // Boton amarillo TL
  yellow = 1;
}
ISR(INT1_vect)
{
  // Boton rojo TR
  red = 1;
}
ISR(PCINT0_vect)
{
  // Boton verde BL
  green = 1;
}
ISR(PCINT1_vect)
{
  // Boton azul BR
  blue = 1;
}

int main(void)
{
  DDRB = 0x3C; //Configuracion del puertos
  GIMSK |= (1<<INT0)|(1<<INT1);
  GIMSK |= (1<<PCIE0)|(1<<PCIE1);
  PCMSK |= 0x03;
  PCMSK1 |= 0x01;
  sei();

  int init = 0;
  int iteracion = 4;
  int velocidad = 2000;
  int valid = 1;
  int spot = 0;
  int arrInput[10] = {0,0,0,0,0,0,0,0,0,0};
  int arrGen[10] = {0,1,2,3,0,1,2,3,0,1};
  
  while (1) {
	  
    if((yellow) | (red) | (green) | (blue))
      {
	init = 1;
	// Flash LEDs 2 times
	PORTB = 0x00; _delay_ms(5000);
	PORTB = 0x3C; _delay_ms(5000);
	PORTB = 0x00; _delay_ms(5000);
	PORTB = 0x3C; _delay_ms(5000);
	PORTB = 0x00; _delay_ms(5000);
	// Reset flags
	yellow = 0;
	red = 0;
	green = 0;
	blue = 0;
      }
    while(init)
      {
	for(int i = 0; i < iteracion; i++)
	  {
	    // Flash LED sequence
	    if(arrGen[i] == 0)
	      {
		PORTB = 0x04; _delay_ms(5000);
		PORTB = 0x00; _delay_ms(500);
	      }
	    if(arrGen[i] == 1)
	      {
		PORTB = 0x08; _delay_ms(5000);
		PORTB = 0x00; _delay_ms(500);
	      }
	    if(arrGen[i] == 2)
	      {
		PORTB = 0x10; _delay_ms(5000);
		PORTB = 0x00; _delay_ms(500);
	      }
	    if(arrGen[i] == 3)
	      {
		PORTB = 0x20; _delay_ms(5000);
		PORTB = 0x00; _delay_ms(500);
	      }
	  }
	
	while(spot < iteracion)
	  {
	    // get inputs
	    if(yellow)
	      {
		arrInput[spot] = 0;
		spot = spot + 1;
	      }
	    if(red)
	      {
		arrInput[spot] = 1;
		spot = spot + 1;
	      }
	    if(green)
	      {
		arrInput[spot] = 2;
		spot = spot + 1;
	      }
	    if(blue)
	      {
		arrInput[spot] = 3;
		spot = spot + 1;
	      }
	  }

	for(int i = 0; i < iteracion; i++)
	  {
	    // Check sequence
	    if(arrInput[i] != arrGen[i])
	      {
		valid = 0;
	      }
	  }
			
	if(!valid)
	  {
	    // Flash LEDs 3 times
	    init = 0;
	    PORTB = 0x00; _delay_ms(500);
	    PORTB = 0x3C; _delay_ms(500);
	    PORTB = 0x00; _delay_ms(500);
	    PORTB = 0x3C; _delay_ms(500);
	    PORTB = 0x00; _delay_ms(500);
	    PORTB = 0x3C; _delay_ms(500);
	  }
	iteracion = iteracion + 1;
	velocidad = velocidad - 200;
      }
  }
}

