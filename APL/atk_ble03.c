/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : atk_ble03.c
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.05.30
* Description       :
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include <string.h>
#include <stdio.h>

#include "atk_ble03.h"
/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */
uint8_t ble_tx_buf[UART_DATA_MAX];      /*BLE03发送缓冲区，大小根据实际需求定义*/
uint16_t ble_send_timer = 0;             /*BLE03发送计时器，单位ms*/
uart_data_s ble_rx_data = {0};      /*BLE03接收数据结构体*/

enum
{
    BLE_INIT_IDLE = 0,    /*空闲*/
    BLE_INIT_SPR0,        /*设置上电回传功能关闭*/
    BLE_INIT_SBSW0,       /*关闭BLE*/
    BLE_INIT_SSSW0,       /*关闭SPP*/
    BLE_INIT_NAME_BLE,    /*设置BLE名称*/
    BLE_INIT_NAME_SPP,    /*设置SPP名称*/
    BLE_INIT_MAC,         /*设置BLEmac地址*/
    BLE_INIT_UUID,        /*设置主UUID*/
    BLE_INIT_UUID1,       /*设置特征码1*/
    BLE_INIT_UUID2,       /*设置特征码2*/
    BLE_INIT_UUID3,       /*设置特征码3*/
    BLE_INIT_RESET,       /*复位*/
    BLE_INIT_SBSW1,       /*开启BLE*/
    BLE_INIT_SSSW1,       /*开启SPP*/
    BLE_INIT_COMPLETE,    /*初始化完成*/
}ble_init_state = BLE_INIT_IDLE;   /*BLE03初始化状态枚举*/    


/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void BLE03_SendCmd(const char *format, const uint8_t *param)
{
    ble_send_timer++;
    if(ble_send_timer >= BLE_SEND_INTERVAL)
    {
        // 清空缓冲区
        memset(ble_tx_buf, 0, UART_DATA_MAX);
        if(param == NULL)
        {
            snprintf((char*)ble_tx_buf, UART_DATA_MAX, format);
            printf("%s\n",format);
        }
        else
        {
            // 拼接 AT 指令（format=模板，param=你的 uint8_t 数组）
            snprintf((char*)ble_tx_buf, UART_DATA_MAX, format, (char*)param);
            printf("%s\n",(char*)param);
        }
        
        // 发送
        uart_data_s ble_tx_data = {0};
        ble_tx_data.len = strlen((char*)ble_tx_buf);
        memcpy(ble_tx_data.data, ble_tx_buf, ble_tx_data.len);
        ble_tx_data.timeout = ble_tx_data.len;
        uart_tx_enqueue(UART_DATA_QUEUE_CHNL_1, &ble_tx_data);

        ble_send_timer = 0;
    }
}
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool atk_ble03_init(atk_ble03_init_data_t *init_data)
{
    bool ret = false;
    /*初始化BLE03模块*/
    switch(ble_init_state)
    {
        case BLE_INIT_IDLE:
            /*初始化BLE03模块*/
            printf("start init BLE03\r\n");
            ble_init_state = BLE_INIT_SPR0;
            break;
        case BLE_INIT_SPR0:
            /*初始化BLE03模块*/
            BLE03_SendCmd(BLE03_AT_SPR1, NULL);
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_NAME_BLE;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
        case BLE_INIT_SBSW0:
            /*初始化BLE03模块*/
            BLE03_SendCmd(BLE03_AT_SBSW0, NULL);
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_SSSW0;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
        case BLE_INIT_SSSW0:
            /*初始化BLE03模块*/
            BLE03_SendCmd(BLE03_AT_SSSW0, NULL);
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_NAME_BLE;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
        case BLE_INIT_NAME_BLE:
            /*设置BLE03模块名称*/
            BLE03_SendCmd(BLE03_AT_SBN, init_data->ble_name);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_NAME_SPP;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
        case BLE_INIT_NAME_SPP:
            /*设置BLE03模块SPP名称*/
            BLE03_SendCmd(BLE03_AT_SSN, init_data->spp_name);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_UUID;
                }
                else
                {
                    printf("%.*s", ble_rx_data.len, (char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
        case BLE_INIT_UUID:
            /*设置BLE03模块UUID*/
            BLE03_SendCmd(BLE03_AT_SU0, init_data->uuid);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_UUID1;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
       
        case BLE_INIT_UUID1:
            /*设置BLE03模块特征码1*/
            BLE03_SendCmd(BLE03_AT_SU1, init_data->uuid1);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_UUID2;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
       case BLE_INIT_UUID2:
            /*设置BLE03模块特征码1*/
            BLE03_SendCmd(BLE03_AT_SU2, init_data->uuid2);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_UUID3;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
       case BLE_INIT_UUID3:
            /*设置BLE03模块特征码1*/
            BLE03_SendCmd(BLE03_AT_SU3, init_data->uuid3);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_MAC;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
       case BLE_INIT_MAC:
            /*设置BLE03模块MAC地址*/
            BLE03_SendCmd(BLE03_AT_SBMAC, init_data->ble_mac);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_SBSW1;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
       case BLE_INIT_SBSW1:
            /*开启BLE蓝牙*/
            BLE03_SendCmd(BLE03_AT_SBSW1, NULL);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_SSSW1;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
        case BLE_INIT_SSSW1:
            /*开启SPP蓝牙*/
            BLE03_SendCmd(BLE03_AT_SSSW1, NULL);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if(strcmp((char*)ble_rx_data.data, "OK\r\n") == 0)
                {
                    ble_init_state = BLE_INIT_RESET;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
        case BLE_INIT_RESET:
            /*重启*/
            BLE03_SendCmd(BLE03_AT_RESET, NULL);
            
            if(true == uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data))
            {
                /*接收到的数据为“OK”*/
                if((char)ble_rx_data.data[0] == 'B')
                {
                    ble_init_state = BLE_INIT_COMPLETE;
                }
                else
                {
                    printf("%s",(char*)ble_rx_data.data);
                }
                memset(&ble_rx_data, 0, sizeof(uart_data_s));
            }
            break;
            
        case BLE_INIT_COMPLETE:
            /*初始化完成*/
            printf("BLE03 initialization complete\r\n");
            ret = true;
            break;
    }
    /*这里先暂时不对BLE模块进行初始化，后续需要根据实际协议要求进行初始化*/
    return ret;
}

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :uint8_t len, uint8_t* data
*Returns    :
*Note       :
*****************************************************************************************************/
bool ble_rx_dequeue(uint8_t* len, uint8_t* data)
{
    bool ret = false;
    uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data);
    if(ble_rx_data.len > 0)
    {
//        printf("recv bt: %.*s\n", ble_rx_data.len, ble_rx_data.data);
        memcpy(data, ble_rx_data.data, ble_rx_data.len);
        *len = ble_rx_data.len;
        memset(&ble_rx_data, 0, sizeof(uart_data_s));
        ret = true;
    }
    return ret;
}

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool ble_tx_enqueue(uint8_t len, uint8_t* data)
{
    bool ret = false;
    uart_data_s ble_tx_data = {0};      /*BLE03发送数据结构体*/
    ble_tx_data.len = len;
    memcpy(ble_tx_data.data, data, len);
    if(uart_tx_enqueue(UART_DATA_QUEUE_CHNL_1, &ble_tx_data) == true)
    {
        ret = true;
    }
    return ret;
}
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
//void atk_ble03_handle_task(void)
//{
//    if(ble_init_flag == false)
//    {
//        /*如果BLE模块未初始化，则先进行初始化*/
//        atk_ble03_init(&ble_init_data);
//    }
//    else
//    {
//        uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data);
//        if(ble_rx_data.len > 0)
//        {
//            printf("recv bt: %.*s\n", ble_rx_data.len, ble_rx_data.data);
//            memset(&ble_rx_data, 0, sizeof(uart_data_s));
//        }
//        
//        if(ble_send_timer >= 200)
//        {
//            device_ctrl_content_u ble_state = {0};
//            device_control(DEV_TYPE_GPIO, "address1", DEV_CTRL_READ, &ble_state);
//            if(ble_state.gpio.level == GPIO_SET)
//            {
//                uart_data_s data = {0};
//                data.len = snprintf((char*)ble_tx_buf, UART_DATA_MAX, "HELLO\r\n");
//                memcpy(data.data, ble_tx_buf, data.len);
//                data.timeout = data.len;
//                uart_tx_enqueue(UART_DATA_QUEUE_CHNL_1, &data);
//                
//                printf("send bt: HELLO\n");
//            }
//            
//            
//            ble_send_timer = 0;
//            
//        }
//        ble_send_timer ++;
//    }
//    
//}
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
