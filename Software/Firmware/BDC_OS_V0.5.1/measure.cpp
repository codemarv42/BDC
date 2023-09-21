#include <Arduino.h>

#include "pins.h"
#include "display.h"
#include "functions.h"
#include "measure.h"
#include "pitches.h"
#include "settings.h"
#include "ESP32_BLE.h"

///////Variables/////////////

//Button
unsigned int ButtonPressType = 0; //1 - short press, 2 - long press
int lastState = HIGH;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

//Battery

unsigned int CellType = 2; //2 - 2S, 3 - 3S ...
float BATVoltage;
float AvgCellVoltage;
unsigned int TmpVbatAvg;
unsigned int DischargePercent;

///////////////////////////

void IRAM_ATTR ReadButton() {
  currentState = digitalRead(BUTTON);

  if (lastState == HIGH && currentState == LOW) {    // button is pressed
    pressedTime = millis();
  } else if (lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if ( pressDuration > LONG_PRESS_TIME ) {
      ButtonPressType = 2;
    } else {
      ButtonPressType = 1;
    }
  }

  // save the the last state
  lastState = currentState;
}

void CalculateBATVoltage() {

  unsigned int TmpVbatArray[30];

  if ((EnDischarge == true) && (DischargeState == 1)) {
    digitalWrite(DISCHARGE_ON, LOW);
  }

  for (int i = 0; i <= 29; i++) {
    TmpVbatArray[i] = analogRead(BVCC);
  }

  if ((EnDischarge == true) && (DischargeState == 1)) {
    digitalWrite(DISCHARGE_ON, HIGH);
  }

  TmpVbatAvg = (TmpVbatArray[0] + TmpVbatArray[1] + TmpVbatArray[2] + TmpVbatArray[3] + TmpVbatArray[4] + TmpVbatArray[5] + TmpVbatArray[6] + TmpVbatArray[7] + TmpVbatArray[8] + TmpVbatArray[9] + TmpVbatArray[10] + TmpVbatArray[11] + TmpVbatArray[12] + TmpVbatArray[13] + TmpVbatArray[14] + TmpVbatArray[15] + TmpVbatArray[16] + TmpVbatArray[17] + TmpVbatArray[18] + TmpVbatArray[19] + TmpVbatArray[20] + TmpVbatArray[21] + TmpVbatArray[22] + TmpVbatArray[23] + TmpVbatArray[24] + TmpVbatArray[25] + TmpVbatArray[26] + TmpVbatArray[27] + TmpVbatArray[28] + TmpVbatArray[29]) / 30;

  double punkte[][2] = {{4095, 24.34}, {4080, 24.14}, {4022, 23.94}, {3969, 23.74},
    {3914, 23.54}, {3861, 23.34}, {3803, 23.14}, {3755, 22.94},
    {3705, 22.74}, {3658, 22.54}, {3610, 22.34}, {3561, 22.14},
    {3516, 21.94}, {3473, 21.73}, {3433, 21.53}, {3393, 21.33},
    {3353, 21.13}, {3316, 20.93}, {3279, 20.73}, {3244, 20.53},
    {3205, 20.33}, {3170, 20.13}, {3136, 19.93}, {3100, 19.73},
    {3044, 19.43}, {2992, 19.13}, {2941, 18.83}, {2891, 18.53},
    {2840, 18.23}, {2793, 17.93}, {2741, 17.63}, {2692, 17.33},
    {2642, 17.03}, {2593, 16.73}, {2540, 16.43}, {2491, 16.12},
    {2443, 15.82}, {2393, 15.52}, {2347, 15.22}, {2296, 14.92},
    {2251, 14.62}, {2202, 14.32}, {2155, 14.02}, {2105, 13.72},
    {2056, 13.42}, {2001, 13.12}, {1953, 12.82}, {1889, 12.42},
    {1823, 12.02}, {1758, 11.62}, {1692, 11.22}, {1626, 10.81},
    {1562, 10.41}, {1489, 10.01}, {1424, 9.61}, {1359, 9.21},
    {1294, 8.81}, {1228, 8.41}, {1161, 8.01}, {1095, 7.61},
    {1030, 7.21}, {959, 6.81}, {894, 6.41}, {829, 6.01},
    {763, 5.61}, {715, 5.31}, {650, 4.91}, {586, 4.51},
    {556, 4.21}, {499, 4.01}
  };

  int anzahlPunkte = sizeof(punkte) / sizeof(punkte[0]);

  // Suche nach dem nächsten Punkt
  int index = -1;
  for (int i = anzahlPunkte - 1; i >= 0; i--) {
    if (TmpVbatAvg >= punkte[i][0]) {
      index = i;
    }
  }

  // Berechnung des y-Werts durch lineare Interpolation
  double x1 = punkte[index][0];
  double x2 = punkte[index - 1][0];
  double y1 = punkte[index][1];
  double y2 = punkte[index - 1][1];

  double m = (y2 - y1) / (x2 - x1);
  double b = y1 - m * x1;

  BATVoltage = m * (TmpVbatAvg - x1) + y1;

  if (CellType == 2) {
    AvgCellVoltage = BATVoltage / 2.00;
  } else if (CellType == 3) {
    AvgCellVoltage = BATVoltage / 3.00;
  } else if (CellType == 4) {
    AvgCellVoltage = BATVoltage / 4.00;
  } else if (CellType == 5) {
    AvgCellVoltage = BATVoltage / 5.00;
  } else if (CellType == 6) {
    AvgCellVoltage = BATVoltage / 6.00;
  }

  DischargePercent = (AvgCellVoltage - (DischargeVoltage - 0.02)) * (100 - 0) / (4.20 - (DischargeVoltage - 0.02)) + 0;

  if (DischargePercent > 100) {
    DischargePercent = 100;
  }
}
