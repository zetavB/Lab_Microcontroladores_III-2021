#include <avr/io.h>
#include <util/delay.h>

//definición de pines
const int LEDazul=3;
const int LEDrojo=2;

int vtermistor; //numero de 0 a 1023 entrada A0
float vtermistorNorm; //numero 0 a 5 entrada A0
float R1 = 100000; //resistencia de 100k en serie con el termistor.
float logR2, R2, TEMPERATURA;
float c1 = 2.115e-03, c2 = 0.383e-04, c3 = 5.228e-07; //coeficientes de steinhart-hart, obtenidos de: https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm


void hart(){
  //Ecuaciones de H-S
  //R2 = R1 * (1023.0 / (float)vtermistor - 1.0);
  //logR2 = log(R2);
  //TEMPERATURA = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); //ecuacion de S-H, da temperatura en kelvin
  //TEMPERATURA = TEMPERATURA -273.15; //convertimos a grados centigrados la temperatura
  //Serial.print("Temperatura: ");
  //Serial.print(TEMPERATURA);
}

void alerta_seguridad(){
  if ((vtermistorNorm < 2.72)){ //2.74V -> 30 grados celsius
    digitalWrite(LEDazul, HIGH);
    digitalWrite(LEDrojo, LOW);
  }
  else if ((vtermistorNorm > 2.72) & (vtermistorNorm < 3.27)){ //funcionamiento normal
    digitalWrite(LEDrojo, LOW);
    digitalWrite(LEDazul, LOW);
  }
  else if ((vtermistorNorm > 3.27)){ //3.26V -> 42 grados celsius
    digitalWrite(LEDazul, LOW);
    digitalWrite(LEDrojo, HIGH);
  }
}

void blink(){
  //led parpadea una vez cada vez que inicia el loop
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(300);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT); //led integrado como salida
  pinMode(LEDazul, OUTPUT);
  pinMode(LEDrojo, OUTPUT);
}

// loop infinito
void loop() {

  //blink();

  vtermistor = analogRead(A0); //leemos el voltaje que entra al pin 0, valor de 0 a 1023
  vtermistorNorm = vtermistor/204.6; //convertimos el valor de 8 bits (0-1023) a un valor acorde al voltaje (0V-5V)
  
  alerta_seguridad(); //se activa una alerta si la temperatura es superior a 42 grados o menor a 30 grados

}