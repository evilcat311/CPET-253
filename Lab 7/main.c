/*
 * main.c
 *
 *  Created on: Mar 29, 2022
 *      Author: cmo5966
 */
#include <stdint.h>
#include "../inc/inc/Clock.h"
#include "../inc/inc/CortexM.h"
#include "../inc/inc/motor.h"
#include <reflectance.h>
#include <stdbool.h>
#include "msp.h"

int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    Clock_Init48MHz();
    Reflectance_Init();
    Timer_Init();
    Motor_Init();
    enum motor_states {FORWARD, RIGHT, LEFT, STOP} state, prevState;

           state =  FORWARD;                  //start state
           prevState =  STOP;          //used to know when the state has changed
           bool isNewState;           //true when the state has switched
    while(1){
       uint8_t data= Reflectance_Read(800);
       int32_t weight= Reflectance_Position(data);
        isNewState = (state != prevState);
        prevState = state;  //save state for next time

       switch (state) {
       case FORWARD:
           if(isNewState){


           Motor_Forward(9000, 9000);
           data = Reflectance_Read(800);
           weight = Reflectance_Position(data);
           }
           if(weight > 0){
              state = RIGHT;
          }
           else if(weight < 0){
              state = LEFT;
          }
           else{
               state = FORWARD;
           }
          break;
       case LEFT:
           if(isNewState){

           //Motor_Stop();
           Motor_Forward(5000, 9000);
           data = Reflectance_Read(800);
           weight = Reflectance_Position(data);
           }
           if(weight > 0){
               state = RIGHT;
           }
           else if(weight == 0){
               state = FORWARD;
           }
           else{
               state = LEFT;
           }

           break;
       case RIGHT:
           if(isNewState){


           //Motor_Stop();
           Motor_Forward(9000, 5000);
           data = Reflectance_Read(800);
           weight = Reflectance_Position(data);
           }
           if(weight < 0){
               state = LEFT;
           }
           else if(weight == 0){
               state = FORWARD;
           }
           else{
               state = RIGHT;
           }

           break;
       case STOP:
           if(isNewState){


           Motor_Stop();

           }
           break;
       }


       Delay();

    }
}




