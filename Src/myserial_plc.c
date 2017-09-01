#include "myserial_plc.h"
#include "mye2prom.h"
#include "cmsis_os.h"

#include "mymessage.h"
#include "myled.h"

UART_HandleTypeDef uartPlc;
extern uint8_t cUartPlc;

uint8_t signalPlcRx = False;

osThreadId plcTaskHandle;

extern const uint32_t BAUDRATE[];
extern const uint32_t WORDWIDTH[];

extern const uint32_t STOPWIDTH[];

extern const uint32_t PARITY[];

extern osMailQId  hmiMail;
extern osMailQId  udpMail;

static uint8_t statePlc;

static uint8_t PLC_RX_BUFFER[PLC_RX_BUFFER_LEN];
static uint8_t indexPlcRxBuffer;
static uint8_t lenPlcRxBuffer;

osMailQId  plcMailHiPrio;
osMailQId  plcMailLoPrio;

// which client is using the PLC
// 0 is free
static uint8_t currentClientId = NULL_ID;
static uint8_t bPlcInSession = False;

osTimerId timerTimeout;
uint32_t      delayTimeout = PLC_MESSAGE_TIMEOUT;


static void TimerHandler(void const *argument)
{
    (void) argument;

    bPlcInSession = False;
    printf("Plc visit timeout");

}

void StartTimer()
{
    printf("start timer %d ms\n",delayTimeout);

    osTimerStart(timerTimeout, delayTimeout);

}
void StopTimer()
{
    printf("timer stopped\n");
    osTimerStop(timerTimeout);
}

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
void CheckHiMailBox()
{
    NetMail_t   *pMail;
    osEvent  evt;

    uint8_t i;

    if(bPlcInSession == True)
    {
        printf("HiMail Plc in session\n");
        return;
    }

    evt = osMailGet(plcMailHiPrio, 10);

    if(evt.status == osEventMail)
    {
        pMail = (NetMail_t*)evt.value.p;
        printf("\nPlc RX from hmi:\n");

        printf("client: %d\n", pMail->clientId);
        printf("server: %d\n", pMail->serverId);

        printf("len: %d\n", pMail->length);

        for(i=0; i< pMail->length; i++)
        {
            printf("%02x ", pMail->data[i]);

        }
        printf("\n");

        currentClientId = pMail->clientId;

        bPlcInSession = True;

        HAL_UART_Transmit(&uartPlc, pMail->data, pMail->length, 100);

        StartTimer();

        printf("--> Send out to Real-Plc\n");

        osMailFree(plcMailHiPrio, pMail);


    }

}
void CheckLoMailBox()
{
    NetMail_t   *pMail;
    osEvent  evt;
    uint8_t i;

    if(bPlcInSession == True)
    {
        printf("LoMail Plc in session\n");
        return;
    }
// check Low priority mailbox
    evt = osMailGet(plcMailLoPrio, 10);

    if(evt.status == osEventMail)
    {
        pMail = (NetMail_t*)evt.value.p;
        printf("\nPlc RX from : %d\n", pMail->clientId);

        printf("len: %d\n", pMail->length);

        for(i=0; i< pMail->length; i++)
        {
            printf("%02x ", pMail->data[i]);

        }
        printf("\n");

        currentClientId = pMail->clientId;

        bPlcInSession = True;

        HAL_UART_Transmit(&uartPlc, pMail->data, pMail->length, 100);

        StartTimer();

        printf("--> Send out to Real-Plc\n");

        osMailFree(plcMailLoPrio, pMail);

    }


}

void checkPLCUart()
{
    NetMail_t   *pMail;

    uint8_t i;

    if(signalPlcRx == False)
    {
        return;

    }
    printf("\n<-- Plc RX Uart from Real_Plc:%d:\n", lenPlcRxBuffer);

    if(bPlcInSession == False)
    {
        // Won't process it
        printf("%d bytes got from PLC, no client\n", lenPlcRxBuffer);
        return;
    }

    if(currentClientId == HMI_ID)
    {
        pMail = (NetMail_t*)osMailAlloc(hmiMail, 100);

        if(pMail != NULL)
        {
            pMail->clientId = PLC_ID;
            pMail->serverId = currentClientId;
            pMail->length = lenPlcRxBuffer;

            for(i=0; i< lenPlcRxBuffer; i++)
            {
                printf("%02x ", PLC_RX_BUFFER[i]);
                pMail->data[i] = PLC_RX_BUFFER[i];
            }
            printf("\n");

            osMailPut(hmiMail, pMail);
            printf("send it to Hmi\n");

        }
    }
    else if(currentClientId == UDP_ID)
    {
        pMail = (NetMail_t*)osMailAlloc(udpMail, 100);

        if(pMail != NULL)
        {
            pMail->clientId = PLC_ID;
            pMail->serverId = currentClientId;
            pMail->length = lenPlcRxBuffer;

            for(i=0; i< lenPlcRxBuffer; i++)
            {
                printf("%02x ", PLC_RX_BUFFER[i]);
                pMail->data[i] = PLC_RX_BUFFER[i];
            }
            printf("\n");

            osMailPut(udpMail, pMail);
            printf("send it to Udp\n");

        }
    }
    else
    {
        printf("Unrecognized client id: %d", currentClientId);
    }

    bPlcInSession = False;
    StopTimer();

    printf("Plc out of session\n");

    signalPlcRx = False;

}
void StartPlcTask(void const * argument)
{
    statePlc = STATE_PLC_UNLOCK;

    printf("Task Plc started\n");

    UART_Plc_receive();

    for(;;)
    {

        CheckHiMailBox();

        CheckLoMailBox();

        checkPLCUart();

        LED4_Toggle();

        osDelay(10);
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
    osMailQDef(plcmailhi, 2, NetMail_t);
    plcMailHiPrio = osMailCreate(osMailQ(plcmailhi), NULL);

    if(plcMailHiPrio != NULL)
    {
        printf("plc mail hi created\n");
    }

    osMailQDef(plcmaillo, 4, NetMail_t);
    plcMailLoPrio = osMailCreate(osMailQ(plcmaillo), NULL);

    if(plcMailLoPrio != NULL)
    {
        printf("plc mail lo created\n");
    }

    osThreadDef(plcTask, StartPlcTask, osPriorityHigh, 0, 128);
    plcTaskHandle = osThreadCreate(osThread(plcTask), NULL);

    if(plcTaskHandle == NULL)
    {

        printf("plc task create fail\n");
    }
    //printf("Plc task started\n");

    osTimerDef(timeoutTimer, TimerHandler);
    timerTimeout= osTimerCreate(osTimer(timeoutTimer), osTimerOnce, NULL);

    if(timerTimeout == NULL)
    {
        printf("plc timerTimeout fail\n");
    }
    else
    {
        printf("plc timerTimeout created\n");
    }

}





