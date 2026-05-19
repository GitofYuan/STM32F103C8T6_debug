/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : J1939.c
* Author            : Yuan.Zong
* Version           : V1.0.0/2025.12.18
* Description       : 本J1939协议栈初步调试完成，时未对J1939协议中PDU2部分进行处理，
以及未支持TP.CM BAM传输，后续有机会的时候再完善。
******************************************************************************/

/* ==============================  INCLUDES  =============================== */
#include <string.h>
#include "J1939.h"
/* ==============================  DEFINES   =============================== */
#define TP_PRIORITY        7             /*TP类型报文优先级，含TPCM和TPDT*/
#define TP_LEN             8             /*TP类型报文数据长度，含TPCM和TPDT*/
#define TPCM_PF            0xEC          /*TPCM报文PF值*/
#define TPDT_PF            0xEB          /*TPDT报文PF值*/
#define TPDT_DATA_SIZE     7             /*TPDT报文数据域大小*/
#define TPDT_RETRANS_MAX   3             /*TPDT报文数据最大允许重传次数*/

#define TR      200           /*接收节点接收到RTS或最后一包DT到发出CTS或ACK的使劲按间隔
                                以及发送节点接收到CTS到发出DT的时间间隔*/
#define TH      500           /*接收节点用于维持连接的两个CTS之间的时间间隔*/
#define T1      750           /*接收节点接收到上一包DT后的等待延时*/
#define T2      1250          /*接收节点发送CTS后的等待延时*/
#define T3      1250          /*发送节点发送一包数据后等待CTS或ACK的延时*/
#define T4      1050          /*发送节点接收到CTS（0）后的等待延时*/
/* ==============================   ENUMS    =============================== */
/*J1939多帧报文接收状态*/
typedef enum
{
    IDLE = 0,      /*空闲*/
    TPCM_RTS,      /*连接请求发送*/
    TPCM_CTS,      /*连接准备发送*/
    TPCM_BAM,      /*连接广播消息*/
    TPDT,          /*数据传输*/
    TPCM_ENDACK,   /*连接结束*/
    TPCM_ABORT,    /*放弃连接*/
}J1939_TP_STATUS;

/*J1939传输结果*/
typedef enum 
{
    TP_OK,         /*传输正常*/
    TP_ERROR,      /*传输错误*/
    TP_BUSY,       /*传输繁忙*/
}J1939_TP_RESULT;

/*J1939放弃连接原因*/
typedef enum
{
    BUSY = 1,         /*传输繁忙*/
    NO_RESOURCES,     /*资源不足*/
    TIMEOUT,          /*连接超时*/
    UNEXPECTED_CTS,   /*未知的CTS*/
    RETRANS_EXCEEDED, /*重传次数超限*/
}TPCM_ABORT_REASON;

/*J1939放弃连接原因*/
typedef enum
{
    TPCM_CTRL_RTS = 16,            /*请求发送控制字*/
    TPCM_CTRL_CTS = 17,            /*准备发送控制字*/
    TPCM_CTRL_ACK = 19,            /*结束应答控制字*/
    TPCM_CTRL_ABORT = 255,         /*放弃连接控制字*/
    TPCM_CTRL_BAM = 32,            /*广播消息控制字*/
}TPCM_CTRL;

/*J1939计时器类型*/
typedef enum
{
    TIMER_TR = 0,     /*Tr计时器*/
    TIMER_TH,         /*Th计时器*/
    TIMER_T1,         /*T1计时器*/
    TIMER_T2,         /*T2计时器*/
    TIMER_T3,         /*T3计时器*/
    TIMER_T4,         /*T4计时器*/
    TIMER_MAX         /*计时器最大数量*/
}TP_TIMER;

/*计时器状态*/
typedef enum
{
    T_DISABLE = 0,     /*禁用*/
    T_ENABLE,          /*使能*/
}Timer_Status;

/*计时器目标状态*/
typedef enum
{
    STOP = 0,     /*停止*/
    RESTART,      /*重置*/
}Target_Status;

/*输出传输角色*/
typedef enum
{
    RX,         /*接收方*/
    TX,         /*发送方*/
}TP_Role;
/* ======================== STRUCTURES AND UNIONS ========================== */
/* J1939协议数据单元结构体*/
typedef struct
{
    uint8_t       p;           /*优先级*/
    uint8_t       pf;          /*PDU格式*/
    uint8_t       ps;          /*特定PDU*/
    uint8_t       sa;          /*源地址*/
    uint8_t       len;         /*数据长度*/
    uint32_t      pgn;         /*参数组编号*/
    union 
    {
        struct 
        {
            uint16_t           total_byte_num;                   /*报文总字节数*/
            uint8_t            total_pack_num;                   /*报文总包数*/
            uint32_t           pgn;                              /*参数组编号*/
        } rts;
        struct 
        {
            uint8_t            pack_num_to_be_trans;             /*待传输包数量*/
            uint8_t            next_pack_num;                    /*下一个将要传输的包序号*/
            uint32_t           pgn;                              /*参数组编号*/

        } cts;
        struct 
        {
            uint16_t           total_byte_num;                   /*报文总字节数*/
            uint8_t            total_pack_num;                   /*报文总包数*/
            uint32_t           pgn;                              /*参数组编号*/
        } ack;
        struct 
        {
            TPCM_ABORT_REASON  abort_reason;                     /*放弃连接原因*/
            uint32_t           pgn;                              /*参数组编号*/
        } abort;
        struct 
        {
            uint16_t           total_byte_num;                   /*报文总字节数*/
            uint8_t            total_pack_num;                   /*报文总包数*/
            uint32_t           pgn;                              /*参数组编号*/
        } bam;
        uint8_t       data[8];     /*数据域*/
    };
}J1939_PDU;

/* J1939接收链表（单向） 结构体*/
typedef struct j1939_rx_data_queue
{
    J1939_Data                   j1939_data;  /*CAN数据*/
    struct j1939_rx_data_queue*  p_next;      /*链表指针*/
}stru_j1939_rx_data_queue;

/* J1939接收队列指针结构体*/
typedef struct
{
    stru_j1939_rx_data_queue* head;     /*链表头*/
    stru_j1939_rx_data_queue* tail;     /*链表尾*/
}stru_j1939_rx_data_pointer;

/* J1939发送链表（单向） 结构体*/
typedef struct j1939_tx_data_queue
{
    J1939_Data                    j1939_data;  /*CAN数据*/
    struct j1939_tx_data_queue*   p_next;      /*链表指针*/
}stru_j1939_tx_data_queue;

/* J1939发送队列指针结构体*/
typedef struct
{
    stru_j1939_tx_data_queue* head;     /*链表头*/
    stru_j1939_tx_data_queue* tail;     /*链表尾*/
}stru_j1939_tx_data_pointer;

/* J1939计时器结构体*/
typedef struct
{
    Timer_Status status;     /*计时器状态*/
    uint16_t timer_num;      /*计时数*/
    uint8_t  timeout_flag;   /*超时标志位*/
}stru_j1939_timer;

/* J1939多帧传输过程数据结构体*/
typedef struct
{
    uint8_t            da;                               /*目的地址*/
    uint8_t            sa;                               /*源地址*/
    J1939_TP_STATUS    tp_status;                        /*多帧传输阶段*/
    stru_j1939_timer   tp_timer[TIMER_MAX];              /*多帧交互计时器*/
    uint16_t           total_byte_num;                   /*报文总字节数*/
    uint8_t            total_pack_num;                   /*报文总包数*/
    uint8_t            pack_num_per_time;                /*可连续传输数据包数量*/
    uint32_t           pgn;                              /*参数组编号*/
    uint8_t            pack_num_to_be_trans;             /*待传输包总数量*/
    uint8_t            pack_num_to_be_trans_per_time;    /*当轮待传输包数量*/
    uint8_t            next_pack_num;                    /*下一个将要传输的包序号*/
    TPCM_ABORT_REASON  abort_reason;                     /*放弃连接原因*/
    uint8_t            tpdt_retrans_num;                 /*TPDT数据重传次数*/
    uint8_t            m_data_buffer[MAX_J1939_MESSAGE]; /*多帧数据缓冲区*/
}stru_j1939_tp_msg;

/* ==============================  EXTERNS   =============================== */
stru_j1939_rx_data_queue     g_j1939_rx_data_queue[CAN_DATA_CHNL_NUM_MAX][J1939_DATA_QUEUE_NUM];   /*J1939接收数据队列*/
stru_j1939_rx_data_pointer   g_j1939_rx_data_pointer[CAN_DATA_CHNL_NUM_MAX];                       /*J1939接收数据队列指针*/

stru_j1939_tx_data_queue     g_j1939_tx_data_queue[CAN_DATA_CHNL_NUM_MAX][J1939_DATA_QUEUE_NUM];   /*J1939发送数据队列*/
stru_j1939_tx_data_pointer   g_j1939_tx_data_pointer[CAN_DATA_CHNL_NUM_MAX];                       /*J1939发送数据队列指针*/

