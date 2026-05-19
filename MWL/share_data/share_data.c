
/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : share_data.c
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 共享数据管理
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include "share_data.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>
#include <stdlib.h>
/* ==============================  DEFINES   =============================== */
#define MUTEX_WAIT_DELAY    portMAX_DELAY    /*互斥访问超时时间，时间节拍*/
/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
typedef struct                                                                          /*共享内存*/
{
    system_info_t       *system_info;
    control_info_t      *control_info;
    mesure_info_t       *mesure_info;
    fault_info_t        *fault_info;
}share_data_t;

typedef struct 
{
    share_data_type_e   type;                                                       /* 共享数据类型 */
    uint8_t**           data_addr;                                                  /* 指向数据地址的指针 */
    uint32_t            data_size;                                                  /* 数据长度*/
    SemaphoreHandle_t*  lock;                                                       /* 互斥信号量 */
}share_data_attri_t;


/* ==============================  EXTERNS   =============================== */
share_data_t      share_data;                                                    /* share_data */
static SemaphoreHandle_t    mutex[MAX_SHARE_DATA] = {NULL};                             /* 互斥量 */
static const share_data_attri_t c_share_data_attri_tab[MAX_SHARE_DATA]   =              /* 共享数据属性 */                        
{
    {SYSTEM_INFO,       (uint8_t**)&share_data.system_info,         sizeof(system_info_t),      &mutex[SYSTEM_INFO]},
    {CONTROL_INFO,      (uint8_t**)&share_data.control_info,        sizeof(control_info_t),     &mutex[CONTROL_INFO]},      
    {MESURE_INFO,       (uint8_t**)&share_data.mesure_info,         sizeof(mesure_info_t),      &mutex[MESURE_INFO]},    
    {FAULT_INFO,        (uint8_t**)&share_data.fault_info,          sizeof(fault_info_t),       &mutex[FAULT_INFO]},
};
/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :share_data_init（共享内存初始化）
*Description:
*Input      :grid_type_e    type    要申请内存的设备类型
*Output     :
*Returns    :bool    false/true    失败/成功
*Note       :rtos初始化后,读取系统参数前调用。
*****************************************************************************************************/
bool share_data_init(grid_type_e type)
{
    /*定义个控制器的数据类型使能*/
    uint8_t ccu[MAX_SHARE_DATA] = {1,1,1,1};
    uint8_t pdu[MAX_SHARE_DATA] = {1,1,1,1};
    uint8_t pcu[MAX_SHARE_DATA] = {1,1,1,1};

    /*清空共享内存各数据板块指针*/
    memset(&share_data, 0, sizeof(share_data_t));
    
    for(int i = 0; i < MAX_SHARE_DATA; i++)
    {
        /*按照指定控制器使能的数据板块进行内存初始化*/
        if( ((type&0xf00) == (PCU_V1&0xf00) && pcu[i] == 0)
            ||((type&0xf00) == (CCU_V1&0xf00) && ccu[i] == 0)
            || ((type&0xf00) == (PDU_V1&0xf00) && pdu[i] == 0))
        {
            continue;
        }
        
        /*申请互斥量*/
        if (NULL == mutex[i])
        {
            mutex[i] = xSemaphoreCreateMutex();
        }
        /*申请内存，将申请到的内存指针赋值到数据指针*/
        *c_share_data_attri_tab[i].data_addr = malloc(c_share_data_attri_tab[i].data_size);
        /*如果指针有效，则开始对共享内存数据进行初始化*/
        if (NULL != *c_share_data_attri_tab[i].data_addr)
        {
            memset(*c_share_data_attri_tab[i].data_addr,0,c_share_data_attri_tab[i].data_size);
            *(*c_share_data_attri_tab[i].data_addr + c_share_data_attri_tab[i].data_size -1) = 0xAA;
            *(*c_share_data_attri_tab[i].data_addr + c_share_data_attri_tab[i].data_size -2) = 0x55;
        }
        /*如果互斥量为空或者共享内存指针为空，则说明初始化异常，返回失败*/
        if (NULL == mutex[i] || NULL == *c_share_data_attri_tab[i].data_addr)
        {
            return false;
        }
    }
    return true;
}

/*****************************************************************************************************
*Function   :share_data_read（读取共享内存数据）
*Description:
*Input      :const share_data_type_t    type      读取的数据类型
             const uint32_t             offset    读取的数据偏移量
             const uint32_t             size      读取数据长度（单位：字节）
*Output     :const void*                out       读取数据存放地址
*Returns    :bool    false/true    失败/成功
*Note       :rtos初始化后,读取系统参数前调用。该函数会阻塞不能在中断中调用。
*****************************************************************************************************/
bool share_data_read(const share_data_type_e type, const uint32_t offset, void* out, const uint32_t size)
{
    uint8_t* p_start;
    
    /*判断数据存放指针是否有效*/
    if(NULL == out)
    {
        return false;
    }
    /*判断输入是否溢出*/
    if((type >= MAX_SHARE_DATA)
       || offset >= c_share_data_attri_tab[(uint8_t)type].data_size
       || (size > c_share_data_attri_tab[(uint8_t)type].data_size - offset))
    {
        return false;
    }
    
    if(*c_share_data_attri_tab[(uint8_t)type].lock == NULL)
    {
        return false;
    }

    /*申请互斥信号量，xSemaphoreTake和xSemaphoreGive一定要成对调用，这里申请互斥量等待时间为1ms，为了尽可能不影响调用任务正常运行*/
    if(pdTRUE == xSemaphoreTake(*c_share_data_attri_tab[(uint8_t)type].lock, MUTEX_WAIT_DELAY))
    {
        /*计算偏移量*/
        p_start = (uint8_t*)*c_share_data_attri_tab[(uint8_t)type].data_addr + offset;
        /*拷贝数据到输出*/
        memcpy(out, p_start, size);
        /*释放互斥信号量*/
        xSemaphoreGive(*c_share_data_attri_tab[(uint8_t)type].lock);
        return true;
    }
    else
    {
        printf("share_data_read fail\n");
        return false;
    }
}

/*****************************************************************************************************
*Function   :share_data_write（写入数据到共享内存）
*Description:
*Input      :const share_data_type_t    type      写入的数据类型
             const uint32_t             offset    写入的数据偏移量
             void*                      in        待写入的数据地址
             const uint32_t             size      写入数据长度（单位：字节）
*Output     :
*Returns    :bool    false/true    失败/成功
*Note       :rtos初始化后,读取系统参数前调用。该函数会阻塞不能在中断中调用。
*****************************************************************************************************/
bool share_data_write(const share_data_type_e type, const uint32_t offset, const void* in, const uint32_t size)
{
    uint8_t* p_start;
    
    /*判断数据写入指针是否有效*/
    if(NULL == in)
    {
        return false;
    }
    /*判断输入是否溢出*/
    if((type >= MAX_SHARE_DATA)
       || offset >= c_share_data_attri_tab[(uint8_t)type].data_size
       || (size > c_share_data_attri_tab[(uint8_t)type].data_size - offset))
    {
        return false;
    }

    /*申请互斥信号量，xSemaphoreTake和xSemaphoreGive一定要成对调用，这里申请互斥量使用了无限阻塞式，期望是写入数据时必须一次性成功*/
    if(pdTRUE == xSemaphoreTake(*c_share_data_attri_tab[(uint8_t)type].lock, MUTEX_WAIT_DELAY))
    {
        /*计算偏移量*/
        p_start = (uint8_t*)*c_share_data_attri_tab[(uint8_t)type].data_addr + offset;
        /*写入数据到共享内存*/
        memcpy(p_start, in, size);
        /*释放互斥信号量*/
        xSemaphoreGive(*c_share_data_attri_tab[(uint8_t)type].lock);
        return true;
    }
    else
    {
        return false;
    }
}
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
