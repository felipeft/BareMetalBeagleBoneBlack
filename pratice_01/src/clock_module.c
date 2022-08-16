#include "clock_module.h"
#include "hw_types.h"
#include "soc_AM335x.h"


static bool ckmCheckValidModule(CKM_MODULE_REG offset){
	if((offset > CKM_PER_CLK_24MHZ_CLKSTCTRL)){
      	return(false);
   	}
   	return(true);
}

void ckmSetCLKModuleRegister(CKM_MODULE base, CKM_MODULE_REG offset, unsigned int value){
   	if(ckmCheckValidModule(offset)){
		unsigned int addr_temp = base + offset; // clock base + offset, TRM 2.1 & 8.1.12.1
		HWREG(addr_temp) |= value;
  	}
}

unsigned int ckmGetCLKModuleRegister(CKM_MODULE base, CKM_MODULE_REG offset){
   	if(ckmCheckValidModule(offset)){
		unsigned int addr_temp = base + offset; // clock base + offset, TRM 2.1 & 8.1.12.1
      	return(HWREG(addr_temp));
   	}
   	return(0);
}

