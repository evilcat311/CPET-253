//Connor Ott
//Microcontroller systems Section 5

#include "msp.h"
#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include "..\inc\inc\Clock.h"
#include "..\inc\inc\CortexM.h"


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	Clock_Init48MHz();  // makes bus clock 48 MHz
	PortInit();
	TimerInit();
	while(1){
	    if((P1IN & 0b00000010)==0){  //check for button press
	        Delay();  //call delay function for 225ms Delay
	        P6OUT ^= 0b00010000; //toggle pin 6.4 to measure waveform
	    }
	    else{ //else to catch when button stops being pressed
	        TA1CCTL0 &= ~0x0001; //clear flag
	        TA1CTL &= ~0x0030; //stop timer
	    }
	}
}


void PortInit()
{
    P6DIR |= 0b00010000; //initialize P6.4 for output
    P1DIR &= 0b11111101; //initialize button S1 for input
    P1REN  |=  0b00000010; //enable resistor for button S1
    P1OUT |= 0b00000010; //make resistor a pull up
}

void TimerInit(){
    TA1CTL &= ~0x0030; //Stop timer
    TA1CTL |= 0x0200; TA1CTL &= ~0x0100; //choose SMCLK
    TA1CTL |= 0x00C0; //clock divider 8
    TA1EX0 |= 0x0005; TA1EX0 &= ~0x0002; //second clock divider 6
    TA1CCR0 = 56250; //number of counts in 250ms


}
void Delay(){
    TA1R = 0; //clear timer
    TA1CTL |= 0x0010; //set timer to up mode and start timer
    while(!(TA1CCTL0 & 0x0001)){} //wait for flag
    TA1CCTL0 &= ~0x0001; //clear flag
    TA1CTL &= ~0x0030; //stop timer
}
