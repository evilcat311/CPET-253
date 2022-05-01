#include "msp.h"


#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include "..\inc\inc\Clock.h"
#include "..\inc\inc\CortexM.h"

volatile int32_t count;
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
       Clock_Init48MHz();  // makes bus clock 48 MHz

       Motor_Init();
       TimerInit();
       BumpInt_Init();

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
                                 count = 0;
                                 Motor_Forward(15000, 15000);
                             }
                             if(count != 0 ){
                                 state = BACK;
                             }
                                 break;
                         case BACK:
                             if(isNewState){
                                 stateTimer = 0;
                                 Motor_Stop();
                                 Motor_Backward(15000, 15000);
                             }
                             stateTimer++;
                              if(stateTimer >= 10){
                                  if(count > 0){
                                      state = LEFT;
                                  }
                                  else if(count < 0){
                                      state = RIGHT;
                                  }
                                  else{
                                      Motor_Stop();
                                  }
                                                   }
                                 break;
                         case RIGHT:
                             if(isNewState){
                                 stateTimer=0;
                                 Motor_Stop();
                                 Motor_Right(15000, 15000);
                             }
                             stateTimer++;
                             if(stateTimer >= 10){
                                 state = FORWARD;
                             }
                                 break;
                         case LEFT:
                             if(isNewState){
                                 stateTimer=0;
                                 Motor_Stop();
                                 Motor_Left(15000, 15000);
                             }
                             stateTimer++;
                             if(stateTimer >= 10){
                                 state = FORWARD;
                             }
                                 break;

}
                         Delay();
           }
}

