#include "gpio.h"

/*-----------------------------------------------------------------------------
 * insert [pin, mod], get CM_conf adress
 * Datasheet 4.3.2
 *-----------------------------------------------------------------------------*/
static const CONTROL_MODULE GPIO_CTRL_MODULE_ARRAY[32][4] = {
    //p0                          //p1                          //p2                          //p3
   {CM_conf_mdio                 ,CM_conf_gpmc_ad0             ,CM_conf_gpmc_csn3            ,CM_conf_mii1_col       },//.0
   {CM_conf_mdc                  ,CM_conf_gpmc_ad1             ,CM_conf_gpmc_clk             ,CM_conf_mii1_crs       },//.1
   {CM_conf_spi0_sclk            ,CM_conf_gpmc_ad2             ,CM_conf_gpmc_advn_ale        ,CM_conf_mii1_rx_er     },//.2
   {CM_conf_spi0_d0              ,CM_conf_gpmc_ad3             ,CM_conf_gpmc_oen_ren         ,CM_conf_mii1_tx_en     },//.3
   {CM_conf_spi0_d1              ,CM_conf_gpmc_ad4             ,CM_conf_gpmc_wen             ,CM_conf_mii1_rx_dv     },//.4
   {CM_conf_spi0_cs0             ,CM_conf_gpmc_ad5             ,CM_conf_gpmc_ben0_cle        ,CM_conf_i2c0_sda       },//.5
   {CM_conf_spi0_cs1             ,CM_conf_gpmc_ad6             ,CM_conf_lcd_data0            ,CM_conf_i2c0_scl       },//.6
   {CM_conf_ecap0_in_pwm0_out    ,CM_conf_gpmc_ad7             ,CM_conf_lcd_data1            ,CM_conf_emu0           },//.7
   {CM_conf_lcd_data12           ,CM_conf_uart0_ctsn           ,CM_conf_lcd_data2            ,CM_conf_emu1           },//.8
   {CM_conf_lcd_data13           ,CM_conf_uart0_rtsn           ,CM_conf_lcd_data3            ,CM_conf_mii1_tx_clk    },//.9
   {CM_conf_lcd_data14           ,CM_conf_uart0_rxd            ,CM_conf_lcd_data4            ,CM_conf_mii1_rx_clk    },//.10
   {CM_conf_lcd_data15           ,CM_conf_uart0_txd            ,CM_conf_lcd_data5            ,-1                     },//.11
   {CM_conf_uart1_ctsn           ,CM_conf_gpmc_ad12            ,CM_conf_lcd_data6            ,-1                     },//.12
   {CM_conf_uart1_rtsn           ,CM_conf_gpmc_ad13            ,CM_conf_lcd_data7            ,CM_conf_usb1_drvvbus   },//.13
   {CM_conf_uart1_rxd            ,CM_conf_gpmc_ad14            ,CM_conf_lcd_data8            ,CM_conf_mcasp0_aclkx   },//.14
   {CM_conf_uart1_txd            ,CM_conf_gpmc_ad15            ,CM_conf_lcd_data9            ,CM_conf_mcasp0_fsx     },//.15
   {CM_conf_mii1_txd3            ,CM_conf_gpmc_a0              ,CM_conf_lcd_data10           ,CM_conf_mcasp0_axr0    },//.16
   {CM_conf_mii1_txd2            ,CM_conf_gpmc_a1              ,CM_conf_lcd_data11           ,CM_conf_mcasp0_ahclkr  },//.17
   {CM_conf_usb0_drvvbus         ,CM_conf_gpmc_a2              ,CM_conf_mii1_rxd3            ,CM_conf_mcasp0_aclkr   },//.18
   {CM_conf_xdma_event_intr0     ,CM_conf_gpmc_a3              ,CM_conf_mii1_rxd2            ,CM_conf_mcasp0_fsr     },//.19
   {CM_conf_xdma_event_intr1     ,CM_conf_gpmc_a4              ,CM_conf_mii1_rxd1            ,CM_conf_mcasp0_axr1    },//.20
   {CM_conf_mii1_txd1            ,CM_conf_gpmc_a5              ,CM_conf_mii1_rxd0            ,CM_conf_mcasp0_ahclkx  },//.21
   {CM_conf_gpmc_ad8             ,CM_conf_gpmc_a6              ,CM_conf_lcd_vsync            ,-1                     },//.22
   {CM_conf_gpmc_ad9             ,CM_conf_gpmc_a7              ,CM_conf_lcd_hsync            ,-1                     },//.23
   {-1                           ,CM_conf_gpmc_a8              ,CM_conf_lcd_pclk             ,-1                     },//.24
   {-1                           ,CM_conf_gpmc_a9              ,CM_conf_lcd_ac_bias_en       ,-1                     },//.25
   {CM_conf_gpmc_ad10            ,CM_conf_gpmc_a10             ,CM_conf_mmc0_dat3            ,-1                     },//.26
   {CM_conf_gpmc_ad11            ,CM_conf_gpmc_a11             ,CM_conf_mmc0_dat2            ,-1                     },//.27
   {CM_conf_mii1_txd0            ,CM_conf_gpmc_ben1            ,CM_conf_mmc0_dat1            ,-1                     },//.28
   {CM_conf_rmii1_ref_clk        ,CM_conf_gpmc_csn0            ,CM_conf_mmc0_dat0            ,-1                     },//.29
   {CM_conf_gpmc_wait0           ,CM_conf_gpmc_csn1            ,CM_conf_mmc0_clk             ,-1                     },//.30
   {CM_conf_gpmc_wpn             ,CM_conf_gpmc_csn2            ,CM_conf_mmc0_cmd             ,-1                     },//.31
};


