/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : can_control.c
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.02.27
* Description       :
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include <stdio.h>
#include <string.h>
#include "can.h"

#include "device_control.h"
#include "can_bus.h"
/* ==============================  DEFINES   =============================== */


/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */
extern CAN_HandleTypeDef hcan;

/* ========================= FUNCTION PROTOTYPES =========================== */

// ====================== CAN适配实现 ======================
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static bool can_stm32_init(DeviceRuntimeInfo *dev, device_ctrl_content_u *content) 
{
    // 初始化CAN（默认250K，可后续通过control修改）
    CAN_HandleTypeDef *can = dev->hw_res.can.hcan;

    /* CAN初始化参数配置 */
    switch(content->can.baudrate)                    /* 波特率 */
    {
        case BAUD_CAN_125K:
            can->Init.Prescaler = 8;
            can->Init.SyncJumpWidth = CAN_SJW_1TQ;
            can->Init.TimeSeg1 = CAN_BS1_13TQ;
            can->Init.TimeSeg2 = CAN_BS2_2TQ;
            break;
        case BAUD_CAN_250K:
            can->Init.Prescaler = 8;
            can->Init.SyncJumpWidth = CAN_SJW_1TQ;
            can->Init.TimeSeg1 = CAN_BS1_6TQ;
            can->Init.TimeSeg2 = CAN_BS2_1TQ;
            break;
        case BAUD_CAN_500K:
            can->Init.Prescaler = 4;
            can->Init.SyncJumpWidth = CAN_SJW_1TQ;
            can->Init.TimeSeg1 = CAN_BS1_6TQ;
            can->Init.TimeSeg2 = CAN_BS2_1TQ;
            break;
        default:
            return false;
    }
    can->Instance  = dev->hw_res.can.instance;
    can->Init.Mode = CAN_MODE_NORMAL;
    can->Init.TimeTriggeredMode = DISABLE;
    can->Init.AutoBusOff = DISABLE;
    can->Init.AutoWakeUp = ENABLE;
    can->Init.AutoRetransmission = ENABLE;
    can->Init.ReceiveFifoLocked = DISABLE;
    can->Init.TransmitFifoPriority = DISABLE;
    
    if (HAL_CAN_Init(can) != HAL_OK)
    {
        return false;
    }

    /* 启动 CAN 模块并激活接收中断 */
    HAL_CAN_Start(can);
    HAL_CAN_ActivateNotification(can, CAN_IT_RX_FIFO0_MSG_PENDING);

    // 初始化设备状态
    dev->is_opened = true; // UART初始化后需手动打开
    return true;
}

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool can_stm32_control(DeviceRuntimeInfo *dev, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content) 
{
    if (dev == NULL || content == NULL) 
    {
       return false;
    }

    CAN_HandleTypeDef *can = dev->hw_res.can.hcan;
    switch (ctrl_type) 
    {
        case DEV_CTRL_OPEN:
           can_stm32_init(dev, content);
           break;
        case DEV_CTRL_CLOSE:
           HAL_CAN_DeInit(can);
           dev->is_opened = false;
           break;

        /*CAN只通过中断出发接收然后进入can_bus，不能手动读取*/
        case DEV_CTRL_READ:
            return false;
        case DEV_CTRL_WRITE:
        {
            uint32_t             TxMailbox;  /*发送邮箱*不用管，调用HAL_CAN_AddTxMessage函数用的必要填充而已*/
            CAN_TxHeaderTypeDef  TxHeader;   /*CAN消息头结构体*/
            
            /*设置消息头*/
            TxHeader.StdId = 0x0;                          /*设置标准ID*/
            TxHeader.ExtId = content->can.id;              /*设置扩展ID*/
            TxHeader.RTR   = CAN_RTR_DATA;                 /*数据帧*/
            TxHeader.IDE   = CAN_ID_EXT;                   /*选择以扩展ID发送*/
            TxHeader.DLC   = content->can.dlc;             /*数据长度*/
            
            if(HAL_CAN_AddTxMessage(can, &TxHeader, content->can.data, &TxMailbox) == HAL_OK)
            {
                break;
            }
            else
            {
                uint32_t free_mailboxes = HAL_CAN_GetTxMailboxesFreeLevel(can);
                HAL_CAN_StateTypeDef can_state = HAL_CAN_GetState(can);

                /* 发送失败：打印诊断信息，不再在写路径中重发 */
                #ifdef DEBUG_CAN_BUS
                printf("CAN send failure: free_mailboxes=%lu, state=%lu\n",
                       (unsigned long)free_mailboxes,                        /*空闲邮箱数*/
                       (unsigned long)can_state);                            /*CAN状态*/
                #endif

                /*如果CAN处于错误状态或总线复位状态，尝试重新初始化CAN*/
                if(can_state == HAL_CAN_STATE_ERROR || can_state == HAL_CAN_STATE_RESET)
                {
                    
                    device_ctrl_content_u baud_rate;
                    baud_rate.can.baudrate = dev->hw_res.can.baud_rate;
                    can_stm32_init(dev, &baud_rate);
                }
                return false;
            }
        }
        case DEV_CTRL_CONFIG:
            HAL_CAN_DeInit(can);
            can_stm32_init(dev, content);
            break;
        case DEV_CTRL_GET_STATUS:
            content->is_open = dev->is_opened;
            break;
        default:
            return false;
    }
    return true;
}

/*****************************************************************************************************
*Function   :HAL_CAN_RxFifo0MsgPendingCallback（CAN接收完成回调函数重定义）
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :待完善
*****************************************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_t)
{
	CAN_RxHeaderTypeDef  pMycan_tx_Head;   /*接收的报文头信息*/
	Can_Rcv_Data         g_can1_rcv;       /*报文接收数据（can_bus自定义结构体）*/
	uint8_t              RxData[8];        /*报文内容*/
	
	if (hcan_t->Instance == hcan.Instance && HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &pMycan_tx_Head, RxData) == HAL_OK)
	{
        /*将接收到的报文头信息复制到can_bus要求的结构体变量*/
        g_can1_rcv.id = pMycan_tx_Head.ExtId;
        g_can1_rcv.len = pMycan_tx_Head.DLC;
        for (int i = 0; i < 8; i++)
        {
            g_can1_rcv.data[i] = RxData[i];
        }

        /*数据入队*/
        can_rx_enqueue(CAN_DATA_QUEUE_CHNL_1, &g_can1_rcv);

        /*重新使能接收中断*/
        HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	}
}

/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
