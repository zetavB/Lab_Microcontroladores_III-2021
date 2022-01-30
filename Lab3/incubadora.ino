#include <Adafruit_PCD8544.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>

//definición de pines
const int LEDazul=3;
const int LEDrojo=2;
const int calentador=9;
char pantalla[10];

// definimos LCD
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 5, 6, 4, 8);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

//definicion de parámetros necesarios para las ecuaciones de S-H para estimar la temperatura a partir del termistor
float R1 = 100000; //resistencia de 100k en serie con el termistor.
float logR2, R2, TEMPERATURA;

//coeficientes de steinhart-hart, obtenidos de: https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm
float c1 = -0.008017042919e-03, c2 = 2.931366473e-04, c3 = -0.08405517035e-07; 

float dutyCycle;
int valorCalentador=0;

int valorTermistor; //numero de 0 a 1023 entrada A0
float voltajeTermistor; //numero 0 a 5 entrada A04

float valorDeseado;
float valorDeseadoDisplay;

int valorHumedad; //numero de 0 a 1023 entrada A5
float humedadNormalizada; //numero 0 a 100 entrada A5

void hart(){ //uiliza una ecuación para estimar la temperatura de acuerdo a la resistencia del termistor
  R2 = R1 * (1023.0 / (float)valorTermistor - 1.0);
  logR2 = log(R2);
  TEMPERATURA = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); //ecuacion de S-H, da temperatura en kelvin
  TEMPERATURA = TEMPERATURA -273.15; //convertimos a grados centigrados la temperatura
}

void alerta_seguridad(){ //se activa un led rojo si la temperatura es superior a 42 grados o un led azul si es menor a 30 grados
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

void blink(){ //parpadea el led integrado
  //led parpadea una vez cada vez que inicia el loop
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(300);
}

float ajusteCalentador(float target){
    analogWrite(calentador, roundf(target*0.15*21.25));
    return target;
}

void display_refresh(){
  display.setCursor(0,0);
  display.print("T.Set:");
  display.println(valorDeseadoDisplay);
  display.print("Temp:");
  display.println(TEMPERATURA);
  display.print("Duty:");
  display.print(dutyCycle);
  display.println("%");
  display.print("Hum:");
  display.print(humedadNormalizada);
  display.println("%");
  display.display();
  display.clearDisplay();
}

void setup() {
  Serial.begin(9600);
  Serial.println("PCD test");
  display.begin();
  display.setContrast(75);
  display.display(); // show splashscreen
  delay(500);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(BLACK);
  //configuracion de pines como salida
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LEDazul, OUTPUT);
  pinMode(LEDrojo, OUTPUT);
}

void loop() { // loop infinito
  valorDeseado = analogRead(A4);
  valorDeseadoDisplay = (valorDeseado)/5.11;

  valorTermistor = analogRead(A0); //leemos el voltaje que entra al pin A0, valor de 0 a 1023
  voltajeTermistor = valorTermistor/204.6; //convertimos el valor de 10 bits (0-1023) a un valor acorde al voltaje (0V-5V)
  
  valorHumedad = analogRead(A5); //leemos el voltaje que entra al pin A5, valor de 0 a 1023
  humedadNormalizada = valorHumedad/10.23; //convertimos el valor de 10 bits (0-1023) a un valor normalizado (0%-100%)

  alerta_seguridad();

  dutyCycle = ajusteCalentador(20);

  hart();

  display_refresh();
}