static bool gpioCheckValidPortPin(gpioMod mod, ucPinNumber pin){
   if((mod < GPIO0) || (mod > GPIO3)){
      // TODO: raise error (mod is either too big or negative: /mod)
      return(false);
   }
   if((pin < 0) || (pin > 31)){
      // TODO: raise error (pin is either too big or negative: /pin)
      return(false);
   }
   return(true);
}


static bool gpioCheckValidDirection(pinDirection dir){
	if((dir!=INPUT) && (dir!=OUTPUT)){
      	// TODO: raise error (direction not valid: /dir)
      	return(false);
   	}
   	return(true);
}


void gpioInitModule(gpioMod mod){
	if(gpioCheckValidPortPin(mod, 0)){  // pin 0 always exist
    	unsigned int setting = (1<<18) | (0x2<<0);   //enable functional clock & enable module, TRM 8.1.12.1.29
      	switch(mod){
        	case GPIO0:
        	return;        // GPIO0 doesnt have a clock module register, TRM 8.1.12.1
        break;
        case GPIO1:
        	ckmSetCLKModuleRegister(CKM_PER, CKM_PER_GPIO1_CLKCTRL, setting);
        	while((ckmGetCLKModuleRegister(CKM_PER, CKM_PER_GPIO1_CLKCTRL) & (0x3<<16)) != 0)
        break;
        case GPIO2:
        	ckmSetCLKModuleRegister(CKM_PER, CKM_PER_GPIO2_CLKCTRL, setting);
        	while((ckmGetCLKModuleRegister(CKM_PER, CKM_PER_GPIO2_CLKCTRL) & (0x3<<16)) != 0) 
        break;
        case GPIO3:
        break;
        default:
        	// TODO: raise error (not possible, checked mod before: /mod)
        break;
      	}
	}
}


void gpioInitPin(gpioMod mod, ucPinNumber pin){
	if(gpioCheckValidPortPin(mod, pin)){
		CONTROL_MODULE module = GPIO_CTRL_MODULE_ARRAY[pin][mod];  // get conf <module> <pin> for mod/pin combination
      	padSetMode(module, MODE_7);  //set mode to GPIO, Datasheet 4.3.2
	}
}


