#include "mye2prom.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "stm32f1xx_hal_i2c.h"
#include "string.h"

/* Private variables
---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
uint8_t   mbrInfo[MBRINFO_SIZE];
uint8_t    sysInfo[SYSINFO_SIZE];
uint8_t    ipInfo[IPINFO_SIZE];
//uint8_t    plcInfo[PLCINFO_SIZE];

extern const uint32_t BAUDRATE[];
extern const char* strWORDWIDTH[];
extern const char * strPARITY[];
extern const char* strSTOPWIDTH[];




void I2C1_Init(void)
{
    __I2C1_CLK_ENABLE();
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    
    if(HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    else{
        printf("HAL I2C OK\n");
    }

}
//void E2PROM_Init(void){
//    printf("I2C1 init for E2PROM\n");
//    I2C1_Init();
//
//}

/* USER CODE BEGIN 1 */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{

  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }

}



void E2PROM_Read(uint8_t section, uint8_t page, uint8_t * data, uint8_t len){
    HAL_StatusTypeDef fb;
    uint16_t memaddr;

    memaddr = 32*(8*section + page);
    
    do{
        printf("E2PROM read\n");
        fb = HAL_I2C_Mem_Read(&hi2c1, E2PROM_ADDR, memaddr, I2C_MEMADD_SIZE_16BIT, data, 
        len, E2PROM_READ_WAIT);

    }while(fb != HAL_OK);

}
void E2PROM_Write(uint8_t section, uint8_t page, uint8_t * data, uint8_t len){
    HAL_StatusTypeDef fb;
    uint16_t memaddr;

    memaddr = 32*(8*section + page);
    
    do{
        printf("E2PROM write\n");
        fb = HAL_I2C_Mem_Write(&hi2c1, E2PROM_ADDR, memaddr, I2C_MEMADD_SIZE_16BIT, data, 
        len, E2PROM_WRITE_WAIT);

    }while(fb != HAL_OK);

}
void Get_MBR(uint8_t * data, uint8_t len){
       E2PROM_Read(SECTION_MBRINFO, PAGE_MBRINFO,  data, len);

}
void Set_MBR(uint8_t * data, uint8_t len){
    E2PROM_Write(SECTION_MBRINFO, PAGE_MBRINFO,  data, len);
}
void Get_SysInfo(uint8_t * data, uint8_t len){
       E2PROM_Read(SECTION_SYSINFO, PAGE_SYSINFO,  data, len);
}
void Set_SysInfo(uint8_t * data, uint8_t len){
    E2PROM_Write(SECTION_SYSINFO, PAGE_SYSINFO,  data, len);
}
void Get_IpInfo(uint8_t * data, uint8_t len){
       E2PROM_Read(SECTION_IPINFO, PAGE_IPINFO,  data, len);
}
void Set_IpInfo(uint8_t * data, uint8_t len){
    E2PROM_Write(SECTION_IPINFO, PAGE_IPINFO,  data, len);
}

void Print_SysInfo(SysInfo_t * p){
    printf("\n-----------Sys Info-------------\n");
    printf("Model name: %s\n", p->modelName);
    printf("baudrate is %d\n", BAUDRATE[p->baudrate]);
    printf("wordlength is %s\n", strWORDWIDTH[p->wordlength]); 
    printf("parity is %s\n", strPARITY[p->parity]);
    printf("stopbits is %s\n", strSTOPWIDTH[p->stopbits]);
    printf("--------------------------------\n");

}
void Print_IpInfo(IpInfo_t *p){
    printf("\n-----------Static IP-------------\n");
    printf("IP: %d.%d.%d.%d\n", p->ip[0],p->ip[1], p->ip[2],p->ip[3]);
    printf("netmask: %d.%d.%d.%d\n", p->netmask[0],p->netmask[1], 
    p->netmask[2],p->netmask[3]);
    printf("gwIP: %d.%d.%d.%d\n", p->gwip[0],p->gwip[1], p->gwip[2],p->gwip[3]);
    printf("--------------------------------\n");    
}

SysInfo_t * getSysInfoP(){
    SysInfo_t * pSysInfo;
        pSysInfo = (SysInfo_t *) & sysInfo;
    return pSysInfo;
}
IpInfo_t * getIpInfoP(){
    IpInfo_t * pIpInfo;
        pIpInfo = (IpInfo_t *)&ipInfo;
        return pIpInfo;
}
void E2PROM_Reset_Default(void){
    MBRInfo_t *pMBRInfo;
    SysInfo_t * pSysInfo;
    IpInfo_t * pIpInfo;
   // PlcInfo_t* pPlcInfo;

    pMBRInfo = (MBRInfo_t *)&mbrInfo;
    pSysInfo = (SysInfo_t *) & sysInfo;
    pIpInfo = (IpInfo_t *)&ipInfo;
    //pPlcInfo = (PlcInfo_t *)&plcInfo;

    pMBRInfo->mark1= (uint8_t)DEFAULT_MARK1;
    pMBRInfo->mark2= (uint8_t)DEFAULT_MARK2;
    pMBRInfo->mark3= (uint8_t)DEFAULT_MARK3;
    pMBRInfo->mark4= (uint8_t)DEFAULT_MARK4;
    
    
    sprintf((char*)pSysInfo->modelName, "%s",(char*)DEFAULT_MODEL_NAME );
    pSysInfo->modelNameLen = sizeof(DEFAULT_MODEL_NAME);
    pSysInfo->version = DEFAULT_VERSION;    
    pSysInfo->year= DEFAULT_YEAR;
    pSysInfo->month= DEFAULT_MONTH;
    pSysInfo->day= DEFAULT_DAY;
    pSysInfo->baudrate= DEFAULT_BAUDRATE;
    pSysInfo->wordlength= DEFAULT_WORDLEN;
    pSysInfo->parity= DEFAULT_PARITY;
    pSysInfo->stopbits= DEFAULT_STOPBITS;

    pIpInfo->ip[0]= DEFAULT_IP1;
    pIpInfo->ip[1]= DEFAULT_IP2;
    pIpInfo->ip[2]= DEFAULT_IP3;
    pIpInfo->ip[3]= DEFAULT_IP4;

    pIpInfo->netmask[0]= DEFAULT_NETMASK1;
    pIpInfo->netmask[1]= DEFAULT_NETMASK2;
    pIpInfo->netmask[2]= DEFAULT_NETMASK3;
    pIpInfo->netmask[3]= DEFAULT_NETMASK4;

    pIpInfo->gwip[0]= DEFAULT_GW1;
    pIpInfo->gwip[1]= DEFAULT_GW2;
    pIpInfo->gwip[2]= DEFAULT_GW3;
    pIpInfo->gwip[3]= DEFAULT_GW4;

//    sprintf((char*)pPlcInfo->plcName, "%s", "PLC name" );
//    pPlcInfo->plcNameLen = strlen(pPlcInfo->plcName);
//    pPlcInfo->baudrate= 


    printf("Set E2prom values to default\n");

    Set_MBR((uint8_t *)&mbrInfo, (uint8_t)MBRINFO_SIZE);
    Set_SysInfo((uint8_t *)&sysInfo, (uint8_t)SYSINFO_SIZE);
    Set_IpInfo((uint8_t*)&ipInfo, (uint8_t)IPINFO_SIZE);

}
void E2PROM_Init(void){
    HAL_StatusTypeDef fb;
    uint8_t data[5],i;
    MBRInfo_t *pMBRInfo;
    SysInfo_t  *pSysInfo;
    IpInfo_t  *pIpInfo;
    data[0] = 0x01;

    uint8_t dataIn = 0x44;
    
    printf("Init E2Prom storage\n");

    Get_MBR((uint8_t *)&mbrInfo, MBRINFO_SIZE);
    Get_SysInfo((uint8_t *)&sysInfo, SYSINFO_SIZE);
    Get_IpInfo((uint8_t *)&ipInfo, IPINFO_SIZE);
    
    pMBRInfo = (MBRInfo_t *)&mbrInfo;
    pSysInfo = (SysInfo_t *) &sysInfo;
    pIpInfo = (IpInfo_t *)&ipInfo;

    printf("Check MBR %d %d \n", pMBRInfo->mark1, pMBRInfo->mark2);

    #ifndef E2PROM_RESTORE_ENABLE
    if(pMBRInfo->mark1 == DEFAULT_MARK1&& pMBRInfo->mark2 ==  DEFAULT_MARK2){
        printf("E2PROM already initialized\n");
        
    }else{
        printf("E2PROM not initialized\n");
        E2PROM_Reset_Default();
    }
    #else
        E2PROM_Reset_Default();
    #endif
    
    Print_SysInfo(pSysInfo);
    Print_IpInfo(pIpInfo);

}


/* USER CODE END 1 */



