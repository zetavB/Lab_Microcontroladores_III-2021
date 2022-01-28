#include <avr/io.h>
#include <util/delay.h>

//definición de pines
const int LEDazul=3;
const int LEDrojo=2;

int valorTermistor; //numero de 0 a 1023 entrada A0
float voltajeTermistor; //numero 0 a 5 entrada A0
int valorHumedad;
float humedadNormalizada;

void hart(){
  /*float R1 = 100000; //resistencia de 100k en serie con el termistor.
  float logR2, R2, TEMPERATURA;
  float c1 = 2.115e-03, c2 = 0.383e-04, c3 = 5.228e-07; //coeficientes de steinhart-hart, obtenidos de: https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm

  //Ecuaciones de H-S
  R2 = R1 * (1023.0 / (float)valorTermistor - 1.0);
  logR2 = log(R2);
  TEMPERATURA = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); //ecuacion de S-H, da temperatura en kelvin
  TEMPERATURA = TEMPERATURA -273.15; //convertimos a grados centigrados la temperatura
  Serial.print("Temperatura: ");
  Serial.print(TEMPERATURA);*/
}

void alerta_seguridad(){
  if ((voltajeTermistor < 2.72)){ //2.74V -> 30 grados celsius
    digitalWrite(LEDazul, HIGH);
    digitalWrite(LEDrojo, LOW);
  }
  else if ((voltajeTermistor > 2.72) & (voltajeTermistor < 3.27)){ //funcionamiento normal
    digitalWrite(LEDrojo, LOW);
    digitalWrite(LEDazul, LOW);
  }
  else if ((voltajeTermistor > 3.27)){ //3.26V -> 42 grados celsius
    digitalWrite(LEDazul, LOW);
    digitalWrite(LEDrojo, HIGH);
  }
}

void indicador_humedad(){ //enciende el led integrado si se supera el 50% de humedad relativa
  if (humedadNormalizada > 50){
    digitalWrite(LED_BUILTIN, HIGH);    
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
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
  //configuracion de pines como salida
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LEDazul, OUTPUT);
  pinMode(LEDrojo, OUTPUT);
}


void loop() { // loop infinito

  valorTermistor = analogRead(A0); //leemos el voltaje que entra al pin A0, valor de 0 a 1023
  voltajeTermistor = valorTermistor/204.6; //convertimos el valor de 8 bits (0-1023) a un valor acorde al voltaje (0V-5V)
  
  valorHumedad = analogRead(A5); //leemos el voltaje que entra al pin A5, valor de 0 a 1023
  humedadNormalizada = valorHumedad/10.23; //convertimos el valor de 8 bits (0-1023) a un valor normalizado (0%-100%)

  alerta_seguridad(); //se activa una alerta si la temperatura es superior a 42 grados o menor a 30 grados
  indicador_humedad(); //enciende una luz si la humedad es más del 50%
}