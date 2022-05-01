//------------See AP.c for details of hardware connections to CC2650--------------------
//------------See LaunchPad.c for details of switches and LEDs--------------------------

#include <stdint.h>
#include "../inc/inc/Clock.h"
#include "../inc/inc/CortexM.h"
#include "../inc/inc/AP.h"
#include "../inc/inc/UART0.h"
#include "../inc/inc/UART1.h"
#include "../inc/inc/SSD1306.h"
#include "../inc/inc/motor.h"
#include "msp.h"


uint8_t  BT_ByteData;      // 8-bit user data from the phone


// ********OutValue**********
// Debugging dump of a data value to virtual serial port to PC
// data shown as 1 to 8 hexadecimal characters
// Inputs:  response (number returned by last AP call)
// Outputs: none
void ValueOut(char *label,uint32_t value){
  UART0_OutString(label);
  UART0_OutUHex(value);
}

void MoveRobot (uint8_t command) {
// this function calls the appropriate functions to stop, move forward, move backward, turn right,
// or turn left according to the command received from the BLE

        if(command == 1){
            Motor_Stop();
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- stop --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(40);
        }
        if(command == 2){
            Motor_Stop();
            Motor_Forward(15000, 15000);
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- forward --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(40);
        }
        if(command == 3){
            Motor_Stop();
            Motor_Right(15000, 15000);
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- right --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(40);
        }
        if(command == 4){
            Motor_Stop();
            Motor_Left(15000, 15000);
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- left --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(40);
        }
        if(command == 5){
            Motor_Stop();
            Motor_Backward(15000, 15000);
            SSD1306_ClearBuffer();
            SSD1306_DrawString(0, 0,"--- back --------",WHITE);
            SSD1306_DisplayBuffer();
            Clock_Delay1ms(40);
        }
}

void WriteByteData(void){ // called on a SNP Characteristic Write Indication on characteristic ByteData
  MoveRobot(BT_ByteData);   // send command to robot
  ValueOut("\n\rWrite BLE_ByteData=",BT_ByteData);
}


int main(void){
  volatile int r;

  DisableInterrupts();
  Clock_Init48MHz();
  UART0_Init();
  Timer_Init();
  Motor_Init();
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  EnableInterrupts();
  UART0_OutString("\n\rApplication Processor - MSP432-CC2650\n\r");
  r = AP_Init();
  AP_GetStatus();  // optional
  AP_GetVersion(); // optional
  AP_AddService(0xFFF0);
  //------------------------
  BT_ByteData = 0;  // write parameter from the phone will be used to control direction
  AP_AddCharacteristic(0xFFF1,1,&BT_ByteData,0x02,0x08,"DirectionData",0,&WriteByteData);

  //------------------------

  AP_RegisterService();
  AP_StartAdvertisementJacki();
  AP_GetStatus(); // optional
  while(1){
    AP_BackgroundProcess();  // handle incoming SNP frames

  }
}
