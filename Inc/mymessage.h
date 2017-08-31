#ifndef __MYMESSAGE_H_
#define __MYMESSAGE_H_

#define NET_MESSAGE_LEN 128

#define TYPE_MESSAGE_HMI_2_PLC   1
#define TYPE_MESSAGE_PLC_2_HMI  2
#define TYPE_MESSAGE_ETH_2_PLC  3
#define TYPE_MESSAGE_PLC_2_ETH  4
#define TYPE_MESSAGE_LOG_2_PLC  5
#define TYPE_MESSAGE_PLC_2_LOG  6

#define NULL_ID 0
#define HMI_ID  1
#define PLC_ID   2
#define UDP_ID  3
#define LOG_ID  4
#define HTTP_ID 5

/*
typedef struct NetMessage{
    uint8_t type;
    uint8_t length;
    uint32_t data;
    uint32_t block;

} NetMessage_t;
*/

typedef struct NetMail{
    uint8_t clientId;
    uint8_t serverId;
    uint8_t length;
    uint8_t  data[NET_MESSAGE_LEN];

} NetMail_t;


#endif
