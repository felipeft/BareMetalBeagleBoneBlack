#include "bbb_regs.h"
#include "hw_types.h"

#define DELAY_USE_INTERRUPT			1

/**
 * \brief   This macro will check for write POSTED status
 *
 * \param   reg          Register whose status has to be checked
 *
 *    'reg' can take the following values \n
 *    DMTIMER_WRITE_POST_TCLR - Timer Control register \n
 *    DMTIMER_WRITE_POST_TCRR - Timer Counter register \n
 *    DMTIMER_WRITE_POST_TLDR - Timer Load register \n
 *    DMTIMER_WRITE_POST_TTGR - Timer Trigger register \n
 *    DMTIMER_WRITE_POST_TMAR - Timer Match register \n
 *
 **/
#define DMTimerWaitForWrite(reg)   \
            if(HWREG(DMTIMER_TSICR) & 0x4)\
            while((reg & HWREG(DMTIMER_TWPS)));


int flag_timer = false;
bool flag1 = false;
bool flag2 = false;

typedef enum pinNum{
	PIN1=1,
	PIN2,
	PIN3,
    PIN4,
    PIN5,
    PIN6
}pinNum;

void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
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

void timerEnable(){
    /* Espera a conclusão de gravação anterior no Timer control register*/
	DMTimerWaitForWrite(0x1);

    /* Start the timer */
    HWREG(DMTIMER_TCLR) |= 0x1;
}

void timerDisable(){
    /* Wait for previous write to complete in TCLR */
	DMTimerWaitForWrite(0x1);

    /* Start the timer */
    HWREG(DMTIMER_TCLR) &= ~(0x1);
}

void delay(unsigned int mSec){
#ifdef DELAY_USE_INTERRUPT
    unsigned int countVal = TIMER_OVERFLOW - (mSec * TIMER_1MS_COUNT);

   	/* Wait for previous write to complete */
	DMTimerWaitForWrite(0x2);

    /* Load the register with the re-load value */
	HWREG(DMTIMER_TCRR) = countVal;						//registro do contador de temporizador		
	
	flag_timer = false;

    /* Enable the DMTimer interrupts */
	HWREG(DMTIMER_IRQENABLE_SET) = 0x2; 

    /* Start the DMTimer */
	timerEnable();

    while(flag_timer == false);

    /* Disable the DMTimer interrupts */
	HWREG(DMTIMER_IRQENABLE_CLR) = 0x2; 
#else
    while(mSec != 0){
        /* Wait for previous write to complete */
        DMTimerWaitForWrite(0x2);

        /* Set the counter value. */
        HWREG(DMTIMER_TCRR) = 0x0;

        timerEnable();

        while(HWREG(DMTIMER_TCRR) < TIMER_1MS_COUNT);

        /* Stop the timer */
        HWREG(DMTIMER_TCLR) &= ~(0x00000001u);

        mSec--;
    }
#endif
}

void butConfig ( ){
	HWREG(CM_PER_GPMCA0_REGS) |= 0x2F;
	HWREG(CM_PER_GPMCBEn1_REGS) |= 0x2F;

    HWREG(GPIO1_OE) |= 1<<28;
	HWREG(GPIO1_OE) |= 1<<16;
	
	flag1 = false;
	flag2 = false;

	HWREG(GPIO1_IRQSTATUS_SET_0) |= 1<<28;
	HWREG(GPIO1_IRQSTATUS_SET_1) |= 1<<16; 	 	

	HWREG(GPIO1_RISINGDETECT) |= 1<<28;	
	HWREG(GPIO1_RISINGDETECT) |= 1<<16;	
}

void timerSetup(void){
     /*  Clock enable for DMTIMER7 TRM 8.1.12.1.25 */
    HWREG(CM_PER_TIMER7_CLKCTRL) |= 0x2;

	/*  Check clock enable for DMTIMER7 TRM 8.1.12.1.25 */    
    while((HWREG(CM_PER_TIMER7_CLKCTRL) & 0x3) != 0x2);

#ifdef DELAY_USE_INTERRUPT
    /* Interrupt mask */
    HWREG(INTC_MIR_CLEAR2) |= (1<<31);//(95 --> Bit 31 do 3º registrador (MIR CLEAR2))
#endif
}

