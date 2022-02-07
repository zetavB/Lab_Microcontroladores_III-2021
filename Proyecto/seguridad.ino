#include <Adafruit_PCD8544.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>

//definición de pines
const int comm=12;

// definimos LCD
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 5, 6, 4, 8);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

//BATERIA
int valorBateria; 
float bateriaNormalizada;
int bateriaDisplay;

//CONTADORES PARA EL PARPADEO DEL LED de comm
int contador = 0;
int contador2 = 0;
int parpadeos = 0;

void blink(){ //parpadea el led integrado
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(300);
}

void display_refresh(){ // Se refrescan los datos en la pantalla con los resultados más recientes
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
  display.display();
  display.clearDisplay();
}

void serial_refresh(){ //Si la comunicación está activada, se envian los datos
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
  pinMode(comm, INPUT);
}

void loop() { // loop infinito

  serial_refresh();
  display_refresh();

}
