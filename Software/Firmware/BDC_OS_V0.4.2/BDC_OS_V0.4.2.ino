      /////////////////////////////////////////////
     //          BDC OS by Marvin Ramm          //
    //               REV: V0.4.2               //
   //     https://github.com/codemarv42/      //
  //        Last modified: 20.09.2023        //
 //      © Copyright 2023 CC-BY-NC-SA       //
/////////////////////////////////////////////

/*
 * ToDo:
 * 
 * -APP Einbindung
 * -Last mit pwm steuern
 * -Deepsleep Mode wenn Akku auf storage zum wecken Button drücken
 * -Buzzer TON
 * 
 */

///////////////////Includes//////////////////

#include "pins.h"
#include "display.h"
#include "functions.h"
#include "measure.h"
#include "pitches.h"
#include "settings.h"

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
    
    //PrintDataSerial();

    ForceDisplayUpdate = false;
  }
  
}