stru_j1939_tp_msg            j1939_rx_tp_msg[CAN_DATA_CHNL_NUM_MAX];          /*J1939接收处理多帧传输数据的结构体数组*/
stru_j1939_tp_msg            j1939_tx_tp_msg[CAN_DATA_CHNL_NUM_MAX];          /*J1939发送处理多帧传输数据的结构体数组*/
stru_j1939_tp_msg            tpcm_abort;                                      /*处理一些异常TP数据专用的Abort结构体*/
/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :j1939_timer_ctrl（J1939计时器控制函数）
*Description:这是一个J1939内部要求的几个计时器的计时控制函数，可实现对指定CAN通道接收或发送角色下的指定计时器控制。
             可控制某个计时器关闭或者重置。
*Input      :CAN_CHNL       chnl        CAN通道
             TP_Role        role        收发角色
             TP_TIMER       timer_type  计时器类型
             Target_Status  status      控制状态
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void j1939_timer_ctrl(CAN_CHNL chnl, TP_Role role,TP_TIMER timer_type, Target_Status status)
{
    if(role == RX)
    {
        j1939_rx_tp_msg[chnl].tp_timer[timer_type].status = (Timer_Status)status;
        j1939_rx_tp_msg[chnl].tp_timer[timer_type].timer_num = 0;
        j1939_rx_tp_msg[chnl].tp_timer[timer_type].timeout_flag = 0;
    }
    if(role == TX)
    {
        j1939_tx_tp_msg[chnl].tp_timer[timer_type].status = (Timer_Status)status;
        j1939_tx_tp_msg[chnl].tp_timer[timer_type].timer_num = 0;
        j1939_tx_tp_msg[chnl].tp_timer[timer_type].timeout_flag = 0;
    }
}

/*****************************************************************************************************
*Function   :j1939_timer（J1939计时器）
*Description:这是J1939的计时器，它实现对每个CAN通道J1939内部要求的几个计时器的计时任务，并在计时器超时后将其标记超时。
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void j1939_timer(void)
{
    CAN_CHNL chnl;
    TP_TIMER rx_timer;
    TP_TIMER tx_timer;
    
    for(chnl = CAN_DATA_QUEUE_CHNL_1; chnl < CAN_DATA_CHNL_NUM_MAX; chnl++)
    {
        for(rx_timer = TIMER_TR; rx_timer < TIMER_MAX; rx_timer++)
        {
            if(j1939_rx_tp_msg[chnl].tp_timer[rx_timer].status == T_ENABLE)
            {
                j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timer_num++;
                switch(rx_timer)
                {
                    case TIMER_TR:
                        if(j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timer_num >= TR)
                        {
                            j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_TH:
                        if(j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timer_num >= TH)
                        {
                            j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_T1:
                        if(j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timer_num >= T1)
                        {
                            j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_T2:
                        if(j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timer_num >= T2)
                        {
                            j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_T3:
                        if(j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timer_num >= T3)
                        {
                            j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_T4:
                        if(j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timer_num >= T4)
                        {
                            j1939_rx_tp_msg[chnl].tp_timer[rx_timer].timeout_flag = 1;
                        }
                        break;
                    default:break;
                }
            }
        }
    }
    for(chnl = CAN_DATA_QUEUE_CHNL_1; chnl < CAN_DATA_CHNL_NUM_MAX; chnl++)
    {
        for(tx_timer = TIMER_TR; tx_timer < TIMER_MAX; tx_timer++)
        {
            if(j1939_tx_tp_msg[chnl].tp_timer[tx_timer].status == T_ENABLE)
            {
                j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timer_num++;
                switch(tx_timer)
                {
                    case TIMER_TR:
                        if(j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timer_num >= TR)
                        {
                            j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_TH:
                        if(j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timer_num >= TH)
                        {
                            j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_T1:
                        if(j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timer_num >= T1)
                        {
                            j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_T2:
                        if(j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timer_num >= T2)
                        {
                            j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_T3:
                        if(j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timer_num >= T3)
                        {
                            j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timeout_flag = 1;
                        }
                        break;
                    case TIMER_T4:
                        if(j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timer_num >= T4)
                        {
                            j1939_tx_tp_msg[chnl].tp_timer[tx_timer].timeout_flag = 1;
                        }
                        break;
                    default:break;
                }
            }
        }
    }
}

/*****************************************************************************************************
*Function   :j1939_rx_queue_init(J1939接收队列链表初始化)
*Description:先判断输入的CAN通道是否为有效CAN通道，然后清空该CAN通道对应的接收结构体数组数据，将数组连接为单向环形链表
*Input      :CAN_CHNL chnl   CAN通道
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void j1939_rx_queue_init(CAN_CHNL chnl)
{
    if (chnl < CAN_DATA_CHNL_NUM_MAX)
    {
        memset(g_j1939_rx_data_queue[chnl], 0 ,sizeof(g_j1939_rx_data_queue[chnl]));

        for (int i = 0; i < CAN_DATA_QUEUE_NUM; i++)
        {
            /*如果i是数组最后一个，则连接向数组首地址，以形成环形链表*/
            if (i == J1939_DATA_QUEUE_NUM-1)
            {
                g_j1939_rx_data_queue[chnl][i].p_next = &g_j1939_rx_data_queue[chnl][0];
            }
            else
            {
                g_j1939_rx_data_queue[chnl][i].p_next = &g_j1939_rx_data_queue[chnl][i+1];
            }
        }
    }
}

