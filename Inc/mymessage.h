#ifndef __MYMESSAGE_H_
#define __MYMESSAGE_H_

#define NET_MESSAGE_LEN 128


#define NULL_ID 0
#define HMI_ID  1
#define PLC_ID   2
#define UDP_ID  3
#define LOG_ID  4
#define HTTP_ID 5


typedef struct NetMail{
    uint8_t clientId;
    uint8_t serverId;
    uint8_t length;
    uint8_t  data[NET_MESSAGE_LEN];

} NetMail_t;


#endif
