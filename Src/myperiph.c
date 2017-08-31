#include "gpio.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "spi.h"
#include "myusart.h"
#include "myled.h"
#include "myspi.h"
#include "mylcd.h"
#include "mye2prom.h"
#include "myserial_log.h"
#include "myserial_hmi.h"
#include "myserial_plc.h"


static const char PRODUCT_NAME[]= "RuffNet-FX";

void printLogo()
{

    printf("\n|==================================================\n|\n");
    printf("|             _____        __  __ _       \n");
    printf("|            |  __ \\      / _|/ _(_)      \n");
    printf("|            | |__) |   _| |_| |_ _  ___  \n");
    printf("|            |  _  / | | |  _|  _| |/ _ \\ \n");
    printf("|            | | \\ \\ |_| | | | |_| | (_) |\n");
    printf("|            |_|  \\_\\__,_|_| |_(_)_|\\___/ \n");
    printf("|\n|\n| %s\n", PRODUCT_NAME);
    printf("| 2017.7\n");
    printf("==================================================\n\n");
}

void Periph_Init()
{

    MX_GPIO_Init();

    LED2_Off();
    LED3_Off();
    LED4_Off();


    /*
      MX_UART4_Init();
      MX_USART1_UART_Init();
      MX_USART2_UART_Init();
      MX_USART3_UART_Init();

      */

    Serial_Log_Init();//uart1
    printLogo();
        
    I2C1_Init();
    E2PROM_Init();
    
    Serial_Hmi_Init();//uart3
    Serial_Plc_Init(); //uart2 

    LCD_Init();

}