/*****************************************************************************************************
*Function   :J1939_rx_enqueue（J1939接收入队函数）
*Description:先检查输入的CAN通道和输入的数据指针是否非法，如果有异常直接退出；
             然后检查当前CAN通道队列是否为空，如果是空的，则进行队列初始化，然后将数据信息复制到队列头；
             如果当前CAN通道队列非空，则先判断队列是否满了，如果满了也会直接退出；
             如果队列不满，则将队列尾后移，然后将数据信息复制到队列尾。
*Input      :CAN_CHNL       chnl   CAN通道
             J1939_Data*    data   J1939接收数据结构体
*Output     :
*Returns    :
*Note       :这里不考虑队列满了等异常情况的补救措施
*****************************************************************************************************/
void J1939_rx_enqueue(CAN_CHNL chnl, J1939_Data* data)
{
    if(chnl >= CAN_DATA_CHNL_NUM_MAX)
        return;
    if(data == NULL)
        return;
    if((g_j1939_rx_data_pointer[chnl].tail == NULL)
        ||(g_j1939_rx_data_pointer[chnl].head == NULL))
    {
        /*空队列*/
        g_j1939_rx_data_pointer[chnl].tail = &g_j1939_rx_data_queue[chnl][0];
        g_j1939_rx_data_pointer[chnl].head = &g_j1939_rx_data_queue[chnl][0];
        if (g_j1939_rx_data_pointer[chnl].tail->p_next == NULL)
        {
            j1939_rx_queue_init(chnl);
        }
        
        g_j1939_rx_data_queue[chnl][0].j1939_data.p = data->p;
        g_j1939_rx_data_queue[chnl][0].j1939_data.pgn = data->pgn;
        g_j1939_rx_data_queue[chnl][0].j1939_data.da = data->da;
        g_j1939_rx_data_queue[chnl][0].j1939_data.sa = data->sa;
        g_j1939_rx_data_queue[chnl][0].j1939_data.len = data->len;
        /*这里只要打印就会导致程序挂掉*/
//        #ifdef DEBUG_J1939
//                printf("J1939 enqueue PGN:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.pgn);
//                printf("J1939 enqueue DA:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.da);
//                printf("J1939 enqueue SA:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.sa);
//                printf("J1939 enqueue len:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.len);
//        #endif
        /*以下有些特殊处理，如果入队数据为单帧CAN数据，则数据域使用共用体单帧数组，如果是多帧CAN数据，则使用共用体多帧数组*/
        if(data->len > 8)
        {
            memcpy(g_j1939_rx_data_queue[chnl][0].j1939_data.data_m, data->data_m, MAX_J1939_MESSAGE);
            /*这里打印短数据没有问题，长数据就挂*/
            #ifdef DEBUG_J1939
            printf("123456789123456789");
            #endif
        }
        else
        {
            memcpy(g_j1939_rx_data_queue[chnl][0].j1939_data.data_s, data->data_s, 8);
//            #ifdef DEBUG_J1939
//            printf("J1939 de dui lie man le!\n");
//            #endif
        }
    }
    else if((g_j1939_rx_data_pointer[chnl].tail != NULL)
        &&(g_j1939_rx_data_pointer[chnl].head != NULL))
    {
        /*队列满了*/
        if (g_j1939_rx_data_pointer[chnl].tail->p_next == g_j1939_rx_data_pointer[chnl].head) 
        {
            /*发送错误处理*/
            #ifdef DEBUG_J1939
            printf("J1939 de dui lie man le!\n");
            #endif
        }
        else
        {
            g_j1939_rx_data_pointer[chnl].tail = g_j1939_rx_data_pointer[chnl].tail->p_next;
            
            g_j1939_rx_data_pointer[chnl].tail->j1939_data.p = data->p;
            g_j1939_rx_data_pointer[chnl].tail->j1939_data.pgn = data->pgn;
            g_j1939_rx_data_pointer[chnl].tail->j1939_data.da = data->da;
            g_j1939_rx_data_pointer[chnl].tail->j1939_data.sa = data->sa;
            g_j1939_rx_data_pointer[chnl].tail->j1939_data.len = data->len;
            #ifdef DEBUG_J1939
                printf("J1939 enqueue PGN:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.pgn);
                printf("J1939 enqueue DA:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.da);
                printf("J1939 enqueue SA:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.sa);
                printf("J1939 enqueue len:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.len);
            #endif
            if(data->len > 8)
            {
                memcpy(g_j1939_rx_data_pointer[chnl].tail->j1939_data.data_m, data->data_m, MAX_J1939_MESSAGE);
                #ifdef DEBUG_J1939
                    for(int i = 0; i < MAX_J1939_MESSAGE; i++)
                    {
                        printf("J1939 enqueue Data:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.data_m[i]);
                    }
                #endif
            }
            else
            {
                memcpy(g_j1939_rx_data_pointer[chnl].tail->j1939_data.data_s, data->data_s, 8);
                #ifdef DEBUG_J1939
                    for(int i = 0; i < 8; i++)
                    {
                        printf("J1939 enqueue Data:%d\n",g_j1939_rx_data_pointer[chnl].tail->j1939_data.data_s[i]);
                    }
                #endif
            }
        }
    }
}

/*****************************************************************************************************
*Function   :J1939_rx_dequeue（J1939接收出队函数）
*Description:这是一个J1939数据接收的出队函数，先校验CAN通道有效性，然后再校验队列是否为空；
             如果以上校验都通过了，则将队列头中的数据复制到函数输入的指针中，然后将队列头数据域清空，
             如果此时队列中仅有一个数据，则将队列进行复位，否则将队列头指针后移。
*Input      :CAN_CHNL       chnl   CAN通道
*Output     :J1939_Data*    data   J1939接收数据结构体
*Returns    :bool值         出队结果（true成功/false失败）
*Note       :
*****************************************************************************************************/
bool J1939_rx_dequeue(CAN_CHNL chnl, J1939_Data* data)
{
    bool ret;
    if(chnl >= CAN_DATA_CHNL_NUM_MAX)
    {
        return false;
    }
    
    if ((g_j1939_rx_data_pointer[chnl].tail == NULL)
            ||(g_j1939_rx_data_pointer[chnl].head == NULL))
    {
        /*空队列*/
        ret = false;
    }
    else if((g_j1939_rx_data_pointer[chnl].tail != NULL)
        &&(g_j1939_rx_data_pointer[chnl].head != NULL))
    {
        memcpy(data, &g_j1939_rx_data_pointer[chnl].head->j1939_data,sizeof(J1939_Data));
        memset(&g_j1939_rx_data_pointer[chnl].head->j1939_data, 0 ,sizeof(J1939_Data));
        /*只有一个数据*/
        if (g_j1939_rx_data_pointer[chnl].head == g_j1939_rx_data_pointer[chnl].tail)
        {

            g_j1939_rx_data_pointer[chnl].head = NULL;
            g_j1939_rx_data_pointer[chnl].tail = NULL;
        }
        else
        {
            g_j1939_rx_data_pointer[chnl].head =  g_j1939_rx_data_pointer[chnl].head->p_next;
        }
        ret = true;
    }
    return ret;
}

/*****************************************************************************************************
*Function   :j1939_send（J1939发送函数）
*Description:按照输入的CAN通道和要发送的数据，进行数据头编辑，发送长度设置，然后调用相应的CAN通道将CAN数据发送出去。
             如果发送失败则需要重新初始化CAN。
*Input      :CAN_CHNL       chnl        CAN通道
             J1939_PDU      j1939_pdu   CAN数据
*Output     :
*Returns    :
*Note       :这个函数当前还没想好放在驱动层还是CAN_BUS。
*****************************************************************************************************/
bool j1939_send(CAN_CHNL chnl, J1939_PDU* j1939_pdu)
{
    bool                 ret;        /*定义一个返回值*/
    Can_Rcv_Data         can_data;   /*can数据结构体*/
    
    /*编辑can数据结构体*/
    can_data.id = can_data.id >> 26 | j1939_pdu->p;     /*编辑优先级*/
    can_data.id = can_data.id >> 16 | j1939_pdu->pf;    /*编辑PDU格式*/
	can_data.id = can_data.id >> 8  | j1939_pdu->ps;    /*编辑特定PDU*/
	can_data.id = can_data.id       | j1939_pdu->sa;    /*编辑源地址*/
    can_data.len = j1939_pdu->len;                      /*编辑数据长度*/
    memcpy(can_data.data, &(j1939_pdu->data), 8);       /*编辑要发送的数据*/
    
    if(can_tx_enqueue(chnl, &can_data) == true)
    {
        ret = true;
    }
    else
    {
        ret = false;
    }
    return ret;
}

/*****************************************************************************************************
*Function   :j1939_rx_s_frame_handle（处理J1939接收到的单帧报文）
*Description:将J1939接收到的单帧报文进行内容转换并复制进接收队列头中。
*Input      :CAN_CHNL       chnl        CAN通道
             J1939_PDU      j1939_pdu   CAN数据
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void j1939_rx_s_frame_handle(CAN_CHNL chnl ,J1939_PDU* j1939_pdu)
{
    J1939_Data j1939_data;
    
    j1939_data.p = j1939_pdu->p;
    j1939_data.pgn = j1939_pdu->pf;
    j1939_data.da = j1939_pdu->ps;
    j1939_data.sa = j1939_pdu->sa;
    j1939_data.len = (uint16_t)j1939_pdu->len;
    memcpy(&(j1939_data.data_s), j1939_pdu->data, 8);
    
    J1939_rx_enqueue(chnl, &j1939_data);
}

/*****************************************************************************************************
*Function   :send_j1939_s_frame（发送J1939单帧报文）
*Description:将J1939数据结构转换为CAN_BUS要求的数据结构，然后进行CAN_BUS发送入队操作。
*Input      :CAN_CHNL       chnl        CAN通道
             J1939_PDU      j1939_pdu   CAN数据
*Output     :
*Returns    :
*Note       :对于多帧报文，将数据处理成单帧后最终也是需要调用该函数完成发送入队的。
*****************************************************************************************************/
void send_j1939_s_frame(CAN_CHNL chnl, J1939_PDU* j1939_pdu)
{
    Can_Rcv_Data can_data;
    
    can_data.id = 0x00000000 | ((j1939_pdu->p << 2) << 24);
    can_data.id = can_data.id | (j1939_pdu->pf << 16);
    can_data.id = can_data.id | j1939_pdu->ps << 8;
    can_data.id = can_data.id | j1939_pdu->sa;
    can_data.len = j1939_pdu->len;
    /*这里的打印正常的，暂时注掉*/
//    #ifdef DEBUG_J1939
//        printf("%X\n", can_data.id);
//    #endif
    memcpy(&can_data.data, j1939_pdu->data, 8);
    can_tx_enqueue(chnl, &can_data);
}

/*****************************************************************************************************
*Function   :send_j1939_tp_frame（发送J1939多帧TP传输报文）
*Description:这个函数承接了J1939多帧TP数据的发送任务，包括TPCM和TPDT的数据，需要输入要发送的CAN通道、发送时的TP收发角色、
             要发送的TP类型以及数据来源。
*Input      :CAN_CHNL            chnl        CAN通道
             TP_Role             role        收发角色
             J1939_TP_STATUS     tp_type     TP类型
             stru_j1939_tp_msg*  tp_msg      要发送内容来源的J1939多帧数据结构体指针
*Output     :
*Returns    :
*Note       :TPCM_BAM（多帧广播数据）部分还没有实现，后续有机会会完善。
*****************************************************************************************************/
void send_j1939_tp_frame(CAN_CHNL chnl, TP_Role role, J1939_TP_STATUS tp_type, stru_j1939_tp_msg* tp_msg)
{
    J1939_PDU j1939_pdu;         /*定义一个J1939单帧结构体变量，以便进行后续的数据处理*/
    j1939_pdu.p = TP_PRIORITY;   /*优先级赋值*/
    j1939_pdu.len = TP_LEN;      /*数据长度赋值，J1939多帧TP数据都是8字节长度*/
    
    switch(role)
    {
        /*如果是接收角色，由于数据记录时源地址和目的地址是按照接收时的报文ID记录的，所以回复时要将源地址和目的地址互换*/
        case RX:
            j1939_pdu.ps = tp_msg->sa;
            j1939_pdu.sa = tp_msg->da;
            break;
        case TX:
            j1939_pdu.ps = tp_msg->da;
            j1939_pdu.sa = tp_msg->sa;
            break;
        default:break;
    }
    
    switch(tp_type)
    {
        /*如果是发送TPCM_RTS数据，一般是作为发送角色*/
        case TPCM_RTS:
            j1939_pdu.pf = TPCM_PF;
            j1939_pdu.data[0] = TPCM_CTRL_RTS;
            j1939_pdu.data[1] = 0x00 | tp_msg->total_byte_num;
            j1939_pdu.data[2] = 0x00 | (tp_msg->total_byte_num >> 8);
            j1939_pdu.data[3] = tp_msg->total_pack_num;
            j1939_pdu.data[4] = 0xFF;
            j1939_pdu.data[5] = 0x00 | (tp_msg->pgn >> 16);
            j1939_pdu.data[6] = 0x00 | (tp_msg->pgn >> 8);
            j1939_pdu.data[7] = 0x00 | tp_msg->pgn;
            break;
        /*如果是发送TPCM_CTS数据，一般是作为接收角色*/
        case TPCM_CTS:
            j1939_pdu.pf = TPCM_PF;
            j1939_pdu.data[0] = TPCM_CTRL_CTS;
            j1939_pdu.data[1] = tp_msg->pack_num_per_time;
            j1939_pdu.data[2] = tp_msg->next_pack_num;
            j1939_pdu.data[3] = 0xFF;
            j1939_pdu.data[4] = 0xFF;
            j1939_pdu.data[5] = 0x00 | (tp_msg->pgn >> 16);
            j1939_pdu.data[6] = 0x00 | (tp_msg->pgn >> 8);
            j1939_pdu.data[7] = 0x00 | tp_msg->pgn;
            break;
        /*如果是发送TPDT数据，一般是作为发送角色*/
        case TPDT:
            /*这里增加了一个取余过滤，是为了确保多帧数据传输与CAN_BUS出队频率一致，避免CAN_BUS拥堵而丢帧*/
            if(j1939_tx_tp_msg[chnl].tp_timer[TIMER_TR].timer_num % SEND_INTERVAL)
            {
                return;
            }
            j1939_pdu.pf = TPDT_PF;
            j1939_pdu.data[0] = tp_msg->next_pack_num;
            /*已发送数据+i如果小于总字节数，则将待发送数据域字节数据赋值置报文中，否则填充0xFF*/
            for(int i = 0; i < 7; i++)
            {
                if((tp_msg->next_pack_num-1)*TPDT_DATA_SIZE+i < tp_msg->total_byte_num)
                {
                    j1939_pdu.data[i+1] = tp_msg->m_data_buffer[(tp_msg->next_pack_num-1)*TPDT_DATA_SIZE+i];
                }
                else
                {
                    j1939_pdu.data[i+1] = 0xFF;
                }
            }
            
            j1939_timer_ctrl(chnl, TX, TIMER_TR, RESTART);    /*重置TR计时器*/
            tp_msg->next_pack_num++;                          /*待发送数据包号递增*/
            tp_msg->pack_num_to_be_trans--;                   /*剩余总待发送数据包数量递减*/
            tp_msg->pack_num_to_be_trans_per_time--;          /*本轮剩余待发送数据包数量递减*/
            /*如果本轮剩余待发送数据包数量已经没有了，则将J1939发送状态机调整至TPCM_CTS，待接收方重新请求*/
            if(tp_msg->pack_num_to_be_trans_per_time == 0)     /*这里是不是要增加条件，如果当前不是发送的最后一包
                                                        答复：运行到这里是在J1939发送状态机TPDT状态机内调用运行的，
                                                        调用完该函数后续语句做了该逻辑判断，所以这里不用重复做*/
            {
                tp_msg->tp_status = TPCM_CTS;
            }
            break;
        /*如果是发送TPCM_BAM数据，一般是作为发送角色*/
        case TPCM_BAM:
            j1939_pdu.pf = TPCM_PF;
            j1939_pdu.data[0] = TPCM_CTRL_BAM;
            j1939_pdu.data[1] = 0x00 | (tp_msg->total_byte_num >> 8);
            j1939_pdu.data[2] = 0x00 | tp_msg->total_byte_num;
            j1939_pdu.data[3] = tp_msg->total_pack_num;
            j1939_pdu.data[4] = 0xFF;
            j1939_pdu.data[5] = 0x00 | (tp_msg->pgn >> 16);
            j1939_pdu.data[6] = 0x00 | (tp_msg->pgn >> 8);
            j1939_pdu.data[7] = 0x00 | tp_msg->pgn;
            break;
        /*如果是发送TPCM_ENDACK数据，一般是作为接收角色*/
        case TPCM_ENDACK:
            j1939_pdu.pf = TPCM_PF;
            j1939_pdu.data[0] = TPCM_CTRL_ACK;
            j1939_pdu.data[1] = 0x00 | tp_msg->total_byte_num;
            j1939_pdu.data[2] = 0x00 | (tp_msg->total_byte_num >> 8);
            j1939_pdu.data[3] = tp_msg->total_pack_num;
            j1939_pdu.data[4] = 0xFF;
            j1939_pdu.data[5] = 0x00 | (tp_msg->pgn >> 16);
            j1939_pdu.data[6] = 0x00 | (tp_msg->pgn >> 8);
            j1939_pdu.data[7] = 0x00 | tp_msg->pgn;
            break;
        /*对于请求断开连接的报文，发送角色和接收角色都有可能会触发*/
        case TPCM_ABORT:
            j1939_pdu.pf = TPCM_PF;
            j1939_pdu.data[0] = TPCM_CTRL_ABORT;
            j1939_pdu.data[1] = tp_msg->abort_reason;
            j1939_pdu.data[2] = 0xFF;
            j1939_pdu.data[3] = 0xFF;
            j1939_pdu.data[4] = 0xFF;
            j1939_pdu.data[5] = 0x00 | (tp_msg->pgn >> 16);
            j1939_pdu.data[6] = 0x00 | (tp_msg->pgn >> 8);
            j1939_pdu.data[7] = 0x00 | tp_msg->pgn;
            break;
        default:break;
    }
    
    send_j1939_s_frame(chnl, &j1939_pdu);
}

/*****************************************************************************************************
*Function   :j1939_rx_tpcm_handle（处理J1939接收到的TP.CM报文）
*Description:收到TP.CM报文后，根据报文类型以及运行场景，对其进行处理。
*Input      :CAN_CHNL       chnl        CAN通道
             J1939_PDU      j1939_pdu   CAN数据
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void j1939_rx_tpcm_handle(CAN_CHNL chnl ,J1939_PDU* j1939_pdu)
{
    J1939_PDU tpcm;
    tpcm.ps = j1939_pdu->ps;
    tpcm.sa = j1939_pdu->sa;

    switch(j1939_pdu->data[0])
    {
        /*如果接收到TP.CM RTS报文，先对其内容进行解析，
        如果当前CAN通道多帧接收处于非空闲，则直接回复放弃连接，原因是总线忙
        如果要传输的字节数大于支持传输的最大字节数，则回复放弃连接，原因是资源不足
        如果以上过滤条件通过，则对该报文数据维护到接收多帧数据结构体中，并调整状态机，开始准备接收*/
        case TPCM_CTRL_RTS:
            tpcm.rts.total_byte_num = 0x0000 | j1939_pdu->data[1];
            tpcm.rts.total_byte_num = tpcm.rts.total_byte_num | (j1939_pdu->data[2] << 8);
            tpcm.rts.total_pack_num = j1939_pdu->data[3];
            tpcm.rts.pgn = 0x00000000 | j1939_pdu->data[5] << 16;
            tpcm.rts.pgn = tpcm.rts.pgn | j1939_pdu->data[6] << 8;
            tpcm.rts.pgn = tpcm.rts.pgn | j1939_pdu->data[7];
            if(j1939_rx_tp_msg[chnl].tp_status != IDLE) 
            {
                /*这种对于异常报文的处理，都共用了一个内部全局变量，如果使用局部变量，报文发送可能会有数据异常*/
                tpcm_abort.da = tpcm.ps;
                tpcm_abort.sa = tpcm.sa;
                tpcm_abort.pgn = tpcm.rts.pgn;
                tpcm_abort.abort_reason = BUSY;
                send_j1939_tp_frame(chnl, RX, TPCM_ABORT, &tpcm_abort);
            }
            else if(tpcm.rts.total_byte_num > MAX_J1939_MESSAGE)
            {
                tpcm_abort.da = tpcm.ps;
                tpcm_abort.sa = tpcm.sa;
                tpcm_abort.pgn = tpcm.rts.pgn;
                tpcm_abort.abort_reason = NO_RESOURCES;
                send_j1939_tp_frame(chnl, RX, TPCM_ABORT, &tpcm_abort);
            }
            else
            {
                j1939_rx_tp_msg[chnl].da = tpcm.ps;
                j1939_rx_tp_msg[chnl].sa = tpcm.sa;
                j1939_rx_tp_msg[chnl].tp_status = TPCM_RTS;
                j1939_rx_tp_msg[chnl].total_byte_num = tpcm.rts.total_byte_num;
                j1939_rx_tp_msg[chnl].total_pack_num = tpcm.rts.total_pack_num;
                j1939_rx_tp_msg[chnl].pgn = tpcm.rts.pgn;
            }
            break;
        /*如果接收到TP.CM CTS报文，先对其进行数据解析
        然后要对这个CTS报文进行校验，确保其是当前正在进行传输的对象，包括PGN和源地址
        当前工作状态是否处于正在准备接收CTS阶段，*/
        /*这里要注意调试一下，如果接收方出现了丢帧，然后要求对过去的数据进行重发*/
        case TPCM_CTRL_CTS:
            /*j1939_pdu->data[1]含义是接下来可接收的数据包数量*/
            tpcm.cts.pack_num_to_be_trans = j1939_pdu->data[1];
            tpcm.cts.next_pack_num = j1939_pdu->data[2];
            tpcm.cts.pgn = 0x00000000 | j1939_pdu->data[5] << 16;
            tpcm.cts.pgn = tpcm.cts.pgn | j1939_pdu->data[6] << 8;
            tpcm.cts.pgn = tpcm.cts.pgn | j1939_pdu->data[7];
            if(j1939_tx_tp_msg[chnl].tp_status == TPCM_CTS
                && j1939_tx_tp_msg[chnl].pgn == tpcm.cts.pgn
                && tpcm.sa == j1939_tx_tp_msg[chnl].da
                && j1939_tx_tp_msg[chnl].next_pack_num == tpcm.cts.next_pack_num)
            {
                j1939_tx_tp_msg[chnl].pack_num_to_be_trans_per_time = tpcm.cts.pack_num_to_be_trans;
                j1939_tx_tp_msg[chnl].tp_status = TPDT;
            }
            /*如果CTS请求的包号不等于发送方待发送的包号，则认为是丢帧请求重发，先识别重发次数是否超限，
            如果是，则放弃连接，原因为重连次数超限，如果没超限，则重现按照请求重发的包号调整待发送包数和
            待发送包号，进入TPDT阶段进行重发数据*/
            else if(j1939_tx_tp_msg[chnl].pgn == tpcm.cts.pgn
                    && tpcm.sa == j1939_tx_tp_msg[chnl].da
                    && j1939_tx_tp_msg[chnl].next_pack_num != tpcm.cts.next_pack_num
                    && tpcm.cts.pack_num_to_be_trans != 0)
            {
                if(j1939_tx_tp_msg[chnl].tpdt_retrans_num > TPDT_RETRANS_MAX)
                {
                    j1939_tx_tp_msg[chnl].tp_status = TPCM_ABORT;
                    j1939_tx_tp_msg[chnl].abort_reason = RETRANS_EXCEEDED;
                }
                else
                {
                    j1939_tx_tp_msg[chnl].pack_num_to_be_trans = (tpcm.cts.next_pack_num - j1939_tx_tp_msg[chnl].next_pack_num) + j1939_tx_tp_msg[chnl].pack_num_to_be_trans;
                    j1939_tx_tp_msg[chnl].next_pack_num = tpcm.cts.next_pack_num;
                    j1939_tx_tp_msg[chnl].pack_num_to_be_trans_per_time = tpcm.cts.pack_num_to_be_trans;
                    j1939_tx_tp_msg[chnl].tp_status = TPDT;
                    j1939_tx_tp_msg[chnl].tpdt_retrans_num ++;
                }
            }
            /*如果CTS可接收数据包数量为0且待传输数据包号为0xFF，且当前不是处于数据包全部发送完成的ENDACK阶段，
            则说明刚接收到一轮完整数据还在处理，
            暂时不能继续接收，则保持在TPCM_CTS阶段等待，并启动T4定时器*/
            else if(j1939_tx_tp_msg[chnl].pgn == tpcm.cts.pgn
                    && tpcm.sa == j1939_tx_tp_msg[chnl].da
                    && tpcm.cts.next_pack_num == 0xFF
                    && tpcm.cts.pack_num_to_be_trans == 0
                    && j1939_tx_tp_msg[chnl].tp_status != TPCM_ENDACK)
            {
                j1939_tx_tp_msg[chnl].tp_status = TPCM_CTS;
                if(j1939_tx_tp_msg[chnl].tp_timer[TIMER_T4].status == T_DISABLE)
                {
                    j1939_timer_ctrl(chnl, TX, TIMER_T4, RESTART);
                }
            }
            break;
        /*如果接收到TP.EndofMsgACK报文，先对其进行数据解析
        如果当前CAN通道多帧发送状态机为ENDACK，且PGN、源地址都吻合，则就完成了本次多帧数据传输，清空多帧发送结构体，状态机复位*/
        case TPCM_CTRL_ACK:
            tpcm.ack.total_byte_num = 0x0000 | j1939_pdu->data[1];
            tpcm.ack.total_byte_num = tpcm.ack.total_byte_num | j1939_pdu->data[2] << 8;
            tpcm.ack.total_pack_num = j1939_pdu->data[3];
            tpcm.ack.pgn = 0x00000000 | j1939_pdu->data[5] << 16;
            tpcm.ack.pgn = tpcm.ack.pgn | j1939_pdu->data[6] << 8;
            tpcm.ack.pgn = tpcm.ack.pgn | j1939_pdu->data[7];
            if(j1939_tx_tp_msg[chnl].tp_status == TPCM_ENDACK
                && j1939_tx_tp_msg[chnl].pgn == tpcm.ack.pgn
                && tpcm.sa == j1939_tx_tp_msg[chnl].da)
            {
                memset(&j1939_tx_tp_msg[chnl], 0, sizeof(stru_j1939_tp_msg));
                j1939_tx_tp_msg[chnl].tp_status = IDLE;
            }
            break;
        /*如果接收到TP.CM CTS报文，先对其进行数据解析，然后再区分是接收方放弃连接还是发送方放弃链接
        如果是发送方数据发送请求被以总线忙的原因拒绝，则要过段时间尝试重新连接*/
        case TPCM_CTRL_ABORT:    /*总线上的指定的目标地址和源地址之间同时仅能有一个多帧报文，这个好像还做不到，一个总线上收发同时仅能支持一个*/
            tpcm.abort.pgn = 0x00000000 | j1939_pdu->data[5] << 16;
            tpcm.abort.pgn = tpcm.abort.pgn | j1939_pdu->data[6] << 8;
            tpcm.abort.pgn = tpcm.abort.pgn | j1939_pdu->data[7];
            /*作为数据接收角色收到放弃连接*/
            if(j1939_rx_tp_msg[chnl].pgn == tpcm.abort.pgn
               && j1939_rx_tp_msg[chnl].sa == tpcm.sa)
            {
                memset(&j1939_rx_tp_msg[chnl], 0, sizeof(stru_j1939_tp_msg));
                j1939_rx_tp_msg[chnl].tp_status = IDLE;
            }
            /*作为数据发送角色收到放弃连接*/
            else if(j1939_tx_tp_msg[chnl].pgn == tpcm.abort.pgn
                    && j1939_tx_tp_msg[chnl].da == tpcm.sa)
            {
                /*如果是发送完RTS被拒绝原因为目标繁忙，则开启Th计时器，每Th周期尝试重连1次*/
                if(j1939_tx_tp_msg[chnl].tp_status == TPCM_CTS
                    && j1939_tx_tp_msg[chnl].next_pack_num == 1
                    && j1939_pdu->data[1] == BUSY)
                {
                    /*等Th（500ms）时间后再发一遍RTS*/
                    j1939_timer_ctrl(chnl, TX, TIMER_TH, RESTART);
                }
                /*其他情况则接受断连请求，放弃本次数据发送*/
                else
                {
                    memset(&j1939_tx_tp_msg[chnl], 0, sizeof(stru_j1939_tp_msg));
                    j1939_tx_tp_msg[chnl].tp_status = IDLE;
                }
            }
            break;
        case TPCM_CTRL_BAM:
            break;
        /*如果PGN是TP.CM，但是首字节又不在J1939协议范围，则对其回复放弃连接，原因为未知CTS*/
        default:
            {
            tpcm_abort.da = j1939_pdu->ps;
            tpcm_abort.sa = j1939_pdu->sa;
            tpcm_abort.pgn = 0x00000000 | j1939_pdu->data[5] << 16;
            tpcm_abort.pgn = tpcm_abort.pgn | j1939_pdu->data[6] << 8;
            tpcm_abort.pgn = tpcm_abort.pgn | j1939_pdu->data[7];
            tpcm_abort.abort_reason = UNEXPECTED_CTS;
            send_j1939_tp_frame(chnl, RX, TPCM_ABORT, &tpcm_abort);
            }
            break;
    }
}

/*****************************************************************************************************
*Function   :j1939_rx_tpdt_handle（处理J1939接收到的TP.DT报文）
*Description:先对接收到的数据进行校验，如果当前多帧接收状态机不在TPDT，或者地址不对应，则回复断开连接；
             如果发送的数据包号与待接收的数据包号不一致，则先看重传次数是否超限，如是则回复断开连接，原因为重传
             次数超限，否则重新请求待接收的数据包。
             如果以上数据校验通过，则开始将TP.DT数据复制到多帧数据buffer内，如果当前传输的数据包达到了本轮可
             传输的数据包数量，则状态机跳转回TPCM_CTS，开始新一轮传输。
*Input      :CAN_CHNL       chnl        CAN通道
             J1939_PDU      j1939_pdu   CAN数据
*Output     :
*Returns    :
*Note       :对于重传，其实是有风险的，因为目前数据处理都是队列形式，当前处理异常数据的时候，不确定队列里还有
             多少异常数据，等把重传数据塞入队列发出去，然后对方收到处理这个过程中，都不知道会有多少异常数据会接收到；
*****************************************************************************************************/
void j1939_rx_tpdt_handle(CAN_CHNL chnl ,J1939_PDU* j1939_pdu)
{
    /*接收到的这些数据如果回复断开连接，会不会有问题其实需要思考一下*/
    if(j1939_rx_tp_msg[chnl].tp_status != TPDT
       || j1939_rx_tp_msg[chnl].sa != j1939_pdu->sa)
    {
        tpcm_abort.da = j1939_pdu->ps;
        tpcm_abort.sa = j1939_pdu->sa;
        tpcm_abort.pgn = 0x00000000;
        tpcm_abort.abort_reason = UNEXPECTED_CTS;
        send_j1939_tp_frame(chnl, RX, TPCM_ABORT, &tpcm_abort);
    }
    /*如果接收的数据包号不是待传输包，则触发TPCM CTS报文*/
    else if(j1939_pdu->data[0] != j1939_rx_tp_msg[chnl].next_pack_num)
    {
        if(j1939_rx_tp_msg[chnl].tpdt_retrans_num == TPDT_RETRANS_MAX)
        {
            j1939_rx_tp_msg[chnl].tp_status = TPCM_ABORT;
            j1939_rx_tp_msg[chnl].abort_reason = RETRANS_EXCEEDED;
        }
        else
        {
            j1939_rx_tp_msg[chnl].tp_timer[TIMER_TH].timeout_flag = 1;/*按照TH超时处理重发TPCM CTS*/
            j1939_rx_tp_msg[chnl].tpdt_retrans_num++;
        }
    }
    else
    {
        /*假如有重传情况出现，数据链接成功后重置重传次数和恢复数据传输状态机*/
        j1939_rx_tp_msg[chnl].tpdt_retrans_num = 0;
        j1939_rx_tp_msg[chnl].tp_status = TPDT;
        
        j1939_timer_ctrl(chnl, RX, TIMER_T2, STOP);
        j1939_timer_ctrl(chnl, RX, TIMER_T1, RESTART);
        for(int i = 0; i < TPDT_DATA_SIZE; i++)
        {
            if((j1939_rx_tp_msg[chnl].next_pack_num-1)*TPDT_DATA_SIZE+i < j1939_rx_tp_msg[chnl].total_byte_num)
            {
                j1939_rx_tp_msg[chnl].m_data_buffer[(j1939_rx_tp_msg[chnl].next_pack_num-1)*TPDT_DATA_SIZE+i] = j1939_pdu->data[i+1];
            }
        }
        j1939_rx_tp_msg[chnl].pack_num_per_time--;
        j1939_rx_tp_msg[chnl].pack_num_to_be_trans--;
        if((j1939_rx_tp_msg[chnl].pack_num_per_time) == 0
            && j1939_rx_tp_msg[chnl].next_pack_num != j1939_rx_tp_msg[chnl].total_pack_num)
        {
            /*这里要加一个TPCM_CTS报文，J1939协议其实是想通过这帧报文留给接收方接收完一轮数据后有一定的数据处理时间，本程序在这里象征性的发一下*/
            stru_j1939_tp_msg tpcm_cts;
            tpcm_cts.da = j1939_rx_tp_msg[chnl].da;
            tpcm_cts.sa = j1939_rx_tp_msg[chnl].sa;
            tpcm_cts.pgn = j1939_rx_tp_msg[chnl].pgn;
            tpcm_cts.pack_num_per_time = j1939_rx_tp_msg[chnl].pack_num_per_time;
            tpcm_cts.next_pack_num = 0xFF;
            send_j1939_tp_frame(chnl, RX, TPCM_CTS, &tpcm_cts);
            j1939_rx_tp_msg[chnl].tp_status = TPCM_CTS;
        }
        j1939_rx_tp_msg[chnl].next_pack_num++;
    }
}

/*****************************************************************************************************
*Function   :j1939_rx_m_frame_handle（处理J1939接收到的多帧报文）
*Description:分流接收到的PDU1报文，如果是TP.CM，就调用TP.CM处理函数；如果是TPDT，就调用TPDT处理函数；
             如果都不是，则按单帧报文处理。
*Input      :CAN_CHNL       chnl        CAN通道
             J1939_PDU      j1939_pdu   CAN数据
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void j1939_rx_m_frame_handle(CAN_CHNL chnl ,J1939_PDU* j1939_pdu)
{
    switch(j1939_pdu->pf)
    {
        case TPCM_PF:
            j1939_rx_tpcm_handle(chnl ,j1939_pdu);
            break;
        case TPDT_PF:
            j1939_rx_tpdt_handle(chnl ,j1939_pdu);
            break;
        default:
            j1939_rx_s_frame_handle(chnl ,j1939_pdu);
            break;
    }
}

/*****************************************************************************************************
*Function   :j1939_rx_handle（J1939数据接收处理）
*Description:对每个CAN通道进行轮询处理，
             先判断当前J1939数据接收队列是否已经满了，如果是，则暂不进行CAN_BUS接收出队。
             对CAN_BUS队列出队报文进行分流，如果PF大于等于240，属于J1939协议规定的PDU2范畴，暂未编写对应的代码；
             如果PF大于等于232，属于J1939协议规定的PDU1范畴，则按照多帧数据交互进行处理；
             其他的默认按单帧报文进行处理。
             处理后的数据，最终会进行J1939数据接收入队。
*Input      :
*Output     :
*Returns    :
*Note       :PDU2暂未完善，后续有机会再完善吧
*****************************************************************************************************/
void j1939_rx_handle(void)
{
    CAN_CHNL      chnl;
    Can_Rcv_Data  can_data;
    J1939_PDU     j1939_pdu;
    
    for(chnl = CAN_DATA_QUEUE_CHNL_1; chnl < CAN_DATA_CHNL_NUM_MAX; chnl++)
    {
        if ((g_j1939_rx_data_pointer[chnl].tail != NULL)
             &&(g_j1939_rx_data_pointer[chnl].head != NULL)
             &&(g_j1939_rx_data_pointer[chnl].tail->p_next == g_j1939_rx_data_pointer[chnl].head)) 
         {
             /*发送错误处理*/
             #ifdef DEBUG_J1939
             printf("J1939 de dui lie man le!\n");
             #endif
         }
         else if(can_rx_dequeue(chnl, &can_data) == true)
         {
             j1939_pdu.p  = (can_data.id & 0xFF000000) >> 26;
             j1939_pdu.pf = (can_data.id & 0x00FF0000) >> 16;
             j1939_pdu.ps = (can_data.id & 0x0000FF00) >> 8;
             j1939_pdu.sa = can_data.id & 0x000000FF;
             memcpy(&(j1939_pdu.data), can_data.data, 8);
             if(j1939_pdu.pf >= 240)
            {
                /*PDU2*/
            }
            else if(j1939_pdu.pf >= 232)  /*PDU1*/
            {
                /*J1939协议处理*/
                j1939_rx_m_frame_handle(chnl ,&j1939_pdu);
            }
            else
            {
                /*普通单帧处理*/
                j1939_pdu.len = 8;
                j1939_rx_s_frame_handle(chnl ,&j1939_pdu);
            }
         }
    }
}



/*****************************************************************************************************
*Function   :j1939_tx_queue_init(CAN发送队列链表初始化)
*Description:先判断输入的CAN通道是否为有效CAN通道，然后清空该CAN通道对应的接收结构体数组数据，将数组连接为单向环形链表
*Input      :CAN_CHNL chnl   CAN通道
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void j1939_tx_queue_init(CAN_CHNL chnl)
{
    if (chnl < CAN_DATA_CHNL_NUM_MAX)
    {
        memset(g_j1939_tx_data_queue[chnl], 0 ,sizeof(g_j1939_tx_data_queue[chnl]));

        for (int i = 0; i < CAN_DATA_QUEUE_NUM; i++)
        {
            /*如果i是数组最后一个，则连接向数组首地址，以形成环形链表*/
            if (i == J1939_DATA_QUEUE_NUM-1)
            {
                g_j1939_tx_data_queue[chnl][i].p_next = &g_j1939_tx_data_queue[chnl][0];
            }
            else
            {
                g_j1939_tx_data_queue[chnl][i].p_next = &g_j1939_tx_data_queue[chnl][i+1];
            }
        }
    }
}

