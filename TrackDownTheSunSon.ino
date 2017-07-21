#include "TrackDownTheSunSon.h"
#include <Servo.h>

//Instantacion de los objetos servo
Servo ServoPan;
Servo ServoTilt;

//Variables
values_u values;
direction_u direcciones [2];
uint8_t posPan;
uint8_t posTilt;


void setup() {
  
  digitalWrite(A0, INPUT_PULLUP);
  digitalWrite(A1, INPUT_PULLUP);
  digitalWrite(A2, INPUT_PULLUP);
  digitalWrite(A3, INPUT_PULLUP);
  ServoPan.attach(ServoPanPin);
  ServoTilt.attach(ServoTiltPin);
  goHome(&posPan, &posTilt);
}

void loop() {

  readSensors(&values);                         //Leemos los sensores y guardamos los valores en values
  trackingMove(&values, direcciones);           //Con los valores de value decidimos que movimiento hay que hacer, y lo guardamos en direcciones
  settingMove(direcciones, &posPan, &posTilt ); //De acuerdo con la direccion que halla que tomar, se aumenta o reduce en los angulos de tilt&pan
  
  ServoPan.write(posPan);                       //Escribimios los angulos de tilt&pan en los servos
  ServoTilt.write(posTilt);
  delay(10);
}

//Funciones
void goHome(uint8_t *posPanP, uint8_t *posTiltP) {
  *posPanP = HomeP;
  *posTiltP = HomeT;

}

void readSensors(values_u * recivedValues) {

  recivedValues->LDR1 = map(analogRead(A0), 0, 1023, 0, 255);
  recivedValues->LDR2 = map(analogRead(A1), 0, 1023, 0, 255);
  recivedValues->LDR3 = map(analogRead(A2), 0, 1023, 0, 255);
  recivedValues->LDR4 = map(analogRead(A3), 0, 1023, 0, 255);
}

void trackingMove (values_u *recivedValues, direction_u *storage) {

  //Pan move
  if (((recivedValues->LDR2 - recivedValues->LDR1) ) > Hysteresis) {
    storage[0] = Right;
  }
  else if (((recivedValues->LDR2 - recivedValues->LDR1) ) < -Hysteresis ) {
    storage[0] = Left;
  }
  else {
    storage[0] = Still;
  }

  //Tilt move
  if (( recivedValues->LDR3 - recivedValues->LDR4 )  > Hysteresis) {
    storage[1] = Up;
  }
  else if (( recivedValues->LDR3 - recivedValues->LDR4 ) < -Hysteresis)  {
    storage[1] = Down;
  }
  else {
    storage[1] = Still;
  }
}

void settingMove( direction_u *recivedDirections, uint8_t *posPanP, uint8_t *posTiltP) {

  //Pan move
  switch (recivedDirections[0]) {
    case Right: if (*posPanP < MaxAngular) {
        *posPanP = *posPanP + 1;
      }
      break;
    case Left: if (*posPanP > MinAngular ) {
        *posPanP = *posPanP - 1;
      }
      break;
    case Still: ;
      break;
  }
  //Tilt move
  switch (recivedDirections[1]) {
    case Up: if (*posTiltP < MaxAngular) {
        *posTiltP = *posTiltP + 1;
      }
      break;
    case Down: if (*posTiltP > MinAngular ) {
        *posTiltP = *posTiltP - 1;
      }
      break;
    case Still: ;
      break;
  }
}
