#include "msp.h"
#include "msp.h"
#include <msp432.h>
#include <stdint.h>
#include <stdbool.h>
#include "../inc/inc/Clock.h"
#include "../inc/inc/CortexM.h"
#include "../inc/inc/motor.h"
#include "../inc/inc/AP.h"
#include "../inc/inc/UART0.h"
#include "../inc/inc/UART1.h"
#include "../inc/inc/SSD1306.h"

uint8_t  BT_ByteData;      // 8-bit user data from the phone

void WriteByteData(void){ // called on a SNP Characteristic Write Indication on characteristic ByteData
  //MoveRobot(BT_ByteData);   // send command to robot
  ValueOut("\n\rWrite BLE_ByteData=",BT_ByteData);
}
void WriteManualData(void){ // called on a SNP Characteristic Write Indication on characteristic ByteData
  MoveRobot(BT_ByteData);   // send command to robot
  ValueOut("\n\rWrite BLE_ByteData=",BT_ByteData);
}


void main(void)
{
    volatile int r;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    Clock_Init48MHz();

    DisableInterrupts();
    UART0_Init();
    Timer_Init();
    Motor_Init();
    BumpInt_Init();
    UltraSonicInit();
    ServoInit();
    SSD1306_Init(SSD1306_SWITCHCAPVCC);
    EnableInterrupts();
    UART0_OutString("\n\rApplication Processor - MSP432-CC2650\n\r");
    r = AP_Init();
    AP_GetStatus();  // optional
    AP_GetVersion(); // optional
    AP_AddService(0xFFF0);
    //------------------------
    BT_ByteData = 9;  // write parameter from the phone will be used to control direction
    AP_AddCharacteristic(0xFFF1,1,&BT_ByteData,0x02,0x08,"DirectionData",0,&WriteByteData);
    //------------------------

    AP_RegisterService();
    AP_StartAdvertisementJacki();
    AP_GetStatus(); // optional

    enum states {PENDING, SCAN, BUMP, USER, HALT} state, prevState;

    state =  PENDING;                  //start state
    prevState =  HALT;          //used to know when the state has changed
    uint16_t stateTimer;       //used to stay in a state
    bool isNewState;           //true when the state has switched

    while(1){
      AP_BackgroundProcess();  // handle incoming SNP frames
      isNewState = (state != prevState);
      prevState = state;  //save state for next time

      switch(state){
case PENDING:
    SSD1306_ClearBuffer();
    SSD1306_DrawString(0, 0,"-- Mode Select --------",WHITE);
    SSD1306_DrawString(0, 32,"-- Press A for Scan --------",WHITE);
    SSD1306_DrawString(0, 45.5,"-- Press B for Bump --------",WHITE);
    SSD1306_DrawString(0, 56,"-- Press C for Manual ------",WHITE);
    SSD1306_DisplayBuffer();
    Clock_Delay1ms(40);
              if(BT_ByteData == 0){
                  state = HALT;
              }
              else if(BT_ByteData == 10){
                  state = SCAN;
              }
              else if(BT_ByteData == 11){
                  state = BUMP;
              }
              else if(BT_ByteData == 12){
                  state = USER;
              }

      break;

case SCAN:
          if(BT_ByteData == 0){
              state = HALT;
          }
          else {
              SSD1306_ClearBuffer();
              SSD1306_DrawString(0, 32,"--- Scan Mode --------",WHITE);
              SSD1306_DisplayBuffer();
              Clock_Delay1ms(40);
              Scan_main();
          }
          break;

case BUMP:
          if(BT_ByteData == 0){
              state = HALT;
          }
          else {
              SSD1306_ClearBuffer();
              SSD1306_DrawString(0, 32,"--- Bump Mode --------",WHITE);
              SSD1306_DisplayBuffer();
              Clock_Delay1ms(40);
              bumpers();
          }
          break;
case USER:
          if(BT_ByteData == 0){
              state = HALT;
          }
          else {
              MoveRobot(BT_ByteData);
          }
          break;
case HALT:
    Motor_Stop(); //halts motors
    state = PENDING;
    }
    }
}
