/**
 ******************************************************************************
  * File Name          : LWIP.c
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"

/* USER CODE BEGIN 0 */
#include "app_ethernet.h"
#include "myhttpd.h"
#include "mytcp.h"
#include "myudp.h"

/* USER CODE END 0 */

/* ETH Variables initialization ----------------------------------------------*/
void _Error_Handler(char * file, int line);

/* USER CODE BEGIN 1 */
static void Netif_Config(void);
/* USER CODE END 1 */

/* Variables Initialization */
struct netif gnetif;  // network interface, added by Yang
ip_addr_t ipaddr;
ip_addr_t netmask;
ip_addr_t gw;

/* Semaphore to signal Ethernet Link state update */
osSemaphoreId Netif_IRQSemaphore = NULL;
struct enc_irq_str irq_arg;


/* USER CODE BEGIN 2 */
void BSP_Config()
{
    // set interrupt pin for enc28j60


}
/* USER CODE END 2 */

/* init function */
void MX_LWIP_Init(void)
{

    printf("MX_Lwip_init()\n");
    //printf "hello\n"


    printf("BSP_config\n");

    tcpip_init(NULL, NULL);
    printf("tcpip_init()\n");

    printf("netif_config()\n");
    /* Initilaize the LwIP stack */
    Netif_Config();

    udp_server_init();
    
    // add tcp server here
    tcp_server_init();

    // add http server here
    http_server_netconn_init();


    /* Notify user about the netwoek interface config */
    User_notification(&gnetif);

    /* USER CODE BEGIN 3 */
#ifdef USE_DHCP
    /* Start DHCPClient */
    osThreadDef(DHCP, DHCP_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);

    osThreadCreate(osThread(DHCP), &gnetif);
#endif


    /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;

    /* IP address setting */
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2,
             NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    /* create a binary semaphore used for informing ethernetif of frame
    reception */
    osSemaphoreDef(Netif_SEM);
    Netif_IRQSemaphore = osSemaphoreCreate(osSemaphore(Netif_SEM), 1);

    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
    struct ip_addr *netmask, struct ip_addr *gw,
    void *state, err_t (* init)(struct netif *netif),
    err_t (* input)(struct pbuf *p, struct netif *netif))

    Adds your network interface to the netif_list. Allocate a struct
    netif and pass a pointer to this structure as the first argument.
    Give pointers to cleared ip_addr structures when using DHCP,
    or fill them with sane numbers otherwise. The state pointer may be NULL.

    The init function pointer must point to a initialization function for
    your ethernet netif interface. The following code illustrates it's use.*/

    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /*  Registers the default network interface. */
    netif_set_default(&gnetif);

    if(netif_is_link_up(&gnetif))
    {
        /* When the netif is fully configured this function must be called.*/
        netif_set_up(&gnetif);
    }
    else
    {
        /* When the netif link is down this function must be called */
        netif_set_down(&gnetif);
    }

    /* Set the link callback function, this function is called on change of
    link status*/
    netif_set_link_callback(&gnetif, ethernetif_update_config);

    irq_arg.netif = &gnetif;
    irq_arg.semaphore = Netif_IRQSemaphore;

    /* Create the Ethernet IRQ handler thread */
    osThreadDef(IrqThr, ethernetif_process_irq, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);

    osThreadCreate(osThread(IrqThr), &irq_arg);
}






/* USER CODE END 4 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
