#include	"gpio.h"


//INTERNAL MACRO DEFINITIONS

#define TIME				1000000

// state machine
typedef enum _state{
            SEQ1=1,
            SEQ2,
            SEQ3,
            SEQ4,
	    SEQ5
}state;

static void delay(unsigned int );
void ledON(gpioMod  ,ucPinNumber );
void ledOFF(gpioMod ,ucPinNumber );
	
int main(void){
	unsigned int op=SEQ1, count=0;
	ucPinNumber port=21;
	pinDirection dir=OUTPUT;
	pinDirection dirB=INPUT;
	
	//inicializando o modulo 1
	gpioInitModule(GPIO1);

	//inicialização dos pinos do modulo
	for(count=port; count<25; count++){
		gpioInitPin(GPIO1, count);
		delay(1000);
	}
	gpioInitPin(GPIO1, 28);

	//setando a direçao do pino (E/S)
	for(count=port; count<25; count++){
		gpioSetDirection(GPIO1, count, dir);
		delay(1000);	
	}
	gpioSetDirection(GPIO1, 28, dirB); //botao 1

	//setando o pin em nivel baixo (low level)
	for(count=port; count<25; count++){
		ledOFF(GPIO1, count);
		delay(1000);
	}
	ledOFF(GPIO1, 28);

	while(true){

		switch (op){
			
			//primeiro led
			case SEQ1:	
			HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (0<<28);
				ledON(GPIO1, port);
				delay(TIME);
				ledOFF(GPIO1, port);
				delay(TIME);
				
				if(HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (1<<28)){
					op=SEQ2;
				}else{
					op=SEQ1;
				}
			break;

			//segundo led
			case SEQ2:	
			HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (0<<28);
				ledON(GPIO1, port+1);
				delay(TIME);
				ledOFF(GPIO1, port+1);
				delay(TIME);
				if(HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (1<<28)){
					op=SEQ3;
				}else{
					op=SEQ2;
				}
			break;
			
			//terceiro led
			case SEQ3:	
			HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (0<<28);
				ledON(GPIO1, port+2);
				delay(TIME);
				ledOFF(GPIO1, port+2);
				delay(TIME);
				if(HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (1<<28)){
					op=SEQ4;
				}else{
					op=SEQ3;
				}
			break;
			
			//quarto led
			case SEQ4:	
			HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (0<<28);
				ledON(GPIO1, port+3);
				delay(TIME);
				ledOFF(GPIO1, port+3);
				delay(TIME);
				if(HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (1<<28)){
					op=SEQ5;
				}else{
					op=SEQ4;
				}
			break;

			//todos os leds
			case SEQ5:
			HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (0<<28);
				for(count=port; count<25; count++)
					ledON(GPIO1, count);
				delay(TIME);
				for(count=21; count<25; count++)
					ledOFF(GPIO1, count);
				delay(TIME);
				if(HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) &= (1<<28)){
					op=SEQ1;
				}else{
					op=SEQ5;
				}
			break;
			default:	
			break;
		}
	}

	return(0);
}

void ledON(gpioMod mod, ucPinNumber pin){
	gpioSetPinValue(mod, pin, HIGH);	
}

void ledOFF(gpioMod mod,  ucPinNumber pin ){
	gpioSetPinValue(mod, pin, LOW);
}

static void delay(unsigned int uTime){
	volatile unsigned int ra;
	for(ra = 0; ra < uTime; ra ++);
}

