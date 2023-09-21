      /////////////////////////////////////////////
     //          BDC OS by Marvin Ramm          //
    //               REV: V0.5.3               //
   //     https://github.com/codemarv42/      //
  //        Last modified: 21.09.2023        //
 //      © Copyright 2023 CC-BY-NC-SA       //
/////////////////////////////////////////////

/*
   ToDo:

   -Last mit pwm steuern
   -Wenn beim einstecken des Akkus der Knopf gedrückt wird, wird der OTA teil aktiviert
   -Mit PID auf perfekter Spannung entladen

*/

///////////////////Includes//////////////////

#include "pins.h"
#include "display.h"
#include "functions.h"
#include "measure.h"
#include "pitches.h"
#include "settings.h"
#include "ESP32_BLE.h"

//////////////////Variables//////////////////

long PreviousMillis;

/////////////////////////////////////////////

////////////////////Setup////////////////////

void setup(void) {
  HardwareInit();

}

////////////////////LOOP/////////////////////

void loop() {

  ButtonAction();
  Discharge();
  SetLED();

  if ((millis() - PreviousMillis >= 500) || (ForceDisplayUpdate == true)) { //Every 500ms
    PreviousMillis = millis();

    CalculateBATVoltage();
    UpdateDisplay();
    BLELoop();

    //PrintDataSerial();

    ForceDisplayUpdate = false;
  }

  if ((millis() - PreviousMillis2 >= SLEEPMODE_TIME) && (DischargeState == 2)) { //Activate sleep mode
    ActivateSleep();
    PreviousMillis2 = millis();
  }

}
