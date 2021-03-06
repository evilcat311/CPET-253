//******************************************************************//
//         CPET133 Lab1 ? I/O Port Review
//
// author: Jeanne Christman
// original version 1/6/2022
//
// This is a simple program that toggles an LED on every button push
//  -the LED is connected to port 4, pin 0 (via a 470 ohm resistor)
//  -the pushbutton is connect to port 6 pin 0
//       -the pushbutton is active low and has a 22K pull up resistor
//
//This program uses a protoboard board connected to the TI
//MSP432 Launchpad development board.
//*****************************************************************//
#include "msp.h"
#include <stdint.h>
#include <stdbool.h>

void main()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer - always include this
    //Configure the port pins as inputs and outputs
    //Use bitmasking
    int i;
    int j;
    P4DIR |= 0x01;      //P4.0 set as output
    P6DIR &= 0b11111110;      //P6.0 set as input;
    bool isSwPressed;
    isSwPressed = ((P6IN & 0b00000001)==0);
    //turn the LED on - use bitmasking
    P4OUT |= 0x01;
        for(i=10000; i>0; i--);
    P4OUT &= 0xFE;
        for(j=10000; j>0; j--);

    while(1) {

        if ((P6IN & 0b00000001)==0) {  //determine if button is pushed - active low
            P4OUT ^= 0x01;         //toggle LED
            }
            while ((P6IN & 0b00000001)==0); //wait here for pin to be released.
    }
    return;  //code can never get here
}
