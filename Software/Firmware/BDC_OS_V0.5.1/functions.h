#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define ON  255
#define OFF 0

extern void HardwareInit();

extern void RGB(unsigned int r, unsigned int g, unsigned int b);

extern void PrintDataSerial();
extern void Discharge();

extern int DischargeState;
extern bool EnDischarge;

extern void AutoSetCellType();

extern void ButtonAction();

extern float DischargeVoltage;

extern void SetLED();

extern unsigned int GetSerialNumber();

extern void ActivateSleep();

extern long PreviousMillis2;

#endif
