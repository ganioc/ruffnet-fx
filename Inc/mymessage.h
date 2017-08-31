#ifndef __MYMESSAGE_H_
#define __MYMESSAGE_H_

#define NET_MESSAGE_LEN 128

#define TYPE_MESSAGE_HMI_2_PLC   1
#define TYPE_MESSAGE_PLC_2_HMI  2
#define TYPE_MESSAGE_ETH_2_PLC  3
#define TYPE_MESSAGE_PLC_2_ETH  4
#define TYPE_MESSAGE_LOG_2_PLC  5
#define TYPE_MESSAGE_PLC_2_LOG  6

typedef struct NetMessage{
    uint8_t type;
    uint8_t length;
    uint32_t data;
    uint32_t block;

} NetMessage_t;

typedef struct NetMail{
    uint8_t type;
    uint8_t length;
    uint8_t  data[NET_MESSAGE_LEN];
    //uint32_t block;

} NetMail_t;
#endif