/*****************************************************************************************************
*Function   :J1939_tx_enqueue（J1939发送入队函数）
*Description:先检查输入的CAN通道和输入的数据指针是否非法，如果有异常直接报错；
             然后判断CAN数据是单帧数据还是多帧数据，如果是单帧报文，则直接处理后进入CAN_BUS发送入队；
             如果是多帧报文，则要先判断J1939多帧发送队列是否为空队列，如果是则先进行队列初始化，然后将
             多帧数据复制到队列头；
             如果队列非空，则先判断队列是否满了，如果满了也要报错，如果没满，则将链表尾后移，然后将数据
             复制到队列尾。
*Input      :CAN_CHNL       chnl   CAN通道
             Can_Rcv_Data*  data   CAN接收数据结构体
*Output     :
*Returns    :bool值         入队结果（true成功/false失败）
*Note       :
*****************************************************************************************************/
bool J1939_tx_enqueue(CAN_CHNL chnl, J1939_Data* data)
{
    bool ret;
    if(chnl >= CAN_DATA_CHNL_NUM_MAX)
    {
        return false;
    }
    else if(data == NULL)
    {
        return false;
    }
    else if(data->len <= 8)    /*如果是单帧报文，直接处理后发送到CAN_BUS*/
    {
        J1939_PDU j1939_pdu;
        j1939_pdu.p = data->p;
        j1939_pdu.pf = data->pgn;
        j1939_pdu.ps = data->da;
        j1939_pdu.sa = data->sa;
        j1939_pdu.len = data->len;
        memcpy(j1939_pdu.data, data->data_s, 8);
        send_j1939_s_frame(chnl, &j1939_pdu);
        ret = true;
    }
    else
    {
        if((g_j1939_tx_data_pointer[chnl].tail == NULL)
            ||(g_j1939_tx_data_pointer[chnl].head == NULL))
        {
            /*空队列*/
            g_j1939_tx_data_pointer[chnl].tail = &g_j1939_tx_data_queue[chnl][0];
            g_j1939_tx_data_pointer[chnl].head = &g_j1939_tx_data_queue[chnl][0];
            if (g_j1939_tx_data_pointer[chnl].tail->p_next == NULL)
            {
                j1939_tx_queue_init(chnl);
            }
            
            memcpy(&g_j1939_tx_data_queue[chnl][0].j1939_data, data, sizeof(J1939_Data));
            ret = true;
        }
        else if((g_j1939_tx_data_pointer[chnl].tail != NULL)
            &&(g_j1939_tx_data_pointer[chnl].head != NULL))
        {
            /*队列满了*/
            if (g_j1939_tx_data_pointer[chnl].tail->p_next == g_j1939_tx_data_pointer[chnl].head) 
            {
                ret = false;
            }
            else
            {
                g_j1939_tx_data_pointer[chnl].tail = g_j1939_tx_data_pointer[chnl].tail->p_next;
                memcpy(&(g_j1939_tx_data_pointer[chnl].tail->j1939_data), data, sizeof(J1939_Data));
                ret = true;
            }
        }
        else
        {
            ret = false;
        }
    }

    return ret;
}

