#ifndef __MYSERIAL_PLC_H_
#define __MYSERIAL_PLC_H_


#include "stm32f1xx_hal.h"
#include "main.h"
#include "cmsis_os.h"

#define STATE_PLC_UNLOCK  0
//#define STATE_PLC_HEAD     1
#define STATE_PLC_BODY     2
//#define STATE_PLC_END       4
#define STATE_PLC_CRCH        5
#define STATE_PLC_CRCL         6

#define HEAD_PLC     0x02
#define END_PLC       0x03

#define PLC_RX_BUFFER_LEN    128
#define PLC_QUEUE_LEN            128

#define PLC_MESSAGE_TIMEOUT   500

void Serial_Plc_Init();
void Serial_Plc_MspInit();
void Serial_Plc_MspDeInit();
void  UART_Plc_receive();



void UART_Plc_Handle_Byte(uint8_t c);

void Plc_Task_Init();
#endif