void gpioSetDirection(gpioMod mod, ucPinNumber pin, pinDirection dir){
	unsigned int addr_temp, val_temp;

	if(gpioCheckValidPortPin(mod,pin)){
    	if(gpioCheckValidDirection(dir)){	
			switch (mod) {
			case GPIO0:
			break;
			case GPIO1:	
				// GPIOx base + output enable offset, TRM 2.1 & 25.4.1.16
    	   	 	addr_temp = SOC_GPIO_1_REGS + GPIO_OE;     
				// not overwriting previous mod setting
				val_temp =  HWREG(addr_temp);
			break;
			case GPIO2:
				// GPIOx base + output enable offset, TRM 2.1 & 25.4.1.16
    	   	 	addr_temp = SOC_GPIO_2_REGS + GPIO_OE;     
				// not overwriting previous mod setting
				val_temp =  HWREG(addr_temp);		
			break;
			case GPIO3:
			break;
			default:	
			break;
			}
        	
			val_temp &= ~(1<<pin);
        	val_temp |= (dir<<pin);
        		
			// writing new mod setting
			HWREG(addr_temp) = val_temp;
		}
   	}
}


int gpioGetDirection(gpioMod mod, ucPinNumber pin){
	unsigned int addr_temp, val_temp;

	if(gpioCheckValidPortPin(mod,pin)){
		switch (mod) {
			case GPIO0:	
				// GPIOx base + output enable offset, TRM 2.1 & 25.4.1.16
    			addr_temp = SOC_GPIO_0_REGS + GPIO_OE;      
    			val_temp = HWREG(addr_temp);
			break;
			case GPIO1:
				// GPIOx base + output enable offset, TRM 2.1 & 25.4.1.16
    			addr_temp = SOC_GPIO_1_REGS + GPIO_OE;      
    			val_temp = HWREG(addr_temp);
			break;
			case GPIO2:	
			break;
			case GPIO3:	
			break;
			default:
			break;
		}
   		if(val_temp & (1<<pin)) // masking for wanted pin
         	return(INPUT);
      	else
        	return(OUTPUT);
   	}else{
   		return(-1);     // isn't a valid mod/pin combination or doesnt exist
   	}
}


void gpioSetPinValue(gpioMod mod, ucPinNumber pin, pinLevel value){
	unsigned int addr_temp, val_temp;

	if(value == HIGH){
		if(gpioCheckValidPortPin(mod,pin)){
			switch (mod) {
				case GPIO0:	
				break;
				case GPIO1:
					// GPIOx base + set data out offset, TRM 2.1 & 25.4.1.26
      				addr_temp = SOC_GPIO_1_REGS + GPIO_SETDATAOUT;
					val_temp = 1<<pin;
				break;
				case GPIO2:	
					// GPIOx base + set data out offset, TRM 2.1 & 25.4.1.26
      				addr_temp = SOC_GPIO_2_REGS + GPIO_SETDATAOUT;
					val_temp = 1<<pin;	
				break;
				case GPIO3:	
				break;
				default:	
				break;
			}
      		HWREG(addr_temp) |= val_temp;
   		}
	}else{
		if(gpioCheckValidPortPin(mod,pin)){
			switch (mod) {
				case GPIO0:
				break;
				case GPIO1:
					// GPIOx base+clear data out offset, TRM 2.1 & 25.4.1.25
      				addr_temp = SOC_GPIO_1_REGS + GPIO_CLEARDATAOUT;        			
					val_temp = 1<<pin;
				break;
				case GPIO2:
					// GPIOx base+clear data out offset, TRM 2.1 & 25.4.1.25
      				addr_temp = SOC_GPIO_2_REGS + GPIO_CLEARDATAOUT;        			
					val_temp = 1<<pin;		
				break;
				case GPIO3:	
				break;
				default:	
				break;
			}
      		HWREG(addr_temp) &= val_temp;
   		}
	}
}


unsigned int gpioGetPinValue(gpioMod mod, ucPinNumber pin){
  	//unsigned int addr_temp, val_temp;
 
	if(gpioCheckValidPortPin(mod,pin)){
		switch (mod) {
			case GPIO0:
			break;
			case GPIO1:	
			break;
			case GPIO2:	
			break;
			case GPIO3:	
			break;
			default:	
			break;
		}	
		return(1);
   	}else{
    	return(-1);  // isnt a valid mod/pin combination or doesnt exist
   	}
}