/*****************************************************************************************************
*Function   :J1939_tx_dequeue（J1939发送出队函数）
*Description:依次处理每个CAN通道队列，
             如果该CAN通道当前多帧工作状态为非空闲，则跳过当前CAN通道；
             如果队列为空，则跳过当前CAN通道；
             如果队列非空，则再判断当前是否只有一组数据，如果是，则出队后清空队列头尾指针；如不是，则正常出队。
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void J1939_tx_dequeue(void)
{
    CAN_CHNL chnl;
    for(chnl = CAN_DATA_QUEUE_CHNL_1; chnl < CAN_DATA_CHNL_NUM_MAX; chnl++)
    {
        if(j1939_tx_tp_msg[chnl].tp_status != IDLE)
        {
            continue;
        }
        else if ((g_j1939_tx_data_pointer[chnl].tail == NULL)
                ||(g_j1939_tx_data_pointer[chnl].head == NULL))   /*空队列*/
        {
            continue;
        }
        else if((g_j1939_tx_data_pointer[chnl].tail != NULL)
            &&(g_j1939_tx_data_pointer[chnl].head != NULL))
        {
            j1939_tx_tp_msg[chnl].sa = g_j1939_tx_data_pointer[chnl].head->j1939_data.sa;
            j1939_tx_tp_msg[chnl].da = g_j1939_tx_data_pointer[chnl].head->j1939_data.da;
            j1939_tx_tp_msg[chnl].pgn = (uint32_t)(g_j1939_tx_data_pointer[chnl].head->j1939_data.pgn << 8);
            j1939_tx_tp_msg[chnl].total_byte_num = g_j1939_tx_data_pointer[chnl].head->j1939_data.len;
            j1939_tx_tp_msg[chnl].total_pack_num = (j1939_tx_tp_msg[chnl].total_byte_num-1)/TPDT_DATA_SIZE + 1;
            j1939_tx_tp_msg[chnl].next_pack_num = 1;
            memcpy(&(j1939_tx_tp_msg[chnl].m_data_buffer), g_j1939_tx_data_pointer[chnl].head->j1939_data.data_m, MAX_J1939_MESSAGE);
            j1939_tx_tp_msg[chnl].tp_status = TPCM_RTS;
            /*这里一定要清空，不然后续数据可能会出现异常*/
            memset(&g_j1939_tx_data_pointer[chnl].head->j1939_data, 0 ,sizeof(J1939_Data));
            
            /*只有一个数据*/
            if (g_j1939_tx_data_pointer[chnl].head == g_j1939_tx_data_pointer[chnl].tail)
            {
                g_j1939_tx_data_pointer[chnl].head = NULL;
                g_j1939_tx_data_pointer[chnl].tail = NULL;
            }
            else
            {
                g_j1939_tx_data_pointer[chnl].head =  g_j1939_tx_data_pointer[chnl].head->p_next;
            }
        }
    }
}

