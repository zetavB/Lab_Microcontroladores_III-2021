#include <Adafruit_PCD8544.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>

//definicion de pines
const int LEDazul=2;
const int LEDrojo=53;
const int comm=12;
const int powerScreen = 22;

// definimos LCD
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 5, 6, 4, 8);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

//definicion de parametros necesarios para las ecuaciones de S-H para estimar la temperatura a partir del termistor
float R1 = 100000; //resistencia de 100k en serie con el termistor.
float logR2, R2, TEMPERATURA;
float c1 = 0.8586139205e-03, c2 = 2.059709585e-04, c3 = 0.8130635267e-07; //coeficientes de steinhart-hart, obtenidos de: https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm

//TEMP
int valorTermistor; 
int tempDisplay;

//HUMEDAD
int valorHumedad; 
float humedadNormalizada; 
int humedadDisplay;

//VIENTO
int valorViento; 
float vientoNormalizado; 
int vientoDisplay;

//BATERIA
int valorBateria; 
float bateriaNormalizada;
int bateriaDisplay;

//LLUVIA
int valorLluvia; 
float lluviaNormalizada;
int lluviaDisplay;

//LLUVIA
int valorLuz; 
float luzNormalizada;
int luzDisplay;

int contador = 0;
int contador2 = 0;
int parpadeos = 0;
int flag = 0;

void hart(){ //uiliza una ecuacion para estimar la temperatura de acuerdo a la resistencia del termistor
  R2 = R1 * (1023.0 / (float)valorTermistor - 1.0);
  logR2 = log(R2);
  TEMPERATURA = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); //ecuacion de S-H, da temperatura en kelvin
  TEMPERATURA = TEMPERATURA -273.15; //convertimos a grados centigrados la temperatura
}

void blink(){ //parpadea el led integrado
  //led parpadea una vez cada vez que inicia el loop
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(300);
}

void battery_low(){
  if (bateriaDisplay <= 15){
    if (flag < 3){
      flag++;
    }
    else{
      flag = 0;
      digitalWrite(LEDrojo, !digitalRead(LEDrojo));
    }
  }
  else{
    digitalWrite(LEDrojo, LOW);
  }
}

void display_refresh(){ // Se muestan los datos en la pantalla si el switch de powerScreen esta cerrado
  if(digitalRead(powerScreen) == LOW){
      display.setCursor(0,0);
      display.print("BATERIA: ");
      display.print(bateriaDisplay);
      display.println("%");
      display.print("TEMP:    ");
      display.print(tempDisplay);
      display.println("C");
      display.print("HUMEDAD: ");
      display.print(humedadDisplay);
      display.println("%");
      display.print("VIENTO:  ");
      display.print(vientoDisplay);
      display.println("%");
      display.print("LLUVIA:  ");
      display.print(lluviaDisplay);
      display.println("%");
      display.print("LUZ:     ");
      display.print(luzDisplay);
      display.println("%"); 
  }
  display.display();
  display.clearDisplay();
}

void serial_refresh(){ //se refresca el serial si el el switch de comm esta cerrado
  if(digitalRead(comm) == LOW){
    Serial.print("BATERIA:");
    Serial.print(bateriaDisplay);
    Serial.println("%");
    Serial.print("TEMPERATURA:");
    Serial.print(tempDisplay);
    Serial.println(" C");
    Serial.print("HUMEDAD:");
    Serial.print(humedadDisplay);
    Serial.println("%");
    Serial.print("VIENTO:");
    Serial.print(vientoDisplay);
    Serial.println("%");
    Serial.print("LLUVIA:");
    Serial.print(lluviaDisplay);
    Serial.println("%");
    Serial.print("LUZ:");
    Serial.print(luzDisplay);
    Serial.println("%");
  }
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
  pinMode(comm, INPUT);
  pinMode(powerScreen, INPUT);
}

void loop() { // loop infinito

  if (digitalRead(comm) == HIGH){
    digitalWrite(LEDazul, LOW);
    parpadeos = 0;
    contador = 0;
    contador2 = 0;
  }

  //El led del pin 2 parpadea 5 veces y se apaga por un periodo antes de volver a parpadear 5 veces, con esto evitamos usar delays y que se retrase en recorrer el loop 
  if (digitalRead(comm) == LOW){
    if (parpadeos < 10){
      if (contador < 10){
        contador = contador + 1;
      }
      else{
        digitalWrite(LEDazul, !digitalRead(LEDazul)); //toggle
        parpadeos = parpadeos + 1;
        contador = 0;
      }
    }
    else{
      if (contador2 < 90){
        contador2 = contador2 + 1;
      }
      else{
        contador2 = 0;
        parpadeos = 0;
      }
    }
  }
  

  valorTermistor = analogRead(A0); //leemos el voltaje que entra al pin A0, valor de 0 a 1023
  hart();
  tempDisplay = round(TEMPERATURA); //convertimos el valor de 10 bits (0-1023) a un valor acorde al voltaje (0V-5V)
  
  valorHumedad = analogRead(A15); //leemos el voltaje que entra al pin A15, valor de 0 a 1023
  humedadNormalizada = valorHumedad/10.23; //convertimos el valor de 10 bits (0-1023) a un valor normalizado (0%-100%)
  humedadDisplay = round(humedadNormalizada); //se redondea para no mostrar decimales en la pantalla

  valorViento = analogRead(A14); //leemos el voltaje que entra al pin A14, valor de 0 a 1023
  vientoNormalizado = valorViento/10.23; //convertimos el valor de 10 bits (0-1023) a un valor normalizado (0%-100%)
  vientoDisplay = round(vientoNormalizado); //se redondea para no mostrar decimales en la pantalla

  valorBateria = analogRead(A13); //leemos el voltaje que entra al pin A13, valor de 0 a 1023
  bateriaNormalizada = valorBateria/10.23; //convertimos el valor de 10 bits (0-1023) a un valor normalizado (0%-100%)
  bateriaDisplay = round(bateriaNormalizada); //se redondea para no mostrar decimales en la pantalla

  valorLluvia = 1;
  lluviaNormalizada = 1;
  lluviaDisplay = 1;

  valorLuz = 1;
  luzNormalizada = 1;
  luzDisplay = 1;

  battery_low();
  display_refresh();
}