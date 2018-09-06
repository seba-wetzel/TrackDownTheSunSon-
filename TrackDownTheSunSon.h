#ifndef __TRACKDWONTHESUNSON_H
#define __TRACKDWONTHESUNSON_H

//Definicion para activar el debug
#define DEBUG

//Definitions:
#define ServoPanPin 10
#define ServoTiltPin 9

#ifdef invert_dir

#define HomeP 180
#define HomeT 180
#else
#define HomeP 0
#define HomeT 0
#endif

#define Hysteresis 20
#define factor_de_correccion 32
#define MaxAngular 180
#define MinAngular 0
typedef enum {
  Up =1,
  Down,
  Left,
  Right,
  Still,
} direction_u;

typedef struct {
  uint8_t LDR1 = 0;
  uint8_t LDR2 = 0;
  uint8_t LDR3 = 0;
  uint8_t LDR4 = 0;
} values_u;

/*
    ------------------------------------
    |                LDR3              |
    |                                  |
    |LDR1         PANEL SOLAR     LDR2 |
    |                                  |
    |                LDR4              |
    ------------------------------------

*/

// Functions prototypes:
void goHome(uint8_t *, uint8_t *);
void readSensors(values_u *);
void trackingMove( values_u *, direction_u * );
void settingMove( direction_u *, uint8_t *, uint8_t *); 
void serialplot ( values_u *);


#endif
