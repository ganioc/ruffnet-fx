#include "myserial_log.h"
#include "myusart.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "cmsis_os.h"

#include "main.h"
#include "myled.h"
//#include "mail_queue.h"

UART_HandleTypeDef uartLog;
extern uint8_t cUartLog;

osThreadId logTaskHandle;

uint8_t indexLog = 0;
char UART_LOG_RX_BUFFER[RX_BUFFER_SIZE];
char FREERTOS_LOG_RX_BUFFER[RX_BUFFER_SIZE];
uint8_t lenLogRx=0;

uint8_t signalLogRx = False;

static  uint8_t state = MAIN_STATE_LOG;

void console_parse(char *cmd, uint8_t len);


// printf initialization
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of
    transmission */
    HAL_UART_Transmit(&uartLog, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}


void Serial_Log_Init()
{

    uartLog.Instance = USART1;
    uartLog.Init.BaudRate = 115200;
    uartLog.Init.WordLength = UART_WORDLENGTH_8B;
    uartLog.Init.StopBits = UART_STOPBITS_1;
    uartLog.Init.Parity = UART_PARITY_NONE;
    uartLog.Init.Mode = UART_MODE_TX_RX;
    uartLog.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartLog.Init.OverSampling = UART_OVERSAMPLING_16;
    if(HAL_UART_Init(&uartLog) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
}

void Serial_Log_MspInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* USER CODE BEGIN USART1_MspInit 0 */

    /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USER CODE BEGIN USART1_MspInit 1 */
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    /* USER CODE END USART1_MspInit 1 */

}
void Serial_Log_MspDeInit()
{
    /* USER CODE BEGIN USART1_MspDeInit 0 */

    /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USER CODE BEGIN USART1_MspDeInit 1 */

    /* USER CODE END USART1_MspDeInit 1 */

}

void StartLogTask(void const * argument)
{

    printf("task Log started\n");
        
    UART_Log_receive();

    for(;;)
    {
        //printf("Hello\n");
        //LED2_Toggle();

        if(signalLogRx == True)
        {
            printf("\n%d:%s\n", strlen(FREERTOS_LOG_RX_BUFFER),FREERTOS_LOG_RX_BUFFER);

            //console_parse(FREERTOS_LOG_RX_BUFFER, strlen(FREERTOS_LOG_RX_BUFFER));
            //printf("Rx\n");
            
            signalLogRx = False;
        }

        osDelay(10);
    }

    /* USER CODE END StartDefaultTask */
}

void Log_Task_Init()
{
    osThreadDef(logTask, StartLogTask, osPriorityNormal, 0, 128);
    logTaskHandle = osThreadCreate(osThread(logTask), NULL);

}

void UART_Log_Handle_Byte(uint8_t c)
{
    uint8_t i;

    if((c == '\n'|| c== '\r') && indexLog < 1)
    {
        indexLog = 0;
    }
    else if(c == '\n' || c == '\r')
    {
        if(UART_LOG_RX_BUFFER[ indexLog -1] == '\r' ||
           UART_LOG_RX_BUFFER[ indexLog -1] == '\n')
        {
            indexLog --;
        }

        UART_LOG_RX_BUFFER[ indexLog] = '\0';

        // send out the msg from RX Buffer
        for(i = 0; i <= indexLog; i++)
        {
            FREERTOS_LOG_RX_BUFFER[i] = UART_LOG_RX_BUFFER[i];
        }

        signalLogRx = True;

        indexLog = 0;

    }
    else if(indexLog == RX_BUFFER_SIZE-2)
    {
        UART_LOG_RX_BUFFER[indexLog ++] = c;

        UART_LOG_RX_BUFFER[ indexLog] = '\0';

        // send out the msg from RX Buffer
        for(i = 0; i <= indexLog; i++)
        {
            FREERTOS_LOG_RX_BUFFER[i] = UART_LOG_RX_BUFFER[i];
        }

        signalLogRx = True;

        indexLog = 0;
    }
    else if(c >=20 && c <= 126)
    {
        UART_LOG_RX_BUFFER[indexLog ++] = c;
    }
    else
    {
        ; // Do nothing
    }
}


// uart log
void  UART_Log_receive()
{

    uint8_t ret;

    do
    {
        ret = HAL_UART_Receive_IT(&uartLog, (uint8_t *)&cUartLog, 1);

    }
    while(ret != HAL_OK);

}

void print_help()
{
    printf("\n---------------------------------------\n");
    printf("%8s: Bring help menu.\n", "help");
    printf("%8s: Other explanations.\n", "other");
    printf("%8s: Data To PLC\n", "toplc");
    printf("%8s: Data from PLC\n", "fromplc");
    //printf("%*s %s", 8, " ", "command");
    printf("---------------------------------------\n");

}
void main_state_parse(char * cmd, uint8_t len)
{
    if(strcmp(cmd, "help") == 0)
    {

        print_help();
    }
    else if(strcmp(cmd, "toplc") ==0)
    {
        state = COMM_TO_PLC_STATE_LOG;

    }
    else
    {
        printf("$:Unrecognized command\n");
        //print_help();
    }

}
void comm_to_plc_state_parse(char * cmd, uint8_t len)
{
    // if it's data just send it to PLC

    // if it's command , parse it
    if(strcmp(cmd, "q") == 0)
    {
        state = MAIN_STATE_LOG;
    }
}
void console_parse(char * cmd, uint8_t len)
{

    if(state == MAIN_STATE_LOG)
    {

        main_state_parse(cmd, len);
    }
    else if(state == COMM_TO_PLC_STATE_LOG)
    {
        comm_to_plc_state_parse(cmd,len);

    }
}

