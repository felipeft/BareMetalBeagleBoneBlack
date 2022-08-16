#ifndef GPIO_H_
#define GPIO_H_

#include "clock_module.h"
#include "control_module.h"
#include "pad.h"
#include "soc_AM335x.h"
#include "hw_types.h"

#define GPIO_REVISION           0x000
#define GPIO_SYSCONFIG          0x010
#define GPIO_EOI                0x020
#define GPIO_IRQSTATUS_RAW_0    0x024
#define GPIO_IRQSTATUS_RAW_1    0x028
#define GPIO_IRQSTATUS_0        0x02C
#define GPIO_IRQSTATUS_1        0x030
#define GPIO_IRQSTATUS_SET_0    0x034
#define GPIO_IRQSTATUS_SET_1    0x038
#define GPIO_IRQSTATUS_CLR_0    0x03C
#define GPIO_IRQSTATUS_CLR_1    0x040
#define GPIO_IRQWAKEN_0         0x044
#define GPIO_IRQWAKEN_1         0x048
#define GPIO_SYSSTATUS          0x114
#define GPIO_CTRL               0x130
#define GPIO_OE                 0x134
#define GPIO_DATAIN             0x138
#define GPIO_DATAOUT            0x13C
#define GPIO_LEVELDETECT0       0x140
#define GPIO_LEVELDETECT1       0x144
#define GPIO_RISINGDETECT       0x148
#define GPIO_FALLINGDETECT      0x14C
#define GPIO_DEBOUNCENABLE      0x150
#define GPIO_DEBOUNCINGTIME     0x154
#define GPIO_CLEARDATAOUT       0x190
#define GPIO_SETDATAOUT         0x194

typedef unsigned char ucPinNumber;

typedef enum _pinLevel{
	LOW,
	HIGH
}pinLevel;				

typedef enum _gpioMod{
	GPIO0,
	GPIO1,
	GPIO2,
	GPIO3
}gpioMod;				

typedef enum _pinDirection{
	OUTPUT,
	INPUT
}pinDirection;				

void gpioInitModule(gpioMod );
void gpioInitPin(gpioMod ,ucPinNumber );
void gpioSetDirection(gpioMod ,ucPinNumber ,pinDirection );	
int  gpioGetDirection(ucPinNumber ,ucPinNumber );
void gpioSetPinValue(gpioMod ,ucPinNumber ,pinLevel );
unsigned int gpioGetPinValue(gpioMod ,ucPinNumber );
	
#endif /*GPIO_H_ */
