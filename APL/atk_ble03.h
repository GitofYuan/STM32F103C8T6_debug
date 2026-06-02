/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : atk_ble03.h
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 
******************************************************************************/
#ifndef __ATK_BLE03_H__
#define __ATK_BLE03_H__
/* ==============================  INCLUDES  =============================== */
#include "uart_bus.h"
/* ==============================  DEFINES   =============================== */
#define DEBUG_BLE03           /*debug使能，完整保留“DEBUG_BLE03”为使能debug*/
#define BLE_SEND_INTERVAL      200     /*两帧之间发送的最小间隔 ms*/

/* ATK-BLE03 AT指令集（自动带 \r\n） */
#define BLE03_AT_RESET          "AT+RESET\r\n"           // 复位
#define BLE03_AT_RELOAD         "AT+RELOAD\r\n"          // 恢复出厂
#define BLE03_AT_SLED0          "AT+SLED0\r\n"           // LED灭(未连接)
#define BLE03_AT_SLED1          "AT+SLED1\r\n"           // LED闪烁(默认)
#define BLE03_AT_SPR0           "AT+SPR0\r\n"           // 关闭上电回传信息
#define BLE03_AT_SPR1           "AT+SPR1\r\n"           // 打开上电回传信息


#define BLE03_AT_CBN            "AT+CBN\r\n"              // 查询BLE名称
#define BLE03_AT_SBN            "AT+SBN%s\r\n"         // 设置BLE名称
#define BLE03_AT_CSN            "AT+CSN\r\n"              // 查询SPP名称
#define BLE03_AT_SSN            "AT+SSN%s\r\n"         // 设置SPP名称

#define BLE03_AT_SBMAC            "AT+SBMAC%s\r\n"         // 设置BLE MAC地址
#define BLE03_AT_CBMAC            "AT+CBMAC\r\n"         // 查询BLE MAC地址
 

#define BLE03_AT_SBSW0          "AT+SBSW0\r\n"           // 关闭BLE
#define BLE03_AT_SBSW1          "AT+SBSW1\r\n"           // 打开BLE
#define BLE03_AT_CBSW           "AT+CBSW\r\n"            // 查询BLE状态

#define BLE03_AT_SSSW0          "AT+SSSW0\r\n"           // 关闭SPP
#define BLE03_AT_SSSW1          "AT+SSSW1\r\n"           // 打开SPP
#define BLE03_AT_CSSW           "AT+CSSW\r\n"            // 查询SPP状态

#define BLE03_AT_SSPW           "AT+SSPW%s\r\n"        // 设置SPP密码(4位)
#define BLE03_AT_SSPSW1         "AT+SSPSW1\r\n"          // 使能SPP密码
#define BLE03_AT_SSPSW0         "AT+SSPSW0\r\n"          // 关闭SPP密码

#define BLE03_AT_SUART          "AT+SUART%s\r\n"       // 设置波特率 01~16
#define BLE03_AT_CUART          "AT+CUART\r\n"           // 查询波特率

#define BLE03_AT_STXP           "AT+STXP%s\r\n"        // 发射功率 00~09
#define BLE03_AT_SU0            "AT+SU0%s\r\n"      // 设置主UUID
#define BLE03_AT_SU1            "AT+SU1%s\r\n"      // 设置特征UUID1
/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
/*蓝牙模块初始化数据*/
typedef struct {
    uint8_t   ble_name[20];
    uint8_t   spp_name[20];
    uint8_t   ble_mac[13];
    uint8_t   uuid[17];
} atk_ble03_init_data_t;
/* ==============================  EXTERNS   =============================== */


/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool atk_ble03_init(atk_ble03_init_data_t *init_data);

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool ble_rx_dequeue(uint8_t len, uint8_t* data);

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool ble_tx_enqueue(uint8_t len, uint8_t* data);

#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
