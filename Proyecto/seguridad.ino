#include <Servo.h>
#include <Adafruit_PCD8544.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <Keypad.h>


//TECLADO
const byte ROWS = 4; //4 filas
const byte COLS = 3; //3 columnas

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {49, 47, 45, 43}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {31, 33, 41}; //connect to the column pinouts of the keypad

Keypad teclado = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char TECLA;
char CLAVE[5];
char CLAVE_MAESTRA[5] = "1234";
byte INDICE = 0;
int compDisplay = 2;
int teclaDisplay = 0;

//definición de pines
const int comm=12;
const int door = 53;
const int doorLED = 52;
const int window = 51;
const int windowLED = 50;
const int movementLED = 48;

//sensor ultrasónico
int trig = 3;
int echo = 2;
int dist = 0;

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

//Flags para que parpadeen los leds de ventana/puerta abierta
int flagWindow = 0;
int flagDoor = 0;

//Servos de puertas
Servo lockFront;
Servo lockBack;

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

void movement_detected(){
  if (dist < 100){
    digitalWrite(movementLED, HIGH);
  }
  else{
    digitalWrite(movementLED, LOW);
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

void trigger() {
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(trig, LOW);
}

float calcular_distancia(){
  float t = pulseIn(echo, HIGH); //obtenemos el ancho del pulso
  float d = t/59;             //escalamos el tiempo a una distancia en cm
  
  int dround = round(d);
  return dround;
}

void display_refresh(){ // Se refrescan los datos en la pantalla con los resultados más recientes
  display.setCursor(0,0);
  display.print("Pass: ");
  display.print(CLAVE);
  display.println("");
  display.print("Check: ");

  if (compDisplay == 1){
    display.println("Correcta");
  }
  else if(compDisplay == 0){
    display.println("Incorrecta");
  }
  else{
    display.println("Waiting");
  }
  display.println("");
  display.print("Distance: ");
  display.print(dist);
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

void unlock(int door){
  if(door == 1){
    lockFront.write(0);
  }else if(door ==2){
    lockBack.write(0);
  }
}

void lock(int door){
  if(door == 1){
    lockFront.write(180);
  }else if(door ==2){
    lockBack.write(180);
  }
}

void setup() {
  Serial.begin(9600);


  //inicialización de pantalla
  Serial.println("PCD test");
  display.begin();
  display.setContrast(75);
  display.display(); // show splashscreen
  delay(500);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(BLACK);

  //configuracion de pines
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(doorLED, OUTPUT);
  pinMode(windowLED, OUTPUT);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(comm, INPUT);

  //Configuracion de servos
  lockFront.attach(8, 1000, 2000);
  lockBack.attach(9, 1000, 2000);

  digitalWrite(trig, LOW);//Inicializamos el pin con 0
}

void loop() { // loop infinito

  TECLA = teclado.getKey();   // obtiene tecla presionada y asigna a variable

  if (TECLA){        // comprueba que se haya presionado una tecla
    CLAVE[INDICE] = TECLA;    // almacena en array la tecla presionada
    INDICE++;       // incrementa indice en uno
  }

  if(INDICE == 4){       //si ya se almacenaron los 4 digitos
    if(!strcmp(CLAVE, CLAVE_MAESTRA)){   // compara clave ingresada con clave maestra
     compDisplay = 1;  // imprime en monitor serial que es correcta la clave
    }
    else{
      compDisplay = 0;  // imprime en monitor serial que es incorrecta la clave
    }
    INDICE = 0;
  }

  trigger();
  dist = calcular_distancia();
  movement_detected();
  door_open();
  window_open();

  //funcion de keypad para abrir y cerrar locksw
  lock(1);
  lock(2);
  //delay(1000);
  unlock(1);
  unlock(2);
  //delay(1000);
  serial_refresh();
  display_refresh();

}