void gpioSetup(){
	/* set clock for GPIO1, TRM 8.1.12.1.31 */
	HWREG(CM_PER_GPIO1_CLKCTRL) = 0x40002;

    /* Interrupt mask */
    HWREG(INTC_MIR_CLEAR3) |= (1<<2);//(98 --> Bit 2 do 4º registrador (MIR CLEAR3))
	HWREG(INTC_MIR_CLEAR3) |= (1<<3);//(99 --> Bit 3 do 4º registrador (MIR CLEAR3))

	//iniciando os leds
	/*  configure pin mux for output GPIO */
	HWREG(CM_PER_GPMCA5_REGS) |= 0x7;
	HWREG(CM_PER_GPMCA6_REGS) |= 0x7;
    HWREG(CM_PER_GPMCA7_REGS) |= 0x7;
    HWREG(CM_PER_GPMCAD12_REGS) |= 0X7;
    HWREG(CM_PER_GPMCAD14_REGS) |= 0X7;
    HWREG(CM_PER_GPMCAD15_REGS) |= 0X7;

    /* clear pin 23 and 24 for output, leds USR3 and USR4, TRM 25.3.4.3 */
	HWREG(GPIO1_OE) &= ~(1<<21);
	HWREG(GPIO1_OE) &= ~(1<<22);
    HWREG(GPIO1_OE) &= ~(1<<23);
    HWREG(GPIO1_OE) &= ~(1<<12);
    HWREG(GPIO1_OE) &= ~(1<<14);
    HWREG(GPIO1_OE) &= ~(1<<15);
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
            HWREG(GPIO1_CLEARDATAOUT) |= (1<<12);
        break;
        case PIN5:
            HWREG(GPIO1_CLEARDATAOUT) |= (1<<14);
        break;
        case PIN6:
            HWREG(GPIO1_CLEARDATAOUT) |= (1<<15);
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
            HWREG(GPIO1_SETDATAOUT) |= (1<<12);
        break;
        case PIN5:
            HWREG(GPIO1_SETDATAOUT) |= (1<<14);
        break;
        case PIN6:
            HWREG(GPIO1_SETDATAOUT) |= (1<<15);
        break;
		default:
		break;
	}
}

void timerIrqHandler(void){
    /* Clear the status of the interrupt flags */
	HWREG(DMTIMER_IRQSTATUS) = 0x2; 

	flag_timer = true;

    /* Stop the DMTimer */
	timerDisable();

}

void gpioIsrHandler(void){

    /* Clear the status of the interrupt flags */
	HWREG(GPIO1_IRQSTATUS_0) = 1<<28; 

	flag1 = true;
}

void gpioIsrHandler2(void){

    /* Clear the status of the interrupt flags */
	HWREG(GPIO1_IRQSTATUS_1) = 1<<16; 

	flag2 = true;
}

void ISR_Handler(void){
	/* Verifica se é interrupção do DMTIMER7 */
	unsigned int irq_number = HWREG(INTC_SIR_IRQ) & 0x7f;
	if(irq_number==95)
    timerIrqHandler();
	if(irq_number == 98)
		gpioIsrHandler();
    
	if(irq_number == 99)
		gpioIsrHandler2();

	/* Reconhece a IRQ */
	HWREG(INTC_CONTROL) = 0x1;
}

int main(void){
	char count=2;
	
	/* Hardware setup */
	gpioSetup();
	timerSetup();
	disableWdt();
	butConfig();

#ifdef DELAY_USE_INTERRUPT 
	putString("Timer Interrupt: ",17);
#else
	putString("Timer: ",7);
#endif
	while(count){
		putCh(0x30+count);
		putCh(' ');
		delay(1000);
		count--;
	}

	putString("...OK",5);
	while(true){

		if(flag1){
			putString("button press!\n\r",15);
			ledOff(PIN1);
			ledOff(PIN2);
			ledOff(PIN3);
            ledOff(PIN4);
            ledOff(PIN5);
            ledOff(PIN6);
			delay(1000);
			ledOn(PIN1);
			delay(1000);
			ledOn(PIN2);
			delay(1000);
			ledOn(PIN3);
			delay(1000);
            ledOn(PIN4);
            delay(1000);
            ledOn(PIN5);
            delay(1000);
            ledOn(PIN6);
            delay(1000);
            ledOff(PIN6);
            delay(1000);
            ledOff(PIN5);
            delay(1000);
            ledOff(PIN4);
            delay(1000);
			ledOff(PIN3);
			delay(1000);
			ledOff(PIN2);
			delay(1000);
			ledOff(PIN1);
			delay(1000);
			flag1 = false;

		}else if(flag2){
			putString("button 2 press!\n\r",17);
			ledOff(PIN1);
			ledOff(PIN2);
			ledOff(PIN3);
            ledOff(PIN4);
            ledOff(PIN5);
            ledOff(PIN6);
			delay(1000);
			ledOn(PIN1);
			ledOn(PIN2);
			ledOn(PIN3);
            ledOn(PIN4);
            ledOn(PIN5);
            ledOn(PIN6);
			delay(1000);
			ledOff(PIN1);
			ledOff(PIN2);
			ledOff(PIN3);
            ledOff(PIN4);
            ledOff(PIN5);
            ledOff(PIN6);
			delay(1000);
			ledOn(PIN1);
			ledOn(PIN2);
			ledOn(PIN3);
            ledOn(PIN4);
            ledOn(PIN5);
            ledOn(PIN6);
			delay(1000);
			ledOff(PIN1);
			ledOff(PIN2);
			ledOff(PIN3);
            ledOff(PIN4);
            ledOff(PIN5);
            ledOff(PIN6);
			delay(1000);
			ledOn(PIN1);
			ledOn(PIN2);
			ledOn(PIN3);
            ledOn(PIN4);
            ledOn(PIN5);
            ledOn(PIN6);
			delay(1000);
			ledOff(PIN1);
			ledOff(PIN2);
			ledOff(PIN3);
            ledOff(PIN4);
            ledOff(PIN5);
            ledOff(PIN6);
            
			flag2=false;
		}
		else{
			ledOn(PIN1);
			ledOn(PIN2);
			ledOn(PIN3);
            ledOn(PIN4);
            ledOn(PIN5);
            ledOn(PIN6);
		}
	}
	return(0);
}



