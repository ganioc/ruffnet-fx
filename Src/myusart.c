#include "myusart.h"
#include "myserial_log.h"
#include "myserial_hmi.h"
#include "myserial_plc.h"


uint8_t cUartLog, cUartPlc, cUartHmi, cUartRuff1, cUartRuff2;


const uint32_t BAUDRATE[]=
{
    300,
    600,
    1200,
    2400,
    4800,
    9600,
    19200,
    38400,
    57600,
    115200

};
const uint32_t WORDWIDTH[] =
{
    UART_WORDLENGTH_8B,
    UART_WORDLENGTH_9B
};
const char *strWORDWIDTH[] =
{
    "7 BIT",
    "8 BIT"
};
const uint32_t PARITY[] =
{
    UART_PARITY_NONE,
    UART_PARITY_EVEN,
    UART_PARITY_ODD
};
const char *strPARITY[] =
{
    "PARITY_NONE",
    "PARITY_EVEN",
    "PARITY_ODD"
};
const uint32_t STOPWIDTH[] =
{
    UART_STOPBITS_1,
    UART_STOPBITS_2
};
const char* strSTOPWIDTH[] =
{
    "STOPBITS_1",
    "STOPBITS_2"
};

void My_Usart_Init()
{




}


void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

    //GPIO_InitTypeDef GPIO_InitStruct;
    if(uartHandle->Instance==USART1)
    {
        Serial_Log_MspInit();
    }
    else if(uartHandle->Instance==USART2)
    {
        Serial_Plc_MspInit();
        /* USER CODE END USART2_MspInit 1 */
    }
    else if(uartHandle->Instance==USART3)
    {
        Serial_Hmi_MspInit();

    }
    else  if(uartHandle->Instance==UART4)
    {

    }
    else  if(uartHandle->Instance==UART5)
    {

    }
}
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
    if(uartHandle->Instance==USART1)
    {
        Serial_Log_MspDeInit();
    }
    else if(uartHandle->Instance==USART2)
    {
        Serial_Plc_MspDeInit();
    }
    else if(uartHandle->Instance==USART3)
    {
        Serial_Hmi_MspDeInit();
    }
    else if(uartHandle->Instance==UART4)
    {

    }
    else if(uartHandle->Instance==UART5)
    {

    }
}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    // uart_log
    if(UartHandle->Instance==USART1)
    {

    }
    // uart plc
    else if(UartHandle->Instance==USART2)
    {

    }
    // uart hmi
    else if(UartHandle->Instance==USART3)
    {

    }
    // uart 485
    else  if(UartHandle->Instance==UART4)
    {

    }
    // uart wireless
    else if(UartHandle->Instance==UART5)
    {

    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    

    // uart log
    if(UartHandle->Instance==USART1)
    {
        UART_Log_Handle_Byte(cUartLog);
        UART_Log_receive();
    }
    // uart plc
    else if(UartHandle->Instance==USART2)
    {
        UART_Plc_Handle_Byte(cUartPlc);
        UART_Plc_receive();
    }
    // uart hmi
    else if(UartHandle->Instance==USART3)
    {
        UART_Hmi_Handle_Byte(cUartHmi);
        UART_Hmi_receive();
        
    }
    // uart_ruff1
    else if(UartHandle->Instance==UART4)
    {
        //UART_Ruff1_Handle_Byte(cUartRuff1);
        //UART_Ruff1_receive();
    }
    // uart ruff2
    else if(UartHandle->Instance==UART5)
    {
        //UART_Ruff2_Handle_Byte(cUartRuff2);
        //UART_Ruff2_receive();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{

    // uart log
    if(UartHandle->Instance==USART1)
    {

    }
    // uart plc
    else if(UartHandle->Instance==USART2)
    {

    }
    // uart hmi
    else if(UartHandle->Instance==USART3)
    {
        //printf("uart3 error\n");
    }    // uart_ruff1
    else if(UartHandle->Instance==UART4)
    {

    }
    // uart ruff2
    else if(UartHandle->Instance==UART5)
    {

    }
}

//void Config_Usart(UART_HandleTypeDef *huart, uint32_t BaudRate, uint32_t WordLength, uint32_t StopBits, uint32_t Parity){