/*****************************************************************************************************
*Function   :j1939_tp_poll(J1939 TP数据传输处理)
*Description:本函数包含了接收和发送TP数据的状态机，依次轮询处理每个CAN通道的接收和发送TP状态。
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void j1939_tp_poll(void)
{
    CAN_CHNL chnl;
    for(chnl = CAN_DATA_QUEUE_CHNL_1; chnl < CAN_DATA_CHNL_NUM_MAX; chnl++)
    {
        /*以下这个switch是处理多帧接收数据的状态机*/
        switch(j1939_rx_tp_msg[chnl].tp_status)
        {
            /*如果当前是空闲，则直接跳出状态机*/
            case IDLE:
                break;
            /*如果当前处于TP.CM RTS阶段，则说明CAN通道收到了一个TP.CM RTS报文，按照J1939协议栈多帧
            交互流程，打开Tr计时器，下一个将要接收的数据包编号赋值为1，待接收的数据包数量赋值为数据包
            总数量，然后将状态机跳转至TPCM_CTS*/
            case TPCM_RTS:
                if(j1939_rx_tp_msg[chnl].tp_timer[TIMER_TR].status == T_DISABLE)
                {
                    j1939_timer_ctrl(chnl, RX, TIMER_TR, RESTART);
                }
               
                j1939_rx_tp_msg[chnl].next_pack_num = 1;
                j1939_rx_tp_msg[chnl].pack_num_to_be_trans = j1939_rx_tp_msg[chnl].total_pack_num;
                j1939_rx_tp_msg[chnl].tp_status = TPCM_CTS;
                break;
            /*如果当前处于TP.CM CTS阶段，则说明需要向CAN通道发送一帧TP.CM CTS报文，按照J1939协议栈
                多帧交互流程，先看下Tr计时器是否超时，如果是则应该进入放弃连接，原因为超时，如果没
                超时，则发送TP.CM CTS报文，然后关闭Tr计时器，打开T2计时器，然后将状态机跳转至TPDT*/
            case TPCM_CTS:
                if(j1939_rx_tp_msg[chnl].tp_timer[TIMER_TR].timeout_flag == 1)
                {
                    j1939_rx_tp_msg[chnl].tp_status = TPCM_ABORT;
                    j1939_rx_tp_msg[chnl].abort_reason = TIMEOUT;
                    j1939_timer_ctrl(chnl, RX, TIMER_TR, STOP);
                }
                else
                {
                    j1939_timer_ctrl(chnl, RX, TIMER_TR, STOP);
                    if(j1939_rx_tp_msg[chnl].pack_num_to_be_trans > MAX_PACK_PER_TIME)
                    {
                        j1939_rx_tp_msg[chnl].pack_num_per_time = MAX_PACK_PER_TIME;
                    }
                    else
                    {
                        j1939_rx_tp_msg[chnl].pack_num_per_time = j1939_rx_tp_msg[chnl].pack_num_to_be_trans;
                    }
                    send_j1939_tp_frame(chnl, RX, TPCM_CTS, &j1939_rx_tp_msg[chnl]);
                    if(j1939_rx_tp_msg[chnl].tp_timer[TIMER_T2].status == T_DISABLE)
                    {
                        j1939_timer_ctrl(chnl, RX, TIMER_T2, RESTART);
                    }
                    j1939_rx_tp_msg[chnl].tp_status = TPDT;
                }
                break;
            /*暂时没有对TP.CM BAM进行支持*/
            case TPCM_BAM:
                break;
            /*如果当前处于TP.DT阶段，则说明CAN通道处于接收TP.DT数据状态，检擦T2或者T1计时器是否超时，
            如果是则进入放弃连接，原因为超时，如果没有，则打开Th和T1计时器；
            如果Th计时器超时，则发送一帧TP.CM CTS报文以保持数据传输；
            如果当前待接收数据包编号大于总包数了，说明数据接收结束，打开Tr计时器，并将
            状态机跳转至TPCM_ENDACK*/
            case TPDT:
                if(j1939_rx_tp_msg[chnl].tp_timer[TIMER_T2].timeout_flag == 1
                    || j1939_rx_tp_msg[chnl].tp_timer[TIMER_T1].timeout_flag == 1)
                {
                    j1939_rx_tp_msg[chnl].tp_status = TPCM_ABORT;
                    j1939_rx_tp_msg[chnl].abort_reason = TIMEOUT;
                    j1939_timer_ctrl(chnl, RX, TIMER_T2, STOP);
                }
                else
                {
                    if(j1939_rx_tp_msg[chnl].tp_timer[TIMER_TH].status == T_DISABLE)
                    {
                        j1939_timer_ctrl(chnl, RX, TIMER_TH, RESTART);
                    }
                    if(j1939_rx_tp_msg[chnl].tp_timer[TIMER_T1].status == T_DISABLE)
                    {
                        j1939_timer_ctrl(chnl, RX, TIMER_T1, RESTART);
                    }
                    
                    if(j1939_rx_tp_msg[chnl].tp_timer[TIMER_TH].timeout_flag == 1)
                    {
                        send_j1939_tp_frame(chnl, RX, TPCM_CTS, &j1939_rx_tp_msg[chnl]);
                        j1939_timer_ctrl(chnl, RX, TIMER_TH, RESTART);
                    }

                    if(j1939_rx_tp_msg[chnl].next_pack_num > j1939_rx_tp_msg[chnl].total_pack_num)
                    {
                        j1939_rx_tp_msg[chnl].tp_status = TPCM_ENDACK;
                        j1939_timer_ctrl(chnl, RX, TIMER_TH, STOP);
                        j1939_timer_ctrl(chnl, RX, TIMER_TR, RESTART);
                    }
                }
                break;
            /*如果当前处于TP.CM EndofMsgACK阶段，说明数据传输完成，需要向CAN通道发送一帧
            TP.CM EndofMsgACK报文，先判断Tr计时器是否超时，如果是，则进入放弃连接，原因为超时，
            如果没有，则发送一帧TP.CM EndofMsgACK报文，将接收到的数据整理后进行J1939接收入队
            清空数据处理区，状态机跳转至IDLE*/
            case TPCM_ENDACK:
                if(j1939_rx_tp_msg[chnl].tp_timer[TIMER_TR].timeout_flag == 1)
                {
                    j1939_rx_tp_msg[chnl].tp_status = TPCM_ABORT;
                    j1939_rx_tp_msg[chnl].abort_reason = TIMEOUT;
                    j1939_timer_ctrl(chnl, RX, TIMER_TR, STOP);
                }
                else
                {
                    send_j1939_tp_frame(chnl, RX, TPCM_ENDACK, &j1939_rx_tp_msg[chnl]);
                    /*最后将数据转移至J1939接收队列*/
                    J1939_Data j1939_m_data;
                    j1939_m_data.p = TP_PRIORITY;
                    j1939_m_data.pgn = j1939_rx_tp_msg[chnl].pgn >> 8;
                    j1939_m_data.da = j1939_rx_tp_msg[chnl].da;
                    j1939_m_data.sa = j1939_rx_tp_msg[chnl].sa;
                    j1939_m_data.len = j1939_rx_tp_msg[chnl].total_byte_num;
                    memcpy(j1939_m_data.data_s, j1939_rx_tp_msg[chnl].m_data_buffer, MAX_J1939_MESSAGE);
                    J1939_rx_enqueue(chnl, &j1939_m_data);
                    
                    memset(&j1939_rx_tp_msg[chnl], 0, sizeof(stru_j1939_tp_msg));
                    j1939_rx_tp_msg[chnl].tp_status = IDLE;
                }
                break;
            /*如果当前处于TP.CM Abort阶段，说明有异常，需要放弃连接，发送一帧TP.CM Abort报文，
            清空数据处理区，状态机跳转至IDLE*/
            case TPCM_ABORT:
                send_j1939_tp_frame(chnl, RX, TPCM_ABORT, &j1939_rx_tp_msg[chnl]);
                memset(&j1939_rx_tp_msg[chnl], 0, sizeof(stru_j1939_tp_msg));
                j1939_rx_tp_msg[chnl].tp_status = IDLE;
                break;
            default:break;
        }
        
        /*以下这个switch是处理多帧发送数据的状态机*/
        switch(j1939_tx_tp_msg[chnl].tp_status)
        {
            /*如果当前是空闲，则直接跳出状态机*/
            case IDLE:
                break;
            /*如果当前处于TP.CM RTS阶段，则说明需要向CAN通道发送一个TP.CM RTS报文，按照J1939协议栈多帧
            交互流程，向CAN通道发送一个TP.CM RTS报文，打开T3计时器，然后将状态机跳转至TPCM_CTS*/
            case TPCM_RTS:
                send_j1939_tp_frame(chnl, TX, TPCM_RTS, &j1939_tx_tp_msg[chnl]);
                j1939_timer_ctrl(chnl, TX, TIMER_T3, RESTART);
                j1939_tx_tp_msg[chnl].tp_status = TPCM_CTS;
                break;
            /*如果当前处于TP.CM RTS阶段，则说明CAN通道正在等待接收TP.CM CTS报文（当轮首帧），
            判断T3和T4计时器是否超时，如果是则进入放弃连接，原因为超时，
            如果对方回复的CTS为繁忙，则会开启Th计时器，待Th计时器超时后重新触发发送RTS报文*/
            case TPCM_CTS:
                if(j1939_tx_tp_msg[chnl].tp_timer[TIMER_T3].timeout_flag == 1
                    || j1939_tx_tp_msg[chnl].tp_timer[TIMER_T4].timeout_flag == 1)
                {
                    j1939_tx_tp_msg[chnl].tp_status = TPCM_ABORT;
                    j1939_tx_tp_msg[chnl].abort_reason = TIMEOUT;
                    j1939_timer_ctrl(chnl, TX, TIMER_T3, STOP);
                    return;
                }
                /*这里对应TPDT发送过程重返TPCM_CTS，关闭Tr计时器*/
                j1939_timer_ctrl(chnl, TX, TIMER_TR, STOP);
                
                /*这里对应发送RTS后对方回复BUSY，Th时间后重新发送RTS尝试连接*/
                if(j1939_tx_tp_msg[chnl].tp_timer[TIMER_TH].timeout_flag == 1)
                {
                    send_j1939_tp_frame(chnl, TX, TPCM_RTS, &j1939_tx_tp_msg[chnl]);
                    j1939_timer_ctrl(chnl, TX, TIMER_TH, RESTART);
                }
                break;
            /*暂时没有对TP.CM BAM进行支持*/
            case TPCM_BAM:
                break;
            /*如果当前处于TP.DT阶段，则说明CAN通道进入了数据域发送阶段，
            关闭T3和T4计时器，打开Tr计时器，Tr计时器已经打开的情况下，则判断Tr是否超时，如果超时，则
            放弃连接，原因为超时，如果都正常，则发送数据域数据，
            发送过数据后，如果判断待发送的数据包号已经超过了总包数，则说明数据发完了，状态机跳转到TPCM_ENDACK*/
            case TPDT:
                j1939_timer_ctrl(chnl, TX, TIMER_T3, STOP);
                j1939_timer_ctrl(chnl, TX, TIMER_T4, STOP);
                if(j1939_tx_tp_msg[chnl].tp_timer[TIMER_TR].status == T_DISABLE)
                {
                    j1939_timer_ctrl(chnl, TX, TIMER_TR, RESTART);
                }
                else if(j1939_tx_tp_msg[chnl].tp_timer[TIMER_TR].timeout_flag == 1)
                {
                    j1939_tx_tp_msg[chnl].tp_status = TPCM_ABORT;
                    j1939_tx_tp_msg[chnl].abort_reason = TIMEOUT;
                    j1939_timer_ctrl(chnl, TX, TIMER_TR, STOP);
                    return;
                }
                send_j1939_tp_frame(chnl, TX, TPDT, &j1939_tx_tp_msg[chnl]);
                if(j1939_tx_tp_msg[chnl].next_pack_num > j1939_tx_tp_msg[chnl].total_pack_num)
                {
                    j1939_tx_tp_msg[chnl].tp_status = TPCM_ENDACK;
                }
                break;
            /*如果当前处于TP.EndofMsgACK阶段，说明数据包传输完成，等待接收TP.EndofMsgACK报文，
            关闭Tr计时器，打开T3计时器，如果T3计时器已打开则判断T3是否超时，如果超时则放弃连接，
            原因为超时。*/
            case TPCM_ENDACK:
                j1939_timer_ctrl(chnl, TX, TIMER_TR, STOP);
                if(j1939_tx_tp_msg[chnl].tp_timer[TIMER_T3].status == T_DISABLE)
                {
                    j1939_timer_ctrl(chnl, TX, TIMER_T3, RESTART);
                }
                else if(j1939_tx_tp_msg[chnl].tp_timer[TIMER_T3].timeout_flag == 1)
                {
                    j1939_tx_tp_msg[chnl].tp_status = TPCM_ABORT;
                    j1939_tx_tp_msg[chnl].abort_reason = TIMEOUT;
                    j1939_timer_ctrl(chnl, TX, TIMER_T3, STOP);
                }
                break;
            /*如果当前处于TP.CM Abort阶段，说明有异常，需要放弃连接，发送一帧TP.CM Abort报文，
            清空数据处理区，状态机跳转至IDLE*/
            case TPCM_ABORT:
                send_j1939_tp_frame(chnl, TX, TPCM_ABORT, &j1939_tx_tp_msg[chnl]);
                memset(&j1939_tx_tp_msg[chnl], 0, sizeof(stru_j1939_tp_msg));
                j1939_tx_tp_msg[chnl].tp_status = IDLE;
                break;
            default:break;
        }
    }
}

/*****************************************************************************************************
*Function   :J1939_handle_task（J1939处理任务）
*Description:CAN_BUS发送出队；J1939定时器运行；J1939接收处理；J1939发送出队；J1939 TP传输数据处理
*Input      :
*Output     :
*Returns    :
*Note       :对任务周期要求为1ms，单独创建任务并引用为佳
*****************************************************************************************************/
void J1939_handle_task(void)
{
    can_tx_dequeue();
    j1939_timer();
    j1939_rx_handle();
    J1939_tx_dequeue();
    j1939_tp_poll();
}

/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
