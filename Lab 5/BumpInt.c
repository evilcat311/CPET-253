// BumpInt.c
// Runs on MSP432, interrupt version
// Provide low-level functions that interface bump switches on the robot.
// Daniel Valvano and Jonathan Valvano
// July 11, 2019

/* This example accompanies the book
   "Embedded Systems: Introduction to Robotics,
   Jonathan W. Valvano, ISBN: 9781074544300, copyright (c) 2019
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2019, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// Negative logic bump sensors
// P4.7 Bump5, left side of robot
// P4.6 Bump4
// P4.5 Bump3
// P4.3 Bump2
// P4.2 Bump1
// P4.0 Bump0, right side of robot

#include <stdint.h>
#include <stdbool.h>
#include "msp.h"
#include "../inc/inc/CortexM.h"
#include "../inc/inc/Clock.h"



// Initialize Bump sensors
// Make six Port 4 pins inputs
// Activate interface pullup
// pins 7,6,5,3,2,0
// Interrupt on falling edge (on touch)
void BumpInt_Init(void){
    // write this as part of Lab 5
    P4DIR &= 0b00010010;
    P4REN  |=  0b11101101;          //enable resistor on P1.1
    P4OUT  |=  0b11101101;         //make them pull up resistors
    P4SEL0 &= 0b00010010;
    P4SEL1 &= 0b00010010;

    //enable interrupt
    P4IE |= 0b11101101;
    P1IES |= 0b11101101;
    NVIC -> ISER[1] |=  0x00000040;
    P4IFG &= 0b00010010;
    EnableInterrupts();
    return;

}

// triggered on touch, falling edge
void PORT4_IRQHandler(void){
    extern int32_t count;
    // write this as part of Lab 5
    uint16_t status = P4IV;
    if (status == 2){
        count = count + 3;
    }
    if (status == 6){
        count = count + 2;
    }
    if (status == 8){
        count = count + 1;
    }
    if (status == 12){
        count = count - 1;
    }
    if (status == 14){
        count = count - 2;
    }
    if (status == 16){
        count = count - 3;
    }
    printf("Count = %d \n", count);
    P4IFG &= 0b00010010;
    return;
}

