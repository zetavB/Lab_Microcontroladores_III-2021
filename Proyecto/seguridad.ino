#include <Servo.h>
#include <Adafruit_PCD8544.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <Keypad.h>
#include <TimerOne.h>


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
int correctPassword = 2;
int teclaDisplay = 0;
int ALARM_ON = 0;
int movement = 0;
int movement2 = 0;

//definicion de pines
const int comm=12;
const int door = 53;
const int doorLED = 52;
const int window = 51;
const int windowLED = 50;
const int movementLED = 48;
const int alarmSound = 46;
const int cam1 = 25;
const int cam2 = 23;

//sensor ultrasonico
const int trig2 = 9;
const int trig = 3;
const int echo2 = 4;
const int echo = 2;
int dist = 0;
int dist2 = 0;

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

void movement_detected(){
  if (dist > 1){
    digitalWrite(cam1, HIGH);
    movement = 1;
  }
  else{
    movement = 0;
    digitalWrite(cam1, LOW);
  }
  if (dist2 > 1){
    digitalWrite(cam2, HIGH);
    movement2 = 1;
  }
  else{
    movement2 = 0;
    digitalWrite(cam2, LOW);
  }
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

void soundAlert(){
  if ((doorState == 1) | windowState == 1 | movement == 1 | movement2 == 1){
    digitalWrite(alarmSound, HIGH);
  }
  else{
    digitalWrite(alarmSound, LOW);
  }
}

void trigger(){
  digitalWrite(trig, HIGH);
  delayMicroseconds(10); //Enviamos un pulso de 10us
  digitalWrite(trig, LOW);

  digitalWrite(trig2, HIGH);
  delayMicroseconds(10); //Enviamos un pulso de 10us
  digitalWrite(trig2, LOW);
}

float calcular_distancia(){
  float t = pulseIn(echo, HIGH); //obtenemos el ancho del pulso
  float d = t/59;             //escalamos el tiempo a una distancia en cm
  
  int dround = round(d);
  return dround;
}

float calcular_distancia2(){
  float t2 = pulseIn(echo2, HIGH); //obtenemos el ancho del pulso
  float d2 = t2/59;             //escalamos el tiempo a una distancia en cm
  
  int dround2 = round(d2);
  return dround2;
}

void display_refresh(){ // Se refrescan los datos en la pantalla con los resultados mas recientes
  display.setCursor(0,0);
  display.print("Pass: ");
  display.print(CLAVE);
  display.println("");
  
  display.print("Check: ");

  if (correctPassword == 1){
    display.println("Correct");
    display.print("Alarm: ");

    if (ALARM_ON == 1){
      display.print("ON");
    }
    else{
      display.print("OFF");
    }
    
  }
  else if(correctPassword == 0){
    display.println("wrong");
    display.print("Alarm: ");

    if (ALARM_ON == 1){
      display.print("ON");
    }
    else{
      display.print("OFF");
    }

  }
  else{
    display.println("Waiting");
    display.println("Alarm: OFF");
    
  }
  display.println("");

  display.display();
  display.clearDisplay();
}

void alarms_off(){
  digitalWrite(doorLED, LOW);
  digitalWrite(windowLED, LOW);
  digitalWrite(alarmSound, LOW);
  digitalWrite(cam1, LOW);
  digitalWrite(cam2, LOW);
}

void serial_refresh(){ //Si la comunicacion esta activada, se envian los datos
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

void isr(){
  TECLA = teclado.getKey();

  if (TECLA){ // comprueba que se haya presionado una tecla
    CLAVE[INDICE] = TECLA; // almacena en array la tecla presionada
    INDICE++; // incrementa indice en uno
  }
  if(INDICE == 4){ // si ya se almacenaron los 4 digitos
    if(!strcmp(CLAVE, CLAVE_MAESTRA)){ // compara clave ingresada con clave maestra
      correctPassword = 1;  // imprime en pantalla que es correcta la clave
      if (ALARM_ON == 1){ //si la pass es correcta, se cambia de estado a la alarma
        ALARM_ON = 0;
        unlock(1);
        unlock(2);
      }
      else{
         ALARM_ON = 1;
         lock(1);
         lock(2);
      }
      for (int i = 0; i < 4; ++i){ //llenamos con asteriscos la clave de nuevo
        CLAVE[i] = '-'; 
      }
    }
    else{
      for (int i = 0; i < 4; ++i){
        CLAVE[i] = '-';
      }
      correctPassword = 0;  // imprime en pantalla que es incorrecta la clave
    }
    INDICE = 0;
  }
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; ++i){ //llenamos CLAVE con guiones
    CLAVE[i] = '-';
  }

  //inicializacion de pantalla
  Serial.println("PCD test");
  display.begin();
  display.setContrast(75);
  display.display(); // show splashscreen
  delay(500);
  display.clearDisplay(); // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(BLACK);

  //configuracion de pines
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(doorLED, OUTPUT);
  pinMode(windowLED, OUTPUT);
  pinMode(alarmSound, OUTPUT);
  pinMode(cam1, OUTPUT);
  pinMode(cam2, OUTPUT);
  pinMode(trig,OUTPUT);
  pinMode(trig2,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(echo2,INPUT);
  pinMode(comm, INPUT);

  //Configuracion de servos
  lockFront.attach(8, 1000, 2000);
  lockBack.attach(9, 1000, 2000);

  digitalWrite(trig, LOW);// Inicializamos el pin con 0
  digitalWrite(trig2, LOW);// Inicializamos el pin con 0

  //timer
  Timer1.initialize(100000);//Cada cuantos us ocurre le interrupt
  Timer1.attachInterrupt(isr, 100000);
}


void loop() { // loop infinito

  trigger();
  dist = calcular_distancia();
  dist2 = calcular_distancia2();
  
  //movement_detected2();
  
  //si la alarma esta armada, revisa si se abre window/door y hace que se active la alarma
  if (ALARM_ON == 1){
    door_open(); //revisa si se abrio una puerta
    window_open(); //revisa si se abrio una ventana
    movement_detected(); //revisa si se detecto algun movimiento en el sensor ultrasonico
    soundAlert(); //enciende la alarma de sonido
  }
  else{ //si la alarma esta desarmada, apaga todas las alarmas
    alarms_off();
  }
  
  serial_refresh();
  display_refresh();

}
