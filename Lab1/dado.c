/*
Codigo para dado del Laboratorio 1
*/
#include <pic14/pic12f675.h>
typedef unsigned int word;
word __at 0x2007 __CONFIG = (_BODEN_OFF & _MCLRE_OFF & _WDT_OFF);
void delay (unsigned inttiempo);
void main(void)
	{
		/* 
		Se definen todos los puertos como salidas menos el 5.
		Siempre va a haber un LED encendido, no se puede rodar 0.
		Esto significa que podemos lograr este dado de 6 con 4 salidas y una entrada.
		*/
		TRISIO = 0b00100000;
		CMCON = 0b00000111;
		
		// Se digitalizan los pins de entrada para lograr interaccion con el boton
		ANSEL = 0;
		unsigned int time = 10;

		while(1)
		{
			if(GPIO5 != 0)
			{
				GP0 = 0x1;
				GP1 = 0x1;
				GP2 = 0x1;
				GP3 = 0x1;
				GP4 = 0x1;
			}else{
				GP0 = 0x0;
				GP1 = 0x0;
				GP2 = 0x0;
				GP3 = 0x0;
				GP4 = 0x0;
			}
		}
		
	}
void delay(unsigned int tiempo)
	{
		unsigned int i;
		unsigned int j;

		for(i=0;i<tiempo;i++)
		  for(j=0;j<1275;j++);
	}
