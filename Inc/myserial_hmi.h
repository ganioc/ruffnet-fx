#ifndef __MYSERIAL_HMI_H_
#define __MYSERIAL_HMI_H_

#include "stm32f1xx_hal.h"
#include "main.h"
#include "cmsis_os.h"

//#define 
#define STATE_HMI_UNLOCK  0
//#define STATE_HMI_HEAD     1
#define STATE_HMI_BODY     2
//#define STATE_HMI_END       4
#define STATE_HMI_CRCH        5
#define STATE_HMI_CRCL         6

#define HEAD_HMI     0x02
#define END_HMI       0x03

#define HMI_RX_BUFFER_LEN    128

void Serial_Hmi_Init();
void Serial_Hmi_MspInit();
void Serial_Hmi_MspDeInit();
void  UART_Hmi_receive();

void UART_Hmi_Handle_Byte(uint8_t c);

void Hmi_Task_Init();

#endif