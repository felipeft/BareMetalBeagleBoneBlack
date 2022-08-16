//Authors:  Alisson Rodrigues, Felipe Feitosa, Juan Rufino

#include	"hw_types.h"
#include	"soc_AM335x.h"

//INTERNAL MACRO DEFINITIONS

#define TIME											1000000
#define TOGGLE          								(0x01u)

#define CM_PER_GPIO1									0xAC
#define CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE   		(0x2u)
#define CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK   	(0x00040000u)

#define CM_conf_gpmc_ben1      	 						0x0878
#define CM_conf_gpmc_a5         						0x0854   
#define CM_conf_gpmc_a6                                 0x0858	
#define CM_conf_gpmc_a7                                 0x085C
#define CM_conf_gpmc_a8                                 0x0858

#define GPIO_OE                 						0x134
#define GPIO_CLEARDATAOUT       						0x190
#define GPIO_SETDATAOUT         						0x194


unsigned int flagBlink;

//INTERNAL FUNCTION PROTOTYPES

static void delay();
static void ledInit();
static void ledToggle();

int _main(void){

	flagBlink=0;	//init flag
  	
	/* Configure the green LED control pin. */
  	ledInit();
  
  	while (1){
    	/* Change the state of the green LED. */
    	ledToggle();
		delay();
		ledToggle();
		delay();
	}

	return(0);
}

static void delay(){
	volatile unsigned int ra;
	for(ra = 0; ra < TIME; ra ++);
}

void ledInit( ){
	
	unsigned int val_temp; 	
	/*-----------------------------------------------------------------------------
	 *  configure clock GPIO in clock module
	 *-----------------------------------------------------------------------------*/
	HWREG(SOC_CM_PER_REGS+CM_PER_GPIO1) |= CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK | CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;
	
	/*-----------------------------------------------------------------------------
	 * configure mux pin in control module
	 *-----------------------------------------------------------------------------*/
   	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a5) |= 7;
    HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a6) |= 7;
    HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a7) |= 7;
    HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a8) |= 7;
    HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_ben1) |= 7;
 
	/*-----------------------------------------------------------------------------
	 *  set pin direction 
	 *-----------------------------------------------------------------------------*/
	val_temp = HWREG(SOC_GPIO_1_REGS+GPIO_OE);
	val_temp &= ~(1<<28);
    val_temp &= ~(1<<21);
    val_temp &= ~(1<<22);
    val_temp &= ~(1<<23);
    val_temp &= ~(1<<24);
	
	HWREG(SOC_GPIO_1_REGS+GPIO_OE) = val_temp;
		
}

void ledToggle( ){
		
		flagBlink ^= TOGGLE;

		if(flagBlink){
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<28);
            HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<21);
            HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<22);
            HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<23);
            HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<24);
            }
		else{
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<28);
            HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<21);
            HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<22);
            HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<23);
            HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<24);
            }

}










