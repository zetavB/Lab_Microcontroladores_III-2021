/*
Codigo para Simon Says del Laboratorio 2
*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

// Banderas de botones
int yellow = 0;
int red = 0;
int green = 0;
int blue = 0;
// Flags para debounce
int deb0 = 0;
int deb1 = 0;

// Tiempo de LED
int ledTimer = 10;
int ledValid = 0;
int initrCount = 0;
int deRep = 1;

// recieve inputs
ISR(INT0_vect)
{
  // Boton amarillo TL
  yellow = 1;
  PORTB = 0x04; _delay_ms(5000);
  PORTB = 0x00; _delay_ms(500);
}
ISR(INT1_vect)
{
  // Boton rojo TR
  red = 1;
  PORTB = 0x08; _delay_ms(5000);
  PORTB = 0x00; _delay_ms(500);
}
ISR(PCINT0_vect)
{
  // Boton verde BL
  if(deb0)
    {
      green = 1;
      deb0 = 0;
      PORTB = 0x10; _delay_ms(5000);
      PORTB = 0x00; _delay_ms(500);
    }else
    {
      green = 0;
      deb0 = 1;
    }
}
ISR(PCINT1_vect)
{
  // Boton azul BR
  if(deb1)
    {
      blue = 1;
      deb1 = 0;
      PORTB = 0x20; _delay_ms(5000);
      PORTB = 0x00; _delay_ms(500);
    }else
    {
      blue = 0;
      deb1 = 1;
    }
}

ISR(TIMER0_OVF_vect)
{
  if(initrCount == ledTimer)
    {
      ledValid = 1;
      initrCount = 0;
      //PORTB = 0x04; _delay_ms(5000);
      
    }else
    {
      initrCount++;
      
      //PORTB = 0x00; _delay_ms(5000);
    }
}

void shuffle(int *array, size_t n)
{
  if (n > 1) 
    {
      size_t i;
      for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void delay_ms(unsigned int milliseconds)
 {
   while(milliseconds > 0)
     {
       _delay_ms(1);
       milliseconds--;
     }
 }

int main(void)
{
  DDRB = 0x3C; //Configuracion del puertos
  //GIMSK |= (1<<INT0)|(1<<INT1);
  //GIMSK |= (1<<PCIE0)|(1<<PCIE1);
  GIMSK |= 0xE8;
  PCMSK |= 0x03;
  PCMSK1 |= 0x03;
  TCCR0A |= 0x00;
  TCCR0B |= 0x05;
  sei();
  TCNT0 = 0;
  TIMSK |= (1<<TOIE0);
  

  int init = 0;
  int iteracion = 4;
  int velocidad = 10000;
  int valid = 1;
  int spot = 0;
  int arrInput[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int arrGen[20] = {0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1};
  
  shuffle(arrGen, 20);
  
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
	//int nada = 0;
	for(int i = 0; i < iteracion; i++)
	  {
	    // Flash LED sequence
	    if(arrGen[i] == 0)
	      {
		PORTB = 0x04; delay_ms(velocidad);
		/*
		while(initrCount <= velocidad)
		  {
		    nada = nada;
		  }
		ledValid = 0;
		*/
		PORTB = 0x00; _delay_ms(500);
	      }
	    if(arrGen[i] == 1)
	      {
		TCNT0 |= 0x00;
		PORTB = 0x08; delay_ms(velocidad);
		PORTB = 0x00; _delay_ms(500);
	      }
	    if(arrGen[i] == 2)
	      {
		TCNT0 |= 0x00;
		PORTB = 0x10; delay_ms(velocidad);
		PORTB = 0x00; _delay_ms(500);
	      }
	    if(arrGen[i] == 3)
	      {
		TCNT0 |= 0x00;
		PORTB = 0x20; delay_ms(velocidad);
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
		_delay_ms(500);
		yellow = 0;
	      }else if(red)
	      {
		arrInput[spot] = 1;
		spot = spot + 1;
		_delay_ms(500);
		red = 0;
	      }else if(green)
	      {
		arrInput[spot] = 2;
		spot = spot + 1;
		_delay_ms(500);
		green = 0;
	      }else if(blue)
	      {
		arrInput[spot] = 3;
		spot = spot + 1;
		_delay_ms(500);
		blue = 0;
	      }
	  }

	for(int j = 0; j < iteracion; j++)
	  {
	    // Check sequence
	    if(arrInput[j] != arrGen[j])
	      {
		valid = 0;
	      }else
	      {
		valid = 1;
	      }
	  }
			
	if(!valid)
	  {
	    // Flash LEDs 3 times
	    init = 0;
	    spot = 0;
	    PORTB = 0x00; _delay_ms(500);
	    PORTB = 0x3C; _delay_ms(5000);
	    PORTB = 0x00; _delay_ms(5000);
	    PORTB = 0x3C; _delay_ms(5000);
	    PORTB = 0x00; _delay_ms(5000);
	    PORTB = 0x3C; _delay_ms(5000);
	    PORTB = 0x00; _delay_ms(5000);
	  }else
	  {
	    iteracion = iteracion + 1;
	    velocidad = velocidad - 500;
	    ledTimer = velocidad;
	    spot = 0;
	    init = 1;
	  }
      }
  }
}



