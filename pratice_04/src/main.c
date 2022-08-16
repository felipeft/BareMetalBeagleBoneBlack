#include "bbb_regs.h"
#include "hw_types.h"


bool flag1;
bool flag2;

typedef enum _pinNum{
	PIN1=1,
	PIN2,
	PIN3,
	PIN4
}pinNum;

//desabilita wdt
void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}

void delay(unsigned int mSec){
    volatile unsigned int count;
    
	for(count=0; count<mSec; count++);
}

void putCh(char c){
	while(!(HWREG(UART0_LSR) & (1<<5)));

	HWREG(UART0_THR) = c;
}

char getCh(){
	while(!(HWREG(UART0_LSR) & (1<<0)));

	return(HWREG(UART0_RHR));
}

int putString(char *str, unsigned int length){
	for(int i = 0; i < length; i++){
    	putCh(str[i]);
	}
	return(length);
}

int getString(char *buf, unsigned int length){
	for(int i = 0; i < length; i ++){
    	buf[i] = getCh();
   	}
	return(length);
}

void gpioSetup(){
	/* set clock for GPIO1, TRM 8.1.12.1.31 */
	HWREG(CM_PER_GPIO1_CLKCTRL) = 0x40002;

    /* Interrupt mask */
    HWREG(INTC_MIR_CLEAR3) |= (1<<2);//(98 --> Bit 2 do 4º registrador (MIR CLEAR3))
	HWREG(INTC_MIR_CLEAR3) |= (1<<3);//(99 --> Bit 3 do 4º registrador (MIR CLEAR3))
}

void butConfig ( ){
    /*  configure pin mux for input GPIO */
	
	HWREG(CM_PER_GPMCBEn1_REGS) |= 0x2F;
	HWREG(CM_PER_GPMCAD14_REGS) |= 0x2F;
	
    /* clear pin 23 for input, led USR0, TRM 25.3.4.3 */
    HWREG(GPIO1_OE) |= 1<<28;
	HWREG(GPIO1_OE) |= 1<<14;
	
	flag1 = false;
	flag2 = false;

    /* Setting interrupt GPIO pin. */
	HWREG(GPIO1_IRQSTATUS_SET_0) |= 1<<28;
	HWREG(GPIO1_IRQSTATUS_SET_1) |= 1<<14; 	 	

  	/* Enable interrupt generation on detection of a rising edge.*/
	HWREG(GPIO1_RISINGDETECT) |= 1<<28;	
	HWREG(GPIO1_RISINGDETECT) |= 1<<14;	
}


void ledConfig ( ){
    /*  configure pin mux for output GPIO */
	HWREG(CM_PER_GPMCA5_REGS) |= 0x7;
	HWREG(CM_PER_GPMCA6_REGS) |= 0x7;
    HWREG(CM_PER_GPMCA7_REGS) |= 0x7;
    HWREG(CM_PER_GPMCA8_REGS) |= 0x7;

    /* clear pin 23 and 24 for output, leds USR3 and USR4, TRM 25.3.4.3 */
	HWREG(GPIO1_OE) &= ~(1<<21);
	HWREG(GPIO1_OE) &= ~(1<<22);
    HWREG(GPIO1_OE) &= ~(1<<23);
    HWREG(GPIO1_OE) &= ~(1<<24);
	
}

unsigned int readBut ( ){
	unsigned int temp;
	temp = HWREG(GPIO1_DATAIN)&0x1000;
	
	return(temp);
}

void ledOff(pinNum pin){
	switch (pin) {
		case PIN1:
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<21);
		break;
		case PIN2:	
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<22);
		break;
		case PIN3:
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<23);
		break;
		case PIN4:
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<24);
		break;
		default:
		break;
	}
}

void ledOn(pinNum pin){
	
	switch (pin) {
		case PIN1:
			HWREG(GPIO1_SETDATAOUT) |= (1<<21);
		break;
		case PIN2:
			HWREG(GPIO1_SETDATAOUT) |= (1<<22);
		break;
		case PIN3:
			HWREG(GPIO1_SETDATAOUT) |= (1<<23);
		break;
		case PIN4:
			HWREG(GPIO1_SETDATAOUT) |= (1<<24);
		break;
		default:
		break;
	}
}

void gpioIsrHandler(void){

    /* Clear the status of the interrupt flags */
	HWREG(GPIO1_IRQSTATUS_0) = 1<<28; 

	flag1 = true;
}

void gpioIsrHandler2(void){

    /* Clear the status of the interrupt flags */
	HWREG(GPIO1_IRQSTATUS_1) = 1<<14; 

	flag2 = true;
}

void ISR_Handler(void){
	/* Verify active IRQ number */
	unsigned int irq_number = HWREG(INTC_SIR_IRQ) & 0x7f;
	
	if(irq_number == 98)
		gpioIsrHandler();
    
	if(irq_number == 99)
		gpioIsrHandler2();
	/* acknowledge IRQ */

	HWREG(INTC_CONTROL) = 0x1;
}

int main(void){
	
	/* Hardware setup */

	gpioSetup();	
	/*funçao que configura o clock do modulo 1, nela tambem é onde definimos qual interrupt deve ser usado*/

	ledConfig();
	/*nessa funçao, configuramos a multiplexação do pino de saida (no caso os leds)*/

	butConfig();
	/*nessa função, configuramos a multiplexação do pino de entrada (buttom)*/

	disableWdt();
	
	

	putString("gpio Interrupt...\n\r",19);
	ledOff(PIN1);
	ledOff(PIN2);
	delay(0x3FFFF);

	while(true){
		if(flag1){
			putString("button press!\n\r",15);
			ledOn(PIN1);
			delay(0x3FFFFFF);
			ledOn(PIN2);
			delay(0x3FFFFFF);
			ledOn(PIN3);
			delay(0x3FFFFFF);
			ledOn(PIN4);
			delay(0x3FFFFFF);
			ledOff(PIN4);
			ledOn(PIN1);
			ledOn(PIN2);
			ledOn(PIN3);
			delay(0x3FFFFFF);
			ledOff(PIN3);
			ledOn(PIN1);
			ledOn(PIN2);
			delay(0x3FFFFFF);
			ledOff(PIN2);
			ledOn(PIN1);
			delay(0x3FFFFFF);
			ledOff(PIN1);
			delay(0x3FFFFFF);
			flag1 = false;
		}
		else if(flag2){
			putString("button 2 press!\n\r",17);
			ledOn(PIN4);
			delay(0x3FFFFFF);
			ledOff(PIN4);
			ledOn(PIN3);
			delay(0x3FFFFFF);
			ledOff(PIN3);
			ledOn(PIN2);
			delay(0x3FFFFFF);
			ledOff(PIN2);
			ledOn(PIN1);
			delay(0x3FFFFFF);
			ledOff(PIN1);
			ledOn(PIN2);
			delay(0x3FFFFFF);
			ledOff(PIN2);
			ledOn(PIN3);
			delay(0x3FFFFFF);
			ledOff(PIN3);
			ledOn(PIN4);;
			delay(0x3FFFFFF);
			ledOff(PIN4);
			delay(0x3FFFFFF);
			flag2 = false;
		}
		else{
			ledOn(PIN3);
			ledOn(PIN4);
			ledOn(PIN1);
			ledOn(PIN2);
			delay(0x3FFFFFF);
			ledOff(PIN3);
			ledOff(PIN4);
			ledOff(PIN1);
			ledOff(PIN2);
			delay(0x3FFFFFF);		
		}
	}

	return(0);
}




