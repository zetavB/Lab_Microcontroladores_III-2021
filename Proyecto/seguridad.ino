#include <Adafruit_PCD8544.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>

//definición de pines
const int comm=12;
const int door = 53;
const int doorLED = 52;
const int window = 51;
const int windowLED = 50;

// definimos LCD
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 5, 6, 4, 8);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

//PUERTA
int doorState = 0;
int windowState = 0;

//BATERIA
int valorBateria; 
float bateriaNormalizada;
int bateriaDisplay;

//CONTADORES PARA EL PARPADEO DEL LED de comm
int contador = 0;
int contador2 = 0;
int parpadeos = 0;
int flagWindow = 0;
int flagDoor = 0;

void blink(){ //parpadea el led integrado
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(300);
}

void window_open(){
  if (digitalRead(window) == HIGH){
    windowState = 1;
    if (flagWindow < 5){
      flagWindow++;
    }
    else{
      flagWindow = 0;
      digitalWrite(windowLED, !digitalRead(windowLED));
    }
  }
  else{
    windowState = 0;
    digitalWrite(windowLED, LOW);
  }
}

void door_open(){
  if (digitalRead(door) == HIGH){
    doorState = 1;
    if (flagDoor < 5){
      flagDoor++;
    }
    else{
      flagDoor = 0;
      digitalWrite(doorLED, !digitalRead(doorLED));
    }
  }
  else{
    doorState = 0;
    digitalWrite(doorLED, LOW);
  }
}


void display_refresh(){ // Se refrescan los datos en la pantalla con los resultados más recientes
  display.setCursor(0,0);
  display.print("Door open:   ");
  display.print(doorState);
  display.println("");
  display.print("Window open: ");
  display.print(windowState);
  display.println("");
  display.display();
  display.clearDisplay();
}

void serial_refresh(){ //Si la comunicación está activada, se envian los datos
  if(digitalRead(comm) == LOW){
    Serial.print("BATERIA:");
    Serial.print(bateriaDisplay);
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
  pinMode(doorLED, OUTPUT);
  pinMode(windowLED, OUTPUT);
  pinMode(comm, INPUT);
}

void loop() { // loop infinito
  door_open();
  window_open();
  serial_refresh();
  display_refresh();

}
