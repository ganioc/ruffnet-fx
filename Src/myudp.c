#include "myudp.h"
#include "main.h"
#include "lwip/debug.h"
#include "lwip/stats.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#include "cmsis_os.h"

#include "mymessage.h"

extern osMailQId  plcMailLoPrio;

struct udp_pcb *upcb;


osThreadId udpTaskHandle;
osMailQId  udpMail;

static uint8_t clientIp[4];
static uint16_t clientPort;


void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, ip_addr_t *addr, u16_t port);

// send message to udp client
void sendToUdpClient(uint8_t *data, uint8_t len)
{
    ip_addr_t DestIPaddr;
    err_t err;
    struct pbuf *p;

    printf("udp send out\n");

    upcb = udp_new();

    if(upcb!=NULL)
    {
        /*assign destination IP address */
        IP4_ADDR(&DestIPaddr, clientIp[0], clientIp[1], clientIp[2],clientIp[3]);

        printf("udp client:%d.%d.%d.%d :%d\n", clientIp[0], clientIp[1],clientIp[2],clientIp[3], clientPort);

        /* configure destination IP address and port */
        err= udp_connect(upcb, &DestIPaddr, clientPort);

        if(err != ERR_OK){
            printf("udp connect failure");
        }


        /* allocate pbuf from pool*/
        p = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_POOL);

        if(p != NULL)
        {
            /* copy data to pbuf */
            pbuf_take(p, (char*)data, len);

            /* send udp data */
            udp_send(upcb, p);

            /* free pbuf */
            pbuf_free(p);
        }

        udp_disconnect(upcb);
        udp_remove(upcb);

    }
    else{
        printf("Create udp new fail\n");
    }


}
void StartUdpTask(void const * argument)
{
    NetMail_t * pMail;
    osEvent  evt;
    osStatus status;
    uint8_t i;

    for(;;)
    {

        evt = osMailGet(udpMail, 100);

        if(evt.status == osEventMail)
        {
            pMail = (NetMail_t*)evt.value.p;
            printf("\n--> udp receive from plc:\n");

            printf("len: %d\n", pMail->length);

            for(i=0; i< pMail->length; i++)
            {
                printf("%02x ", pMail->data[i]);

            }
            printf("\n");


            printf("--> Send out to Real-Udp client\n");

            sendToUdpClient(pMail->data, pMail->length);

            osMailFree(udpMail, pMail);

        }


        osDelay(10);
    }

}

void udp_server_init(void)
{

    struct udp_pcb *upcb;
    err_t err;

    printf("udp server init() port:%d\n", UDP_SERVER_PORT);


    /* Create a new UDP control block  */
    upcb = udp_new();

    if(upcb)
    {
        /* Bind the upcb to the UDP_PORT port */
        /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
        err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);

        if(err == ERR_OK)
        {
            /* Set a receive callback for the upcb */
            udp_recv(upcb, udp_server_receive_callback, NULL);
        }
        else
        {
            printf("udp bind fail\n");
        }
    }

    osThreadDef(udpTask, StartUdpTask, osPriorityNormal, 0, 128);
    udpTaskHandle = osThreadCreate(osThread(udpTask), NULL);

    if(udpTaskHandle == NULL)
    {

        printf("udp Task create fail\n");

    }
    else
    {
        printf("udp task created\n");
    }

    osMailQDef(udpmail, 2, NetMail_t);
    udpMail= osMailCreate(osMailQ(udpmail), NULL);

    if(udpMail!= NULL)
    {
        printf("udp mail hi created\n");
    }

}


/**
  * @brief This function is called when an UDP datagrm has been received on
the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, ip_addr_t *addr, u16_t port)
{
    uint8_t i;
    u32_t IPaddress;
    uint8_t *ptr;
    NetMail_t * pMail;
    osEvent  evt;
    osStatus status;

    printf("\n<-- udp rx len:%d\n", p->len);


    clientPort = port;
    IPaddress = addr->addr;
    ptr = (uint8_t*)(p->payload);

    clientIp[0]  = (uint8_t)(IPaddress);
    clientIp[1] = (uint8_t)(IPaddress >> 8);
    clientIp[2] = (uint8_t)(IPaddress >> 16);
    clientIp[3] = (uint8_t)(IPaddress >> 24);

    printf("udp client:%d.%d.%d.%d :%d\n",clientIp[0],clientIp[1],clientIp[2],clientIp[3],clientPort);

    pMail = (NetMail_t*)osMailAlloc(plcMailLoPrio, 100);

    if(pMail != NULL)
    {

        pMail->clientId = UDP_ID;
        pMail->serverId = PLC_ID;
        pMail->length = p->len;

        for(i=0; i< p->len; i++)
        {
            printf("%02x ", *(ptr + i));
            pMail->data[i] = *(ptr + i);
        }
        printf("\n");

        osMailPut(plcMailLoPrio, pMail);
        printf("send it to Plc lo\n");

    }

    /* Free the p buffer */
    pbuf_free(p);

}




