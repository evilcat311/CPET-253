/*
 * Manual_mode.c
 *
 *  Created on: Apr 14, 2022
 *      Author: cmo5966
 */
#include <stdint.h>
#include "../inc/inc/Clock.h"
#include "../inc/inc/CortexM.h"
#include "../inc/inc/AP.h"
#include "../inc/inc/UART0.h"
#include "../inc/inc/UART1.h"
#include "../inc/inc/SSD1306.h"
#include "../inc/inc/motor.h"
#include <stdio.h>
#include <stdlib.h>
#include "msp.h"


extern uint8_t  BT_ByteData;

void ValueOut(char *label,uint32_t value){
  UART0_OutString(label);
  UART0_OutUHex(value);
}

void MoveRobot (uint8_t prev) {
// this function calls the appropriate functions to stop, move forward, move backward, turn right,
// or turn left according to the command received from the BLE
    int speed_select_right[] = {7000, 9000, 11000, 13000, 15000};
    int speed_select_left[] = {8000, 10000, 12000, 14000, 16000};
    int right_speed = 0;
    int left_speed = 0;
    uint8_t command = (int)prev;

    while(command == 12 && BT_ByteData != 0){
        AP_BackgroundProcess();  // handle incoming SNP frames
        command = BT_ByteData;

        SSD1306_ClearBuffer();
        SSD1306_DrawString(0, 0,"--- Manual Mode --------",WHITE);
        SSD1306_DrawString(0, 32,"--- Enter Speed 1-5 ---",WHITE);
        SSD1306_DisplayBuffer();
        Clock_Delay1ms(40);
        if(command > 5 && command != 12){
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- Manual Mode --------",WHITE);
            SSD1306_DrawString(0, 32,"--- Invalid Speed -----",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(40);
            command = 12;
        }
        else if(command >= 1 && command <= 5){
            command = BT_ByteData;
            command--;
            left_speed = speed_select_left[command];
            right_speed = speed_select_right[command];
            BT_ByteData = 5;
        }
    }

    while(BT_ByteData != 0){


        AP_BackgroundProcess();  // handle incoming SNP frames
        command = BT_ByteData;
        if(command == 5){
            Motor_Stop();
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- Manual Mode --------",WHITE);
            SSD1306_DrawString(0, 32,"--- stop --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(40);
        }
        if(command == 8){
            Motor_Forward(left_speed, right_speed);
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- Manual Mode --------",WHITE);
            SSD1306_DrawString(0, 32,"--- forward --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(40);
        }
        if(command == 6){
            Motor_Forward(6500, 15000);
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- Manual Mode --------",WHITE);
            SSD1306_DrawString(0, 32,"--- right --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(1000);
            BT_ByteData = 8;
        }
        if(command == 4){
            Motor_Forward(15000, 6500);
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- Manual Mode --------",WHITE);
            SSD1306_DrawString(0, 32,"--- left --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(1000);
            BT_ByteData = 8;
        }
        if(command == 2){
            Motor_Backward(left_speed, right_speed);
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- Manual Mode --------",WHITE);
            SSD1306_DrawString(0, 32,"--- back --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(40);
        }
    }
     return;
}

