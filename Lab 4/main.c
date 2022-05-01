#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include "..\inc\inc\Clock.c"
#include "..\inc\inc\CortexM.c"


//Constants for motor pins
#define RIGHT_MOT_DIR   BIT5     //p5.5
#define RIGHT_MOT_SLEEP BIT6
#define RIGHT_MOT_PWM   BIT6
#define LEFT_MOT_DIR    BIT4
#define LEFT_MOT_SLEEP  BIT7
#define LEFT_MOT_PWM    BIT7
#define DelayTest       BIT7

void MotorInit (void)
//This function sets the motor pins as outputs and puts the motors to sleep
{
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
void TimerInit(void)
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
    TA2CTL &= ~0x0030;
    TA2CTL |= 0x0200; TA2CTL &= ~0x0100;
    TA2CTL |= 0b0000000010000000;
    TA2EX0 |= 0b0000000000000100;
    TA2CCR0 = 60000;

}
void Delay(){
    TA2R = 0;
    TA2CTL |= 0x0010;
    while(!(TA2CCTL0 & 0x0001)){}
    TA2CCTL0 &= ~0x0001;
    TA2CTL &= ~0x0030;

}

void Motor_Forward(uint16_t duty1, uint16_t duty2 ){
   // Run TimerA0 in PWM mode with provided duty cycle
   // Set motor controls for forward

   // turn on PWM and set duty cycle
   // fixed period of 10ms
    TA0CTL  |= 0x0010;  // Control bits 5 and 4 are mode control 00 to stop, 01 for up counting
                                        //         bits 7 and 6 are clock divider 01 = /2
                                        //         bits 9 and 8 choose clock 10 = SMCLK
    TA0R     = 0;              // Counter, start at zero once turned on
    TA0CCR3  = duty1;   // Capture/Compare 3 COMPARE MODE : holds value for comparison to timer TA0R
    TA0CCR4  = duty2;   // Capture/Compare 4 COMPARE MODE : holds value for comparison to timer TA0R


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
void Motor_Right(uint16_t duty1, uint16_t duty2 ){
   // Run TimerA0 in PWM mode with provided duty cycle
   // Set motor controls for forward

   // turn on PWM and set duty cycle
   // fixed period of 10ms
    TA0CTL  |= 0x0010;  // Control bits 5 and 4 are mode control 00 to stop, 01 for up counting
                                        //         bits 7 and 6 are clock divider 01 = /2
                                        //         bits 9 and 8 choose clock 10 = SMCLK
    TA0R     = 0;              // Counter, start at zero once turned on
    TA0CCR3  = duty1;   // Capture/Compare 3 COMPARE MODE : holds value for comparison to timer TA0R
    TA0CCR4  = duty2;   // Capture/Compare 4 COMPARE MODE : holds value for comparison to timer TA0R


     //left motor - START

    P5OUT &= ~0b00010000;   //DIRL on P5.4 (PH)
    P2OUT |= 0b10000000;       //PWML on P2.7 (EN)
    P3OUT |= 0b10000000;       //nSLPL on P3.7(nSLEEP)

 return;
}
void Motor_Left(uint16_t duty1, uint16_t duty2 ){
   // Run TimerA0 in PWM mode with provided duty cycle
   // Set motor controls for forward

   // turn on PWM and set duty cycle
   // fixed period of 10ms
    TA0CTL  |= 0x0010;  // Control bits 5 and 4 are mode control 00 to stop, 01 for up counting
                                        //         bits 7 and 6 are clock divider 01 = /2
                                        //         bits 9 and 8 choose clock 10 = SMCLK
    TA0R     = 0;              // Counter, start at zero once turned on
    TA0CCR3  = duty1;   // Capture/Compare 3 COMPARE MODE : holds value for comparison to timer TA0R
    TA0CCR4  = duty2;   // Capture/Compare 4 COMPARE MODE : holds value for comparison to timer TA0R


     //right motor - START
    P5OUT &= ~0b00100000;  //DIRR on P5.5 (PH)
    P2OUT |= 0b01000000;      //PWMR on P2.6 (EN)
    P3OUT |= 0b01000000;     //nSLPR on P3.6(nSLEEP)

 return;
}
void MotorStop (void)
//This function stops the motors by putting 0 on PWM pins and then puts
//motors to sleep
{
    P2OUT &= ~RIGHT_MOT_PWM & ~LEFT_MOT_PWM;       //stop motors
    P3OUT &= ~RIGHT_MOT_SLEEP & ~LEFT_MOT_SLEEP;  //put motors to sleep
    return;
}


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    Clock_Init48MHz();  // makes bus clock 48 MHz
    MotorInit();
    TimerInit();
    enum motor_states {FORWARD, RIGHT, LEFT, BACK, STOP} state, prevState;

    state =  FORWARD;                  //start state
    prevState =  STOP;          //used to know when the state has changed
    uint16_t stateTimer;       //used to stay in a state
    bool isNewState;           //true when the state has switched
    //declare enumerated states, declare starting state, declare previous state, declare state timer
    //declare boolean to know if state has switched
    while(1)
    {
                   isNewState = (state != prevState);
                   prevState = state;  //save state for next time

                  switch (state) {
                  //each case below should have entry housekeeping, state business and exit housekeeping
                  //remember to reset the stateTimer each time you enter a new state
                  //you must assign a new state when stateTer reaches the correct value
                  case FORWARD:
                      if(isNewState){
                          stateTimer=0;
                          Motor_Forward(15000, 15000);
                      }

                      stateTimer++;
                      if(stateTimer >= 20){
                          state = RIGHT;
                      }
                          break;
                  case RIGHT:
                      if(isNewState){
                          stateTimer=0;
                          MotorStop();
                          Motor_Right(15000, 15000);
                      }

                      stateTimer++;
                      if(stateTimer >= 10){
                          state = FORWARD;
                      }
                          break;

                  }
                  Delay();
                  //P6OUT ^= 0b10000000;

    } //while(1)
} //main()
