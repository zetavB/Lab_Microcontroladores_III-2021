#include <avr/io.h>
#include <util/delay.h>

int vtermistor;
float vtermistorNorm;
float R1 = 100000; //resistencia de 100k en serie con el termistor.
float logR2, R2, TEMPERATURA;
float c1 = 2.115e-03, c2 = 0.383e-04, c3 = 5.228e-07; //coeficientes de steinhart-hart, obtenidos de: https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm


void blink(){
  //led parpadea una vez cada vez que inicia el loop
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT); //led integrado como salida
}

// loop infinito
void loop() {

  blink();

  vtermistor = analogRead(A0);
  vtermistorNorm = vtermistor/204.6; //convertimos el valor de 8 bits (0-1023) a un valor entre 0 y 5
  R2 = R1 * (1023.0 / (float)vtermistor - 1.0);
  logR2 = log(R2);
  TEMPERATURA = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); //ecuacion de S-H, da temperatura en kelvin
  TEMPERATURA = TEMPERATURA -273.15; //convertimos a grados centigrados la temperatura

  Serial.print("Temperatura: ");
  Serial.print(TEMPERATURA);

  if ((vtermistorNorm > 3)){
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
  }
}