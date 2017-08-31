
#include "mytask.h"
#include "myserial_log.h"
#include "myserial_hmi.h"
#include "myserial_plc.h"


void Task_Init(){
    Log_Task_Init();

    Plc_Task_Init();
    Hmi_Task_Init();
    
}