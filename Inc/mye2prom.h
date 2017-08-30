#ifndef __MYE2PROM_H_
#define __MYE2PROM_H_


#include "stm32f1xx_hal.h"
#include "main.h"
#include "cmsis_os.h"
//#include "stm32f1xx_hal.h"
#include "myusart.h"

// set it to default state
//#define E2PROM_RESTORE_ENABLE

#define E2PROM_ADDR   0xA0
#define E2PROM_MAXPKT   32
#define E2PROM_WRITE_WAIT   10  //ms
#define E2PROM_READ_WAIT     10
#define E2PROM_TIMEOUT          5*E2PROM_WRITE_WAIT
#define E2PROM_SECTION_SIZE   32


#define DEFAULT_MARK1                0x33
#define DEFAULT_MARK2                0x33
#define DEFAULT_MARK3                0x33
#define DEFAULT_MARK4                0x33

#define DEFAULT_MODEL_NAME   "RuffNet-FX"
#define DEFAULT_VERSION           0x01
#define DEFAULT_YEAR                 17
#define DEFAULT_MONTH              8
#define DEFAULT_DAY                   15

#define DEFAULT_BAUDRATE        INDEX_BAUDRATE_9600
#define DEFAULT_WORDLEN         INDEX_UART_WORDLENGTH_8B
#define DEFAULT_PARITY             INDEX_UART_PARITY_EVEN
#define DEFAULT_STOPBITS          INDEX_UART_STOPBITS_1


#define DEFAULT_IP1                    192
#define DEFAULT_IP2                    168
#define DEFAULT_IP3                    1
#define DEFAULT_IP4                    150
#define DEFAULT_NETMASK1   255
#define DEFAULT_NETMASK2   255
#define DEFAULT_NETMASK3   255
#define DEFAULT_NETMASK4   255
#define DEFAULT_GW1  192
#define DEFAULT_GW2  168
#define DEFAULT_GW3  1
#define DEFAULT_GW4  1




/**
there are 32 sections
1 section has 8 pages
1 page has 32 bytes

0 section - 0 page will store MBR info

0 section  - 1 page will store system information , <=32 bytes

**/
#define SECTION_MBRINFO   0
#define SECTION_SYSINFO   0
#define SECTION_IPINFO     0

#define PAGE_MBRINFO        0
#define PAGE_SYSINFO        1
#define PAGE_IPINFO           2

typedef struct MBRInfo{
    uint8_t mark1; // Whether it's a valid info, 'Ruff' is correct, 0xFF is not;
    uint8_t mark2;
    uint8_t mark3;
    uint8_t mark4;
} MBRInfo_t;

typedef struct SysInfo {
    uint8_t modelName[16]; //
    uint8_t modelNameLen;
    uint8_t version;    
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t baudrate;
    uint8_t wordlength;
    uint8_t parity;
    uint8_t stopbits;

}SysInfo_t;

typedef struct IpInfo{
    uint8_t ip[4];
    uint8_t netmask[4];
    uint8_t gwip[4];    

}IpInfo_t;

typedef struct PlcInfo{
    uint8_t plcName[16]; //
    uint8_t plcNameLen;
    uint8_t version;    
    uint8_t baudrate;
    uint8_t wordlength;
    uint8_t parity;
    uint8_t stopbits;
}PlcInfo_t;

#define MBRINFO_SIZE                  sizeof(MBRInfo_t)
#define SYSINFO_SIZE                  sizeof(SysInfo_t)
#define IPINFO_SIZE                     sizeof(IpInfo_t)
#define PLCINFO_SIZE                   sizeof(PlcInfo_t)


void I2C1_Init(void);
void E2PROM_Init(void);
void E2PROM_Reset_Default(void);
SysInfo_t * getSysInfoP();
IpInfo_t * getIpInfoP();

#endif
