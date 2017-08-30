#include "myserial_plc.h"
#include "mye2prom.h"
#include "cmsis_os.h"

#include "mymessage.h"

UART_HandleTypeDef uartPlc;
extern uint8_t cUartPlc;

uint8_t signalPlcRx = False;

osThreadId plcTaskHandle;

extern const uint32_t BAUDRATE[];
extern const uint32_t WORDWIDTH[];

extern const uint32_t STOPWIDTH[];

extern const uint32_t PARITY[];

static uint8_t statePlc;

static uint8_t PLC_RX_BUFFER[PLC_RX_BUFFER_LEN];
static uint8_t indexPlcRxBuffer;
static uint8_t lenPlcRxBuffer;


// pool

osPoolId  plcPoolHiPrio;

//osPoolDef(plcPoolLoPrio, 3, NetMessage_t);
//osPoolId  plcPoolLoPrio;

//queue
osMessageQId plcQueueHiPrio;
osMessageQId plcQueueLoPrio;



void Serial_Plc_Init()
{

    SysInfo_t * pSysInfo =  getSysInfoP();



    printf("Serial Plc init\n");
    printf("buadrate:%d , index:%d\n", BAUDRATE[pSysInfo->baudrate], pSysInfo->baudrate);



    uartPlc.Instance = USART2;
    uartPlc.Init.BaudRate = BAUDRATE[pSysInfo->baudrate];
    uartPlc.Init.WordLength = WORDWIDTH[pSysInfo->wordlength];
    uartPlc.Init.StopBits = STOPWIDTH[pSysInfo->stopbits];
    uartPlc.Init.Parity = PARITY[pSysInfo->parity];
    uartPlc.Init.Mode = UART_MODE_TX_RX;
    uartPlc.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartPlc.Init.OverSampling = UART_OVERSAMPLING_16;

    if(HAL_UART_Init(&uartPlc) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}

void Serial_Plc_MspInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* USER CODE BEGIN USART2_MspInit 0 */

    /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USER CODE BEGIN USART2_MspInit 1 */
    HAL_NVIC_SetPriority(USART2_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    /* USER CODE END USART1_MspInit 1 */

}

void Serial_Plc_MspDeInit()
{
    /* USER CODE BEGIN USART2_MspDeInit 0 */

    /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USER CODE BEGIN USART1_MspDeInit 1 */

    /* USER CODE END USART1_MspDeInit 1 */

}

// uart log
void  UART_Plc_receive()
{
    uint8_t ret;

    do
    {
        ret = HAL_UART_Receive_IT(&uartPlc, (uint8_t *)&cUartPlc, 1);

    }
    while(ret != HAL_OK);

}


void StartPlcTask(void const * argument)
{
    uint8_t i;
    NetMessage_t*rptr;
    osEvent  evt;

    statePlc = STATE_PLC_UNLOCK;
    indexPlcRxBuffer = 0;

    UART_Plc_receive();

    for(;;)
    {

        evt = osMessageGet(plcQueueHiPrio, 10);  // wait for message
        
        if(evt.status == osEventMessage)
        {
            rptr =(NetMessage_t*) evt.value.p;
            printf("\ntype: %d V\n", rptr->type);
            printf("len: %d A\n", rptr->length);
            //printf("Number of cycles: %d\n", rptr->counter);
            osPoolFree(plcPoolHiPrio, rptr);                  // free memory allocated 
            // for message
        }

        if(signalPlcRx == True)
        {
            printf("\nPlc RX:%d:\n", lenPlcRxBuffer);

            for(i=0; i< lenPlcRxBuffer; i++)
            {
                printf("%02x ", PLC_RX_BUFFER[i]);
            }
            printf("\n");

            // send out the received cmd to PLC

            signalPlcRx = False;
        }
        //printf("Hello\n");
        LED4_Toggle();
        //HAL_UART_Transmit(&uartHmi, (uint8_t *)&ch, 1, 0xFFFF);
        osDelay(20);
    }

    /* USER CODE END StartDefaultTask */
}


void UART_Plc_Handle_Byte(uint8_t c)
{
    switch(statePlc)
    {
        case STATE_PLC_UNLOCK:
            if(c == HEAD_PLC)
            {
                statePlc = STATE_PLC_BODY;
                PLC_RX_BUFFER[indexPlcRxBuffer++] = c;

            }
            break;
        case STATE_PLC_BODY:
            if(c == END_PLC)
            {
                statePlc = STATE_PLC_CRCH;
            }
            PLC_RX_BUFFER[indexPlcRxBuffer++] = c;
            break;
        case STATE_PLC_CRCH:
            statePlc = STATE_PLC_CRCL;
            PLC_RX_BUFFER[indexPlcRxBuffer++] = c;
            break;
        case STATE_PLC_CRCL:
            PLC_RX_BUFFER[indexPlcRxBuffer++] = c;
            statePlc = STATE_PLC_UNLOCK;
            lenPlcRxBuffer = indexPlcRxBuffer;
            signalPlcRx = True;
            indexPlcRxBuffer = 0;
            break;

        default:

            break;

    }
}
void Plc_Task_Init()
{
    osPoolDef(plcpool, 3, NetMessage_t);
    plcPoolHiPrio = osPoolCreate(osPool(plcpool));
    
    osMessageQDef(plcqueueHi, 1, NetMessage_t);
    plcQueueHiPrio= osMessageCreate(osMessageQ(plcqueueHi), NULL);

    
    osMessageQDef(plcqueueLo, 3, NetMessage_t);
    plcQueueLoPrio = osMessageCreate(osMessageQ(plcqueueLo), NULL);


    osThreadDef(plcTask, StartPlcTask, osPriorityHigh, 0, 128);
    plcTaskHandle = osThreadCreate(osThread(plcTask), NULL);
    printf("Plc task started\n");
}





