#include "myserial_hmi.h"
#include "mye2prom.h"

#include "myserial_hmi.h"
#include "myled.h"


UART_HandleTypeDef uartHmi;
extern uint8_t cUartHmi;


osThreadId hmiTaskHandle;

extern const uint32_t BAUDRATE[];
extern const uint32_t WORDWIDTH[];

extern const uint32_t STOPWIDTH[];

extern const uint32_t PARITY[];

static uint8_t stateHmi;




void Serial_Hmi_Init()
{

    SysInfo_t * pSysInfo =  getSysInfoP();



    printf("Serial Hmi init\n");
    printf("buadrate:%d , index:%d\n", BAUDRATE[pSysInfo->baudrate], pSysInfo->baudrate);



    uartHmi.Instance = USART3;
    uartHmi.Init.BaudRate = 9600;//BAUDRATE[pSysInfo->baudrate];
    uartHmi.Init.WordLength = WORDWIDTH[pSysInfo->wordlength];
    uartHmi.Init.StopBits = STOPWIDTH[pSysInfo->stopbits];
    uartHmi.Init.Parity = PARITY[pSysInfo->parity];
    uartHmi.Init.Mode = UART_MODE_TX_RX;
    uartHmi.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartHmi.Init.OverSampling = UART_OVERSAMPLING_16;

    if(HAL_UART_Init(&uartHmi) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}


void Serial_Hmi_MspInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* USER CODE BEGIN USART3_MspInit 0 */

    /* USER CODE END USART3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE BEGIN USART2_MspInit 1 */
    HAL_NVIC_SetPriority(USART3_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    /* USER CODE END USART1_MspInit 1 */

}

void Serial_Hmi_MspDeInit()
{
    /* USER CODE BEGIN USART2_MspDeInit 0 */

    /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA10     ------> USART3_TX
    PA11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USER CODE BEGIN USART1_MspDeInit 1 */

    /* USER CODE END USART1_MspDeInit 1 */

}

// uart log
void  UART_Hmi_receive()
{
    uint8_t ret;

    do
    {
        ret = HAL_UART_Receive_IT(&uartHmi, (uint8_t *)&cUartHmi, 1);

    }
    while(ret != HAL_OK);

}
void StartHmiTask(void const * argument)
{
    uint8_t ch = 0x02;

    stateHmi = STATE_HMI_UNLOCK;

    UART_Hmi_receive();

    for(;;)
    {
        //printf("Hello\n");
        LED2_Toggle();
        //HAL_UART_Transmit(&uartHmi, (uint8_t *)&ch, 1, 0xFFFF);
        osDelay(200);
    }

    /* USER CODE END StartDefaultTask */
}

void UART_Hmi_Handle_Byte(uint8_t c)
{
    switch(stateHmi)
    {
        default:
            break;

    }
}
void Hmi_Task_Init()
{
    osThreadDef(hmiTask, StartHmiTask, osPriorityHigh, 0, 128);
    hmiTaskHandle = osThreadCreate(osThread(hmiTask), NULL);
    printf("Hmi task started\n");
}


