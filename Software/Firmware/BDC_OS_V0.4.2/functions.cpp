#include <Arduino.h>

#include "pins.h"
#include "display.h"
#include "functions.h"
#include "measure.h"
#include "pitches.h"
#include "settings.h"

////////////Variables//////////////

int DischargeState = 0;  //0 - Aus, 1 - Wird endladen, 2 - Endladen
bool EnDischarge = false;

float DischargeVoltage = STD_DISCHARGE_VOLTAGE;

/////////////////////////////////

void HardwareInit() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(DISCHARGE_ON, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BVCC, INPUT);

  digitalWrite(DISCHARGE_ON, LOW);

  attachInterrupt(BUTTON, ReadButton, CHANGE);

  RGB(OFF, OFF, ON);  //R G B

  AutoSetCellType();

  DisplayInit();
  
  DrawBootPage();
}

void Discharge() {
  if ((EnDischarge == true) && (DischargeState == 1)){
    digitalWrite(DISCHARGE_ON, HIGH);
    if (AvgCellVoltage <= DischargeVoltage - 0.02){
      digitalWrite(DISCHARGE_ON, LOW);
      DischargeState = 2;
      EnDischarge = false;
    }
  }
}

void SetLED(){
  if (DischargeState == 0){
    RGB(OFF, OFF, ON);  //R G B
  }else if(DischargeState == 1) {
    RGB(ON, OFF, OFF);  //R G B
  }else{
    RGB(OFF, ON, OFF);  //R G B
  }
}

void ButtonAction(){
  if (ButtonPressType != 0){
    if(ButtonPressType == 2){  //Long press
      if ((EnDischarge == false) && ((DischargeState == 0) || (DischargeState == 2))){
        EnDischarge = true;
        DischargeState = 1;
        if ((DischargeVoltage + 0.05) < AvgCellVoltage){
          EnDischarge = true;
          DischargeState = 1;
        }
      }else if ((EnDischarge == true) && (DischargeState == 1)){
          EnDischarge = false;
          DischargeState = 0;
          digitalWrite(DISCHARGE_ON, LOW);
      }
      
    } else {  //Short press
      if (DischargeState != 1){
        if (DischargeVoltage <= 3.85){
          DischargeVoltage += 0.05;
        }else {
          DischargeVoltage = 3.60;
        }
      }else{
        if (DischargePage <= 1){
          DischargePage++;
        }else {
          DischargePage = 0;
        }

      }
    }
    ButtonPressType = 0;
    delay(300);
    ForceDisplayUpdate = true;
  }
}

void RGB(unsigned int r, unsigned int g, unsigned int b){
  analogWrite(LED_RED, map(r, 0, 255, 255, 0));
  analogWrite(LED_GREEN, map(g, 0, 255, 255, 0));
  analogWrite(LED_BLUE, map(b, 0, 255, 255, 0));
}

void AutoSetCellType() {
  CalculateBATVoltage();

  if ((BATVoltage > 0.00) && (BATVoltage <= 8.80)){
    CellType = 2;
  }else if ((BATVoltage > 8.80) && (BATVoltage <= 13.20)){
    CellType = 3;
  }else if ((BATVoltage > 13.20) && (BATVoltage <= 17.60)){
    CellType = 4;
  }else if ((BATVoltage > 17.60) && (BATVoltage <= 22.00)){
    CellType = 5;
  }else if ((BATVoltage > 22.00) && (BATVoltage <= 26.40)){
    CellType = 6;
  }
}
void PrintDataSerial() {

  Serial.print("Shunt Voltage [mV]: ");
  Serial.println();
  Serial.print("Bus Voltage [V]: ");
  Serial.println();
  Serial.print("Load Voltage [V]: ");
  Serial.println();
  Serial.print("Current[mA]: ");
  Serial.println();
  Serial.print("Bus Power [mW]: ");
  Serial.println();

  Serial.println();

  Serial.print("Direction: ");
  Serial.println();
  Serial.print("Counter: ");
  Serial.println();
  Serial.print("Button state:");
  Serial.println();

  Serial.println("______________________");
}
