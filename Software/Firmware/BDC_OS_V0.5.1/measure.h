#ifndef MEASURE_H
#define MEASURE_H

extern unsigned int ButtonPressType;
extern void IRAM_ATTR ReadButton();

extern unsigned int CellType;

extern void CalculateBATVoltage();

extern float BATVoltage;
extern float AvgCellVoltage;
extern unsigned int TmpVbatAvg;
extern unsigned int DischargePercent;

#endif
