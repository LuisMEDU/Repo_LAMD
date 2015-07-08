#include "MPC5606B.h"
#include "header.h"

T_UBYTE rub_UpDwnDtct=NO_DETECTED, rub_ValidPress=NO_DETECTED,rub_AtmMnl_dtct=NO_DETECTED,rub_500msCntr=0;
T_UBYTE rub_declare=0,rub_wndwpos=0,rub_nonAntPnch=0;
/*~~~~~~~Main Code~~~~~~~~~~~~~*/
void main(void) {

  initModesAndClock();    /* Initialize mode entries and system clock */
  disableWatchdog();      /* Disable watchdog */  
  
  input_output_declaration();
  
      
  for(;;){
  	if (rub_UpDwnDtct==NO_DETECTED)
  		direction_det();
  	else
  	{
  		switch(rub_AtmMnl_dtct)
  		{
 			case NO_DETECTED: aut_manual_det();
 				break;
 				
  			case ATM_MOVE:
  				if(rub_UpDwnDtct==GO_UP)
  					Automatic_UP();
  				else
  					Automatic_Down();
  				break;
  				
  			case MAN_MOVE:
  				if(rub_UpDwnDtct==GO_UP)
  					Step_UP();
  				else
  					Step_Down();
  				break;
  		}/*END SWITCH*/
  	}/*END ELSE*/
  	  	GREEN_LED=TURN_OFF;
  		BLUE_LED=TURN_OFF;
  }/*END FOR*/
  
}/*~~~END MAIN~~~*/


/*~~~~~~~~~FUNCTIONS~~~~~~~~~~~~~*/
void initModesAndClock(void) {
  ME.MER.R = 0x0000001D;          /* Enable DRUN, RUN0, SAFE, RESET modes */
                                  /* Initialize PLL before turning it on: */
  CGM.FMPLL_CR.R = 0x02400100;    /* 8 MHz xtal: Set PLL0 to 64 MHz */   
  ME.RUN[0].R = 0x001F0074;       /* RUN0 cfg: 16MHzIRCON,OSC0ON,PLL0ON,syclk=PLL0 */
  ME.RUNPC[1].R = 0x00000010; 	  /* Peri. Cfg. 1 settings: only run in RUN0 mode */
  ME.PCTL[32].R = 0x01; 	      /* MPC56xxB/P/S ADC 0: select ME.RUNPC[1] */	
  ME.PCTL[68].R = 0x01; 	      /* MPC56xxB/S SIU: select ME.RUNPC[1] */	
                                  /* Mode Transition to enter RUN0 mode: */
  ME.MCTL.R = 0x40005AF0;         /* Enter RUN0 Mode & Key */
  ME.MCTL.R = 0x4000A50F;         /* Enter RUN0 Mode & Inverted Key */  
  while (ME.GS.B.S_MTRANS) {}     /* Wait for mode transition to complete */    
                                  /* Note: could wait here using timer and/or I_TC IRQ */
  while(ME.GS.B.S_CURRENTMODE != 4) {} /* Verify RUN0 is the current mode */
}
/*-------------------------*/
void disableWatchdog(void) {
  SWT.SR.R = 0x0000c520;     /* Write keys to clear soft lock bit */
  SWT.SR.R = 0x0000d928; 
  SWT.CR.R = 0x8000010A;     /* Clear watchdog enable (WEN) */
}
/*-------------------------*/
void delay_ms(T_ULONG a)	/*Delay function*/{
	T_ULONG i,j;
	for(i=0;i<a;i++){
		for(j=0;j<15500;j++){}
	}
}
/*-------------------------*/
void input_output_declaration(void){
	/* Port E 0 to 2 as inputs*/
  SIU.PCR[64].R = 0x0103;      /* Cfg PE[0]input- KEY 1 on */
  SIU.PCR[65].R = 0x0103;      /* Cfg PE[0]input- KEY 2 on */
  SIU.PCR[66].R = 0x0103;      /* Cfg PE[0]input- KEY 3 on */
  
  	/*Outputs PA 0 to 11:...:0 to 11*/
  for (rub_declare=0;rub_declare<12;rub_declare++){
  	SIU.PCR[rub_declare].R = 0x0200;      /* LED: enable as outputs */
  	SIU.GPDO[rub_declare].R=1;
  }
}
/*-------------------------*/
void LEDS_UP(T_UBYTE Num){
	SIU.GPDO[Num].R=TURN_ON;
}

