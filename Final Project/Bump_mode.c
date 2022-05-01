/*
 * Bump_mode.c
 *
 *  Created on: Apr 14, 2022
 *      Author: cmo5966
 */
#include "msp.h"


#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include "../inc/inc/Clock.h"
#include "../inc/inc/CortexM.h"
#include "../inc/inc/SSD1306.h"

volatile int32_t count;
extern uint8_t  BT_ByteData;

void bumpers(void){
    BumpInt_Init();
    enum motor_states {FORWARD, RIGHT, LEFT, BACK, STOP} state, prevState;

           state =  FORWARD;                  //start state
           prevState =  STOP;          //used to know when the state has changed
           uint16_t stateTimer;       //used to stay in a state
           bool isNewState;           //true when the state has switched
           //declare enumerated states, declare starting state, declare previous state, declare state timer
           //declare boolean to know if state has switched
           while(BT_ByteData != 0)
               {
               AP_BackgroundProcess();  // handle incoming SNP frames
                              isNewState = (state != prevState);
                              prevState = state;  //save state for next time

                             switch (state) {
                             //each case below should have entry housekeeping, state business and exit housekeeping
                             //remember to reset the stateTimer each time you enter a new state
                             //you must assign a new state when stateTer reaches the correct value
                             case FORWARD:
                                 if(isNewState){
                                     SSD1306_ClearBuffer();
                                     SSD1306_DrawString(0, 32,"--- Bump Mode --------",WHITE);
                                     SSD1306_DisplayBuffer();
                                     Clock_Delay1ms(40);
                                     count = 0;
                                     Motor_Forward(16000, 15000);
                                 }
                                 if(count != 0 ){
                                     state = BACK;
                                 }
                                     break;
                             case BACK:
                                 if(isNewState){
                                     stateTimer = 0;
                                     Motor_Stop();
                                     Motor_Backward(16000, 15000);
                                 }
                                 stateTimer++;
                                  if(stateTimer >= 20){
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
                                     Motor_Forward(6500, 15000);
                                 }
                                 stateTimer++;
                                 if(stateTimer >= 17){
                                     state = FORWARD;
                                 }
                                     break;
                             case LEFT:
                                 if(isNewState){
                                     stateTimer=0;
                                     Motor_Forward(15000, 6500);
                                 }
                                 stateTimer++;
                                 if(stateTimer >= 17){
                                     state = FORWARD;
                                 }
                                     break;

    }
                             Delay();
               }
           return;

}

