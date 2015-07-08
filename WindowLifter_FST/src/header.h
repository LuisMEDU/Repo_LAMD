#ifndef _HEADER_H_
#define _HEADER_H_

#include "typedefs.h"

#define BUTTON_UP          SIU.GPDI[64].R
#define BUTTON_DOWN        SIU.GPDI[65].R
#define APINCH_BUT         SIU.GPDI[66].R
#define PRESSED            0
#define TURN_ON 		   0
#define NO_DETECTED		   0
#define TURN_OFF		   1
#define GO_UP			   1
#define GO_DOWN			   2
#define MAN_MOVE		   1
#define ATM_MOVE		   2
#define QUINENTOS_MS	   50
#define BLUE_LED		   SIU.GPDO[10].R
#define GREEN_LED	 	   SIU.GPDO[11].R


/*Prototype Function*/
void initModesAndClock(void);
void disableWatchdog(void);
void LEDS_UP(T_UBYTE Num);
void LEDS_DOWN(T_UBYTE Num);
void input_output_declaration(void);
void delay_ms(unsigned int a);
void Step_Down(void);
void Step_UP(void);
void Automatic_Down(void);
void Automatic_UP(void);
void direction_det();
void aut_manual_det();
void antipinch();

#endif