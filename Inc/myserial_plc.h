#ifndef __MYSERIAL_PLC_H_
#define __MYSERIAL_PLC_H_


#include "stm32f1xx_hal.h"
#include "main.h"
#include "cmsis_os.h"


void Serial_Plc_Init();
void Serial_Plc_MspInit();
void Serial_Plc_MspDeInit();
void  UART_Plc_receive();



#endif