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
uint8_t ble_send_timer = 0;             /*BLE03发送计时器，单位ms*/
uart_data_s ble_rx_data = {0};      /*BLE03接收数据结构体*/
bool ble_init_flag = false;   /*BLE03初始化标志位*/
enum
{
    BLE_INIT_IDLE = 0,
    BLE_INIT_NAME_BLE,
    BLE_INIT_NAME_SPP,
    BLE_INIT_MAC,
    BLE_INIT_UUID,
    BLE_INIT_COMPLETE,
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
void atk_ble03_init(atk_ble03_init_data_t *init_data)
{
    if(ble_init_flag == false)
    {
        /*初始化BLE03模块*/
        switch(ble_init_state)
        {
            case BLE_INIT_IDLE:
                /*初始化BLE03模块*/
                printf("start init BLE03\r\n");
                BLE03_SendCmd(BLE03_AT_SBSW0, NULL);
                BLE03_SendCmd(BLE03_AT_SSSW0, NULL);
                ble_init_state = BLE_INIT_NAME_BLE;
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
                    memset(ble_rx_data.data, 0, UART_DATA_MAX);
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
                        ble_init_state = BLE_INIT_MAC;
                    }
                    else
                    {
                        printf("%s",(char*)ble_rx_data.data);
                    }
                    memset(ble_rx_data.data, 0, UART_DATA_MAX);
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
                        ble_init_state = BLE_INIT_UUID;
                    }
                    else
                    {
                        printf("%s",(char*)ble_rx_data.data);
                    }
                    memset(ble_rx_data.data, 0, UART_DATA_MAX);
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
                        ble_init_state = BLE_INIT_COMPLETE;
                    }
                    else
                    {
                        printf("%s",(char*)ble_rx_data.data);
                    }
                    memset(ble_rx_data.data, 0, UART_DATA_MAX);
                }
                
                break;
            case BLE_INIT_COMPLETE:
                /*初始化完成*/
                printf("BLE03 initialization complete\r\n");
                BLE03_SendCmd(BLE03_AT_RESET, NULL);
                BLE03_SendCmd(BLE03_AT_SBSW1, NULL);
                BLE03_SendCmd(BLE03_AT_SSSW1, NULL);
                ble_init_flag = true;
                break;
        }
        
    }
    /*这里先暂时不对BLE模块进行初始化，后续需要根据实际协议要求进行初始化*/
}
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void atk_ble03_handle_task(void)
{
    if(ble_init_flag == false)
    {
        /*如果BLE模块未初始化，则先进行初始化*/
        atk_ble03_init(NULL);
    }
    else if(uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &ble_rx_data) == true)
    {
        /*这里暂时先不对接收到的数据进行解析处理，后续需要根据实际协议要求进行解析处理*/
        /*debug打印接收到的数据*/
//        print_len = ble_rx_data.len;
//        memcpy(print_buf, ble_rx_data.data, print_len);
//        printf("receive data: %s\r\n", print_buf);
    }
    else
    {
        /*没有接收到数据，或者数据出队失败*/
    }
    
}
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