void LEDS_DOWN(T_UBYTE Num){
	SIU.GPDO[Num].R=TURN_OFF;
}
/*--------ATM UP-----------------*/
void Automatic_UP(void){
	rub_nonAntPnch=rub_wndwpos;
	for(rub_nonAntPnch;rub_nonAntPnch<=9;rub_nonAntPnch++){
		GREEN_LED=TURN_OFF;
		LEDS_UP(rub_nonAntPnch);
  	    BLUE_LED=TURN_ON;
  	    rub_wndwpos=rub_nonAntPnch;
  	    if(APINCH_BUT==PRESSED){
			Automatic_Down();
			delay_ms(5000);
			rub_nonAntPnch=11;
		}
  	    delay_ms(400);
	}
	rub_UpDwnDtct=NO_DETECTED;
	rub_ValidPress=NO_DETECTED;
  	rub_AtmMnl_dtct=NO_DETECTED;
  	rub_500msCntr=0;
  	antipinch();
}
/*-----------ATM DOWN--------------*/
void Automatic_Down(void){
	while(rub_wndwpos>0){
		BLUE_LED=TURN_OFF;
  		LEDS_DOWN(rub_wndwpos);
  	    GREEN_LED=TURN_ON;
  	    if(rub_wndwpos>0)
  	      rub_wndwpos--;
  	    delay_ms(400);
  	    if(rub_wndwpos==0){
  	      LEDS_DOWN(rub_wndwpos);
  	      GREEN_LED=TURN_ON;
  	      delay_ms(400);
  	    }
  	}
  	rub_UpDwnDtct=NO_DETECTED;
  	rub_ValidPress=NO_DETECTED;
  	rub_AtmMnl_dtct=NO_DETECTED;
  	rub_500msCntr=0;
  	GREEN_LED=TURN_OFF;
}
/*----------STEP UP---------------*/
void Step_UP(void){
	while (BUTTON_UP==PRESSED){
		GREEN_LED=TURN_OFF;
		LEDS_UP(rub_wndwpos);
	  	BLUE_LED=TURN_ON;
  		if(rub_wndwpos<9)
  	  	rub_wndwpos++;
	  	delay_ms(400);
	  	antipinch();
	}
	rub_UpDwnDtct=NO_DETECTED;
	rub_ValidPress=NO_DETECTED;
  	rub_AtmMnl_dtct=NO_DETECTED;
  	rub_500msCntr=0;
  	antipinch();
}
/*------------STEP DOWN-------------*/
void Step_Down(void){
	while(BUTTON_DOWN==PRESSED)
	{
		BLUE_LED=TURN_OFF;
		LEDS_DOWN(rub_wndwpos);
	  	GREEN_LED=TURN_ON;
	  	if(rub_wndwpos>0)
		  rub_wndwpos--;
	  	delay_ms(400);
	}
  	rub_UpDwnDtct=NO_DETECTED;
  	rub_ValidPress=NO_DETECTED;
  	rub_AtmMnl_dtct=NO_DETECTED;
  	rub_500msCntr=0;
}

/*-----------Direct. det.--------------*/
void direction_det(){
	T_UBYTE lub_butdet=0;
	if(BUTTON_UP == PRESSED)
		lub_butdet=1;
	if(BUTTON_DOWN == PRESSED)
		lub_butdet=1;
	
	if(lub_butdet==1){
		delay_ms(1);
		rub_ValidPress++;
		lub_butdet=0;
	}
	else{rub_ValidPress=0;}
	
	if(rub_ValidPress>=10){
		if(BUTTON_UP==PRESSED)
			rub_UpDwnDtct=GO_UP;
		else
			rub_UpDwnDtct=GO_DOWN;
	}
}
/*----------ATO OR MAN DET -------------*/
void aut_manual_det()
{
	T_UBYTE lub_butdet=TURN_OFF;
	if(BUTTON_UP == PRESSED)
		lub_butdet=PRESSED;
	if(BUTTON_DOWN == PRESSED)
		lub_butdet=TURN_ON;
	
	if(lub_butdet==PRESSED){
		rub_500msCntr++;
		delay_ms(10);
	}
	else
	{
		if(rub_500msCntr<=QUINENTOS_MS)
			rub_AtmMnl_dtct=ATM_MOVE;
	}
	if(rub_500msCntr>QUINENTOS_MS)
		rub_AtmMnl_dtct=MAN_MOVE;
}
/*-------ANTI-PINCH---------------*/
void antipinch()
{
	if(APINCH_BUT==PRESSED){
		Automatic_Down();
		delay_ms(5000);
	}
	else{}
}