// Motor.c
// Runs on MSP432
// Provide mid-level functions that initialize ports and
// set motor speeds to move the robot. Lab 13 solution
// Daniel Valvano
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

// Left motor direction connected to P5.4 (J3.29)
// Left motor PWM connected to P2.7/TA0CCP4 (J4.40)
// Left motor enable connected to P3.7 (J4.31)
// Right motor direction connected to P5.5 (J3.30)
// Right motor PWM connected to P2.6/TA0CCP3 (J4.39)
// Right motor enable connected to P3.6 (J2.11)

#include <stdint.h>
#include "msp.h"
#include "../inc/inc/CortexM.h"
#include "../inc/inc/Clock.h"

// *******Lab 4 Solution*******

// ------------Motor_Init------------
// Initialize GPIO pins for output, which will be
// used to control the direction of the motors and
// to enable or disable the drivers.
// The motors are initially stopped, the drivers
// are initially powered down, and the PWM speed
// control is uninitialized.
// Input: none
// Output: none
//Constants for motor pins
#define RIGHT_MOT_DIR   BIT5     //p5.5
#define RIGHT_MOT_SLEEP BIT6
#define RIGHT_MOT_PWM   BIT6
#define LEFT_MOT_DIR    BIT4
#define LEFT_MOT_SLEEP  BIT7
#define LEFT_MOT_PWM    BIT7
#define DelayTest       BIT7

void Motor_Init(void){
  // write this as part of Lab 4
    P6DIR |= DelayTest;
         //set direction pins as outputs
    P5DIR |= RIGHT_MOT_DIR;
    P5DIR |= LEFT_MOT_DIR;
         //set sleep pins as outputs
    P3DIR |= RIGHT_MOT_SLEEP;
    P3DIR |= LEFT_MOT_SLEEP;
         //set PWM pins as outputs
    P2DIR |= RIGHT_MOT_PWM;
    P2DIR |= LEFT_MOT_PWM;
         //put motors to sleep
    P3OUT &= 0b00111111;

    P2SEL1 &= 0b00111111;
    P2SEL0 |= 0b11000000;
    return;
  
}
void Timer_Init(void)
{

    //First initialize TimerA0 for PWM
    //Since the motors are connected to P2.6 and P2.7, use TimerA0, compare blocks 3 & 4
    //stop the timer
    //choose SMCLK for the clock source
    //choose clock divider of 2
    //set the period in TAxCCR0
    //Outmode 7: reset/set
    TA0CTL &= ~0x0030;
    TA0CTL |= 0x0200; TA0CTL &= ~0x0100;
    TA0CTL |= 0b0000000001000000;
    TA0CCTL3 |= 0x00E0;
    TA0CCTL4 |= 0x00E0;
    TA0CCR0 = 59999;
    TA0CTL |= 0x0010;

    //Now initialize TimerAx for the delay function
      //stop the timer
      //choose SMCLK for the clock source
      //choose clock divider of 4 : ID = 10
      //choose second clock divider in TAxEX0 of 5, total divide is 20

    TA1CTL &= ~0x0030;
    TA1CTL |= 0x0200; TA2CTL &= ~0x0100;
    TA1CTL |= 0b0000000010000000;
    TA1EX0 |= 0b0000000000000100;
    TA1CCR0 = 60000;

    TA2CTL &= ~0x0030;
    TA2CTL |= 0x0200; TA2CTL &= ~0x0100; //choose SMCLK
    TA2CTL |= 0x0C0; //clock divider 8

    TA3CTL &= ~0x0030;
    TA3CTL |= 0x0200; TA3CTL &= ~0x0100; //choose SMCLK
    TA3CCTL2 |= 0x00E0;
    TA3CTL |= 0x080; //clock divider 4




}
void Delay(){
    TA1R = 0;
    TA1CTL |= 0x0010;
    while(!(TA1CCTL0 & 0x0001)){}
    TA1CCTL0 &= ~0x0001;
    TA1CTL &= ~0x0030;

}

// ------------Motor_Stop------------
// Stop the motors, power down the drivers, and
// set the PWM speed control to 0% duty cycle.
// Input: none
// Output: none
void Motor_Stop(void){
  // write this as part of Lab 4
    P2OUT &= ~RIGHT_MOT_PWM & ~LEFT_MOT_PWM;       //stop motors
    P3OUT &= ~RIGHT_MOT_SLEEP & ~LEFT_MOT_SLEEP;  //put motors to sleep
    return;
  
}

// ------------Motor_Forward------------
// Drive the robot forward by running left and
// right wheels forward with the given duty
// cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Forward(uint16_t leftDuty, uint16_t rightDuty){ 
  // write this as part of Lab 4
    // Run TimerA0 in PWM mode with provided duty cycle
    // Set motor controls for forward

    // turn on PWM and set duty cycle
    // fixed period of 10ms
     TA0CTL  |= 0x0010;  // Control bits 5 and 4 are mode control 00 to stop, 01 for up counting
                                         //         bits 7 and 6 are clock divider 01 = /2
                                         //         bits 9 and 8 choose clock 10 = SMCLK
     TA0R     = 0;              // Counter, start at zero once turned on
     TA0CCR3  = leftDuty;   // Capture/Compare 3 COMPARE MODE : holds value for comparison to timer TA0R
     TA0CCR4  = rightDuty;   // Capture/Compare 4 COMPARE MODE : holds value for comparison to timer TA0R


     //left motor - START

     P5OUT &= ~0b00010000;   //DIRL on P5.4 (PH)
     P2OUT |= 0b10000000;       //PWML on P2.7 (EN)
     P3OUT |= 0b10000000;       //nSLPL on P3.7(nSLEEP)

      //right motor - START
     P5OUT &= ~0b00100000;  //DIRR on P5.5 (PH)
     P2OUT |= 0b01000000;      //PWMR on P2.6 (EN)
     P3OUT |= 0b01000000;     //nSLPR on P3.6(nSLEEP)

  return;
  
}

// ------------Motor_Backward------------
// Drive the robot backward by running left and
// right wheels backward with the given duty
// cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Backward(uint16_t leftDuty, uint16_t rightDuty){
  // write this as part of Lab 4
    // Run TimerA0 in PWM mode with provided duty cycle
    // Set motor controls for forward

    // turn on PWM and set duty cycle
    // fixed period of 10ms
     TA0CTL  |= 0x0010;  // Control bits 5 and 4 are mode control 00 to stop, 01 for up counting
                                         //         bits 7 and 6 are clock divider 01 = /2
                                         //         bits 9 and 8 choose clock 10 = SMCLK
     TA0R     = 0;              // Counter, start at zero once turned on
     TA0CCR3  = leftDuty;   // Capture/Compare 3 COMPARE MODE : holds value for comparison to timer TA0R
     TA0CCR4  = rightDuty;   // Capture/Compare 4 COMPARE MODE : holds value for comparison to timer TA0R


     //left motor - START

     P5OUT |= ~0b00010000;   //DIRL on P5.4 (PH)
     P2OUT |= 0b10000000;       //PWML on P2.7 (EN)
     P3OUT |= 0b10000000;       //nSLPL on P3.7(nSLEEP)

      //right motor - START
     P5OUT |= ~0b00100000;  //DIRR on P5.5 (PH)
     P2OUT |= 0b01000000;      //PWMR on P2.6 (EN)
     P3OUT |= 0b01000000;     //nSLPR on P3.6(nSLEEP)

  return;
}

// ------------Motor_Right------------
// Turn the robot to the right by running the
// left wheel forward and the right wheel
// backward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Right(uint16_t leftDuty, uint16_t rightDuty){
  // write this as part of Lab 4
    // Run TimerA0 in PWM mode with provided duty cycle
    // Set motor controls for forward

    // turn on PWM and set duty cycle
    // fixed period of 10ms
     TA0CTL  |= 0x0010;  // Control bits 5 and 4 are mode control 00 to stop, 01 for up counting
                                         //         bits 7 and 6 are clock divider 01 = /2
                                         //         bits 9 and 8 choose clock 10 = SMCLK
     TA0R     = 0;              // Counter, start at zero once turned on
     TA0CCR3  = leftDuty;   // Capture/Compare 3 COMPARE MODE : holds value for comparison to timer TA0R
     TA0CCR4  = rightDuty;   // Capture/Compare 4 COMPARE MODE : holds value for comparison to timer TA0R


      //left motor - START

     P5OUT &= ~0b00010000;   //DIRL on P5.4 (PH)
     P2OUT |= 0b10000000;       //PWML on P2.7 (EN)
     P3OUT |= 0b10000000;       //nSLPL on P3.7(nSLEEP)

  return;

}

// ------------Motor_Left------------
// Turn the robot to the left by running the
// left wheel backward and the right wheel
// forward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Left(uint16_t leftDuty, uint16_t rightDuty){
  // write this as part of Lab 4
    // Run TimerA0 in PWM mode with provided duty cycle
    // Set motor controls for forward

    // turn on PWM and set duty cycle
    // fixed period of 10ms
     TA0CTL  |= 0x0010;  // Control bits 5 and 4 are mode control 00 to stop, 01 for up counting
                                         //         bits 7 and 6 are clock divider 01 = /2
                                         //         bits 9 and 8 choose clock 10 = SMCLK
     TA0R     = 0;              // Counter, start at zero once turned on
     TA0CCR3  = leftDuty;   // Capture/Compare 3 COMPARE MODE : holds value for comparison to timer TA0R
     TA0CCR4  = rightDuty;   // Capture/Compare 4 COMPARE MODE : holds value for comparison to timer TA0R


      //right motor - START
     P5OUT &= ~0b00100000;  //DIRR on P5.5 (PH)
     P2OUT |= 0b01000000;      //PWMR on P2.6 (EN)
     P3OUT |= 0b01000000;     //nSLPR on P3.6(nSLEEP)

  return;
}


