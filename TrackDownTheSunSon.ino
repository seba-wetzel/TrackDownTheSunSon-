#include "TrackDownTheSunSon.h"
#include <Servo.h>

//Instantacion de los objetos servo
Servo ServoPan;
Servo ServoTilt;

//Variables
values_u values;      //Estructura del tipo values_u donde se va a guardar
                      //los valores finales leidos de los ldr

values_u values_prom; //Estructura temporal tipo buffer para promediar

direction_u direcciones [2]; //Array de dos posiciones del tipo direction_u
                             //donde se guardan para que lado tiene que moverse
                             //cada servo
uint8_t posPan;   //Donde se guarda "el angulo" que se tiene que mover el servo
uint8_t posTilt;


void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif

  digitalWrite(A0, INPUT_PULLUP);
  digitalWrite(A1, INPUT_PULLUP);
  digitalWrite(A2, INPUT_PULLUP);
  digitalWrite(A3, INPUT_PULLUP);
  ServoPan.attach(ServoPanPin);
  ServoTilt.attach(ServoTiltPin);
  goHome(&posPan, &posTilt); //Se pasan punteros a las variable del angulo
}                            //donde tiene el home cada servo

void loop() {
  // Esta version tiene un loop para promediar n lecturas de los LDR's
  for (uint8_t i; i <= (factor_de_correccion - 1); i++) {
    readSensors(&values); //Leemos los sensores y guardamos los valores en
                          //values que sirve de buffer
    //Vamos sumando el valor leido en el bucle y lo guardamos en values_prom
    //para despues dividirlo y tener el promedio
    values_prom.LDR1 += values.LDR1;
    values_prom.LDR2 += values.LDR2;
    values_prom.LDR3 += values.LDR3;
    values_prom.LDR4 += values.LDR4;
    delay(10); //Si incrementamos este delay las lecturas se hacen mas pausadas
  }            //pudiendo dar mas estabilidad a las lecturas

  //Dividimos y guardamos en en values el valor ya promediado
  values.LDR1 = values_prom.LDR1 / factor_de_correccion;
  values.LDR2 = values_prom.LDR2 / factor_de_correccion;
  values.LDR3 = values_prom.LDR3 / factor_de_correccion;
  values.LDR4 = values_prom.LDR4 / factor_de_correccion;

  values_prom.LDR1 = 0;
  values_prom.LDR2 = 0;
  values_prom.LDR3 = 0;
  values_prom.LDR4 = 0;


#ifdef DEBUG
  serialplot(&values);                         //Si esta definido el debug, se imprime de manera que se plotee en el IDE Arduino
#endif

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

//Esta funcion recibe un estructura donde guardar los valor mapeados
//del LDR con el valor que puede generar el PWM para manejar los servos
void readSensors(values_u * recivedValues) {
  recivedValues->LDR1 = map(analogRead(A0), 0, 1023, 0, 255);
  recivedValues->LDR2 = map(analogRead(A1), 0, 1023, 0, 255);
  recivedValues->LDR3 = map(analogRead(A2), 0, 1023, 0, 255);
  recivedValues->LDR4 = map(analogRead(A3), 0, 1023, 0, 255);
}


//Esta funcion recibe dos estructuras, una donde estan guardadas las lecturas
//de los LDR, y otra donde se guardan las direccin a la que debe moverse cada
//servo, segundo compara que LDR esta mas iluminado. Esta funcion solo dice
//para que lado moverse, pero no cuanto se tiene que mover.

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


//Esta funcion es la que se encarga de decir cuanto se tiene que mover cada
//servo, al incrementar o decrementar las variables que recibe por puntero
//posPanP y posTiltP. Pero no controla el servo, de eso se encarga el
//metodo write de cada objeto servo cuando se le pasan estas variable
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

void serialplot ( values_u * valuesP) {
  Serial.print(valuesP->LDR1+100);
  Serial.print(" ");

  Serial.print(valuesP->LDR2+50);
  Serial.print(" ");

  Serial.print(valuesP->LDR3-50);
  Serial.print(" ");

  Serial.print(valuesP->LDR4-100);
  Serial.println(" ");

}
