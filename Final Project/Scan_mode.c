/*
 * Scan_mode.c
 *
 *  Created on: Apr 14, 2022
 *      Author: cmo5966
 */
#include "msp.h"
#include <msp432.h>
#include <stdint.h>
#include <stdbool.h>
#include "../inc/inc/Clock.h"
#include "../inc/inc/CortexM.h"
#include "../inc/inc/motor.h"

#define TRIGGER 0x04
#define ECHO 0x08

#define microsecondsToClockCycles(a) ( (a) * 1.5 )       //assume 12Mhz clock divided by 8
#define clockCyclesToMicroseconds(a) ( (a) / 1.5 )       // 1.5 clock cycles = 1us

extern uint8_t  BT_ByteData;

void TimerInit(void);
void Servo(uint16_t angle);
uint32_t pulseIn (void);

void UltraSonicInit(void)
{
         P6DIR |= 0b00000100;//make P6.2 an output for US trigger
         P6DIR &= 0b11110111;//make P6.3 an input for US echo
         P6SEL1 &= ~0b00001100;//put both pins in normal mode
         P6SEL0 &= ~0b00001100;

    return;
}
void ServoInit(void)
{
    P8DIR |= 0b00000100;     //make P8.2 an output : output from Timer3.2
    P8SEL1 &= ~0b00000100;
    P8SEL0 |= 0b00000100;     //set SEL1 and SEL0 for pwm function



    Servo(4400);     //call Servo() function to center servo
    Clock_Delay1us(1000000);     //delay here to give servo time to move
    TA3CTL &= ~0x0030;     //stop the timer
    return;
}
void Servo(uint16_t angle_count)
{
    TA3CCR0 = 59999;      //set period for 20ms
    TA3CCR2  = angle_count;      //set high time for the input angle
    TA3CTL  |= 0x0010;      //set timer for up mode
    TA3R = 0;
    return;
}
uint16_t distanceInCm(void) {
    uint16_t distance;
    float time;

    P6OUT |= TRIGGER;      //drive trigger pin p6.2 high
    Clock_Delay1us(10);      //wait 10 us
    P6OUT &= ~TRIGGER;      //drive trigger pin low
    time = pulseIn();
    distance = (time*0.034/2);      //calculate distance using s=t * 0.034/2. t comes from pulseIn() function
    if(distance == 0){
        distance = 400;
     }      // if no echo (distance = 0), assume object is at farthest distance
    return(distance);      //return the distance
}

uint32_t pulseIn (void)
{
    uint16_t width = 0;
    uint16_t time = 0;
    uint16_t maxcount = 56999;  //max count for 38 ms (timeout)

    TA2CCR0 = 56999;
    TA2CTL |= 0x0020; //set timer for continuous mode


    TA2R = 0;                   //reset the count
    while(!(P6IN & ECHO)){      //wait for the pulse to start (while Echo is low)
        if(TA2R >= maxcount){                  //if count is greater than maxcount return 0
            return(0);
        }
    }



    TA2R = 0;                   //reset the count
    while(P6IN & ECHO){                    //wait for the pulse to finish (while Echo is high)
        if(TA2R >= maxcount){                  //if count is greater than maxcount return 0
            return(0);
        }
    }

    width = TA2R;               //read the count (width of the return pulse)
    TA2CTL &= ~0x0030;               //stop the timer
    time = width*(1.5);               // convert the reading to microseconds.
    return(time);               //return the microsecond reading
}

void Scan_main(void)
{

    uint16_t distance, right_wall, left_wall;


    //These are the states of the state machine
    enum motor_states {FORWARD, BACKWARD, RIGHT, LEFT, SWEEP_RIGHT, SWEEP_LEFT} state, prevState;



    state = FORWARD;          //start in FORWARD state
    prevState = BACKWARD;   //used to know when the state has changed
    uint16_t stateTimer = 0;           //used to stay in a state
    bool isNewState;              //true when the state has switched


    while(BT_ByteData != 0) {

        AP_BackgroundProcess();  // handle incoming SNP frames
        isNewState = (state != prevState);
            prevState = state;
        distance = distanceInCm();  //this needs to be moved to the states that use it
        printf("%d", distance);

        switch (state) {
                case FORWARD:

                    if(isNewState){
                        distance = distanceInCm();
                        Motor_Forward(16000, 15000);

                    }
                    else if(distance <= 10){

                        state = BACKWARD;
                    }
                    break;
                case BACKWARD:
                    if(isNewState){
                        stateTimer = 0;
                        Motor_Stop();
                        Motor_Backward(16000, 15000);
                    }
                    stateTimer++;
                    if(stateTimer >= 40){
                        Motor_Stop();
                        state = SWEEP_RIGHT;
                    }
                        break;
                case SWEEP_RIGHT:
                    Servo(4400);
                    Clock_Delay1us(1000000);
                    Servo(735);
                    Clock_Delay1us(1000000);
                    distance = distanceInCm();
                    if(distance <= 150){
                        right_wall = distance;

                    }
                    else if(distance > 150){
                        right_wall = 400;

                    }
                    Servo(4400);
                    Clock_Delay1us(1000000);
                    state = SWEEP_LEFT;
                    break;
                           case SWEEP_LEFT:
                               Servo(4400);
                               Clock_Delay1us(1000000);
                               Servo(7500);
                               Clock_Delay1us(1000000);
                               distance = distanceInCm();
                               if(distance <= 150){
                                   left_wall = distance;
                               }
                               else if(distance > 150){
                                   left_wall = 400;
                               }
                                Servo(4400);
                                Clock_Delay1us(1000000);
                               if(left_wall > right_wall){
                                   state = LEFT;
                               }
                               else if(right_wall >= left_wall){
                                   state = RIGHT;
                               }
                               break;
                           case RIGHT:
                               if(isNewState){
                                   stateTimer=0;
                                   Motor_Forward(6500, 15000);
                               }

                               stateTimer++;
                               if(stateTimer >= 45){
                                   state = FORWARD;
                               }
                               break;
                           case LEFT:
                               if(isNewState){
                                   stateTimer=0;
                                   Motor_Forward(15000, 6500);
                               }

                               stateTimer++;
                               if(stateTimer >= 45){
                                   state = FORWARD;
                               }
                               break;

                    }

        Clock_Delay1ms(20);
    } //while
    return;
}
