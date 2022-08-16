#include	"gpio.h"

//INTERNAL MACRO DEFINITIONS

#define TIME				1000000

// state machine
static void delay(unsigned int );
void ledON(gpioMod  ,ucPinNumber );
void ledOFF(gpioMod ,ucPinNumber );
	
int main(void){
	unsigned int count=0;
	ucPinNumber port=21;
	pinDirection dir=OUTPUT;
	
	//inicializando os modulos de GPIO
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
	gpioSetDirection(GPIO1, 28, dir); //led externo

	//setando o pin em nivel baixo (low level)
	for(count=port; count<25; count++){
		ledOFF(GPIO1, count);
		delay(1000);
	}
	ledOFF(GPIO1, 28);

	while(true){

		ledON(GPIO1, 28);
		delay(TIME);

			
			//todos os leds
				for(count=port; count<25; count++)
					ledON(GPIO1, count);
				delay(TIME);
				for(count=21; count<25; count++)
					ledOFF(GPIO1, count);
				delay(TIME);	
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