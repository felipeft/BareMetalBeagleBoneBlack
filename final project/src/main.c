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

#define rounds_to_win 7
int flag_timer = false;
bool flag1 = false;
bool flag2 = false;
bool flag3 = false;
bool flag4 = false;
int rounds;			//contador de rounds do jogo
int seq[32];		//vetor que armazena a sequencia correta
int seqBut[32];		//vetor que armazena a sequencia apertada nos botoes

typedef enum pinNum{
	PIN1=1,
	PIN2,
	PIN3,
    PIN4
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
}/* -----  end of function timerEnable  ----- */

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

	//iniciando os leds
	/*  configure pin mux for output GPIO */
	HWREG(CM_PER_GPMCA5_REGS) |= 0x7;
	HWREG(CM_PER_GPMCA6_REGS) |= 0x7;
    HWREG(CM_PER_GPMCA7_REGS) |= 0x7;
	HWREG(CM_PER_GPMCA8_REGS) |= 0x7;

	HWREG(CM_PER_GPMCAD12_REGS) |= 0x7;
	HWREG(CM_PER_GPMCAD13_REGS) |= 0x7;
	HWREG(CM_PER_GPMCAD14_REGS) |= 0x7;
    HWREG(CM_PER_GPMCAD15_REGS) |= 0x7;


    // clear pin for output
	HWREG(GPIO1_OE) &= ~(1<<21);
	HWREG(GPIO1_OE) &= ~(1<<22);
    HWREG(GPIO1_OE) &= ~(1<<23);
	HWREG(GPIO1_OE) &= ~(1<<24);

	HWREG(GPIO1_OE) &= ~(1<<12);
	HWREG(GPIO1_OE) &= ~(1<<13);
    HWREG(GPIO1_OE) &= ~(1<<14);
	HWREG(GPIO1_OE) &= ~(1<<15);

}

void ledOff(pinNum pin){
	switch (pin) {
		case PIN1:
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<21);
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<12);
		break;
		case PIN2:	
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<22);
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<13);
		break;
		case PIN3:
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<23);
			HWREG(GPIO1_CLEARDATAOUT) |= (1<<14);
		break;
        case PIN4:
            HWREG(GPIO1_CLEARDATAOUT) |= (1<<24);
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
			HWREG(GPIO1_SETDATAOUT) |= (1<<12);
		break;
		case PIN2:
			HWREG(GPIO1_SETDATAOUT) |= (1<<22);
			HWREG(GPIO1_SETDATAOUT) |= (1<<13);
		break;
		case PIN3:
			HWREG(GPIO1_SETDATAOUT) |= (1<<23);
			HWREG(GPIO1_SETDATAOUT) |= (1<<14);
		break;
        case PIN4:
            HWREG(GPIO1_SETDATAOUT) |= (1<<24);
			HWREG(GPIO1_SETDATAOUT) |= (1<<15);
        break;
		default:
		break;
	}
}

void butConfig(){
	//Seleção do mux de sinal funcional do pad
	HWREG(CM_PER_GPMCA0_REGS) 		|= 0x2F;
	HWREG(CM_PER_GPMCBEn1_REGS) 	|= 0x2F;
	HWREG(CM_PER_GPMCA1_REGS) 		|= 0x2F;
	HWREG(CM_PER_GPMCCSn0_REGS) 	|= 0x2F;
	
	//habilita recurso de output dos pinos
	HWREG(GPIO1_OE) |= 1<<16;
    HWREG(GPIO1_OE) |= 1<<28;
	HWREG(GPIO1_OE) |= 1<<17;
	HWREG(GPIO1_OE) |= 1<<29;
	
	flag1 = false;
	flag2 = false;
	flag3 = false;
	flag4 = false;

	//nos diz o status para decidir o que fazer com a interrupção
	HWREG(GPIO1_IRQSTATUS_SET_0) |= 1<<16; 	 
	HWREG(GPIO1_IRQSTATUS_SET_0) |= 1<<28; 
	HWREG(GPIO1_IRQSTATUS_SET_0) |= 1<<17; 
	HWREG(GPIO1_IRQSTATUS_SET_0) |= 1<<29; 
		
	//habilita quando esta em borda de subida (rising)
	HWREG(GPIO1_RISINGDETECT) |= 1<<16;	
	HWREG(GPIO1_RISINGDETECT) |= 1<<28;	
	HWREG(GPIO1_RISINGDETECT) |= 1<<17;	
	HWREG(GPIO1_RISINGDETECT) |= 1<<29;		
	
}

void timerIrqHandler(void){
    /* Clear the status of the interrupt flags */
	HWREG(DMTIMER_IRQSTATUS) = 0x2; 

	flag_timer = true;

    /* Stop the DMTimer */
	timerDisable();

}

void delai(unsigned int mSec){
    volatile unsigned int count;
	for(count=0; count<mSec; count++);
}

void gpioIsrHandler(void){

    /* Clear the status of the interrupt flags */
	HWREG(GPIO1_IRQSTATUS_0) = 1<<28; 

	flag1 = true;

	ledOn(PIN1);
	delai(0x3FFFF);
	ledOff(PIN1);
}

void gpioIsrHandler2(void){

    /* Clear the status of the interrupt flags */
	HWREG(GPIO1_IRQSTATUS_0) = 1<<16; 

	flag2 = true;

	ledOn(PIN2);
	delai(0x3FFFF);
	ledOff(PIN2);
}

void gpioIsrHandler3(void){

    /* Clear the status of the interrupt flags */
	HWREG(GPIO1_IRQSTATUS_0) = 1<<17; 

	flag3 = true;

	ledOn(PIN3);
	delai(0x3FFFF);
	ledOff(PIN3);
}

void gpioIsrHandler4(void){

    /* Clear the status of the interrupt flags */
	HWREG(GPIO1_IRQSTATUS_0) = 1<<29; 

	flag4 = true;

	ledOn(PIN4);
	delai(0x3FFFF);
	ledOff(PIN4);
}


void ISR_Handler(void){
	/* Verifica se é interrupção do DMTIMER7 */
	unsigned int irq_number = HWREG(INTC_SIR_IRQ) & 0x7f;
	if(irq_number==95)
    timerIrqHandler();
	if(irq_number == 98){
		if(HWREG(GPIO_IRQSTATUS_RAW_0) & (1<<28)){
			gpioIsrHandler();
		}
		else if(HWREG(GPIO_IRQSTATUS_RAW_0) & (1<<16)){
			gpioIsrHandler2();
		}	
		else if(HWREG(GPIO_IRQSTATUS_RAW_0) & (1<<17)){
			gpioIsrHandler3();
		}	
		else if(HWREG(GPIO_IRQSTATUS_RAW_0) & (1<<29)){
			gpioIsrHandler4();
		}	
	}
		
	/* Reconhece a IRQ */
	HWREG(INTC_CONTROL) = 0x1;
}


//vetor que armazena a sequencia de pisca correta
//será feito dessa forma pois n foi possivel fazer isso de forma randomica
int r[13] = {1, 0, 2, 1, 3, 0, 3, 1, 1, 0, 2, 3};

void playOnLeds(){
	delay(700);
    for(int i=0; i<=rounds; i++){
		if(seq[i] == 0){
			ledOn(PIN1);
			delay(500);
			ledOff(PIN1);
			delay(500);
		}
		else if(seq[i] == 1){
			ledOn(PIN2);
			delay(500);
			ledOff(PIN2);
			delay(500);
		}
		else if(seq[i] == 2){
			ledOn(PIN3);
			delay(500);
			ledOff(PIN3);
			delay(500);
		}
		else if(seq[i] == 3){
			ledOn(PIN4);
			delay(500);
			ledOff(PIN4);
			delay(500);
		}
	}
}

bool waitForAnyButtom(){
	if(flag1 || flag2 || flag3 || flag4){
		flag1 = false;
		flag2 = false;
		flag3 = false;
		flag4 = false;
		return true;
	}
	else{
		return false;
	}
}


//funçao para ler seq de apertos de botoes e converter isso para um vetor
void readChoose(){
		for(int i=0; i<=rounds; i++){
			while(1){
				if(flag1){
					seqBut[i] = 0;
					//putString("botao 1 pressionado\n\r",22);
					flag1 = false;
					break;
				}
				else if(flag2){
					seqBut[i] = 1;
					//putString("botao 2 pressionado\n\r",22);
					flag2 = false;
					break;
				}
				else if(flag3){
					seqBut[i] = 2;
					//putString("botao 3 pressionado\n\r",22);
					flag3 = false;
					break;
				}
				else if(flag4){
					seqBut[i] = 3;
					//putString("botao 4 pressionado\n\r",22);
					flag4 = false;
					break;
				}
			}
		}
}

//funçao que compara a seq de leds com a seq de apertos nos botoes
int comparator(){
	int count = 0;		//contador para contar a quantidade de erros
	for(int i=0; i<=rounds; i++){
		if(seqBut[i] != seq[i]){
			count++;
		}
	}
	return count; //caso tenha 1 erro, retorna mais de 0, senao retorna 0
}

bool memoria(){
	rounds = 0;
	while(rounds < rounds_to_win){
		seq[rounds] = r[rounds];
		playOnLeds();
		readChoose();
		if(comparator() == 0){
			rounds++;
		}
		else{
			return false;
		}
	}
	return true;
}

int main(void){
	char count=2;
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

	while(true){
		putString("\e[1;1H\e[2J",11);
		putString("\n\rPressione qualquer botao para iniciar o jogo!\n\r",50);
		ledOn(PIN1);
		ledOn(PIN2);
		ledOn(PIN3);
		ledOn(PIN4);
		delay(1000);
		ledOff(PIN1);
		ledOff(PIN2);
		ledOff(PIN3);
		ledOff(PIN4);
		delay(1000);

		if(waitForAnyButtom()){
			putString("jogo inicia em 3s!\n\r",21);
			putString("3\n\r",4);
			delay(1000);
			putString("2\n\r",4);
			delay(1000);
			putString("1\n\r",4);
			delay(1000);
			putString("go!\n\r",6);
			if(memoria()){
				putString("Parabens campeao!\n\r",20);
			}
			else{
				putString("Errou :(\n\r",11);
			}
			delay(3000);
		}
	}
	return 0;
}




