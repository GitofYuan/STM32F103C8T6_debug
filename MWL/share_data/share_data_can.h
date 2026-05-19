/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : share_data_can.h
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 
******************************************************************************/
#ifndef _SHARE_DATA_CAN_H_
#define _SHARE_DATA_CAN_H_
/* ==============================  INCLUDES  =============================== */
#include "share_data.h"
//#include <stdint.h>

/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
#pragma pack(1)          /*设置1字节对齐*/

/* CAN报文控制属性*/
typedef struct
{
    uint8_t   receive_flag;  /*接收标志位*/
    uint8_t   send_flag;     /*默认发送标志位*/
    uint8_t   send_flag2;    /*临时发送标志位，使能的前提是默认发送标志位使能，否则无效*/
}can_frame_control_t;

/*27930-2015报文*/
enum
{
    CHM = 0,/*充电机握手*/
    BHM,    /*车辆握手*/
    CRM,    /*充电机辨识*/
    BRM,    /*BMS和车辆辨识报文*/
    BCP,    /*动力蓄电池充电参数*/
    CTS,    /*充电机发送时间同步信息*/
    CML,    /*充电机最大输出能力*/
    BRO,    /*电池充电准备就绪状态*/
    CRO,    /*充电机输出准备就绪状态*/
    BCL,    /*电池充电需求*/
    BCS,    /*电池充电总状态*/
    CCS,    /*充电机充电状态*/
    BSM,    /*动力蓄电池状态信息*/
    BMV,    /*单体动力蓄电池电压*/
    BMT,    /*动力蓄电池温度*/
    BSP,    /*动力蓄电池预留报文*/
    BST,    /*BMS终止充电*/
    CST,    /*充电机终止充电*/
    BSD,    /*BMS统计数据*/
    CSD,    /*充电机统计数据*/
    BEM,    /*BMS错误报文*/
    CEM,    /*充电机错误报文*/
    CAN27930MAX,
};

/*电池类型*/
typedef enum
{
    VRLA = 1, /*铅酸电池*/
    NI_MH,    /*镍氢电池*/
    LFP,      /*磷酸铁锂电池*/
    LMO,      /*锰酸锂电池*/
    LCO,      /*钴酸锂电池*/
    NCM,      /*三元材料电池*/
    LI_PO,    /*聚合物锂离子电池*/
    LTO,      /*钛酸锂电池*/
    OTHER = 0xFF,    /*其他电池*/
}battery_type_e;

/*车辆终止充电原因*/
typedef enum
{
    STOP_ABNORMAL = 0,          /*异常终止*/
    ATTAIN_SOC,                 /*达到所需求的SOC目标值*/
    ATTAIN_SET_VOL,             /*达到总电压的设定值*/
    ATTAIN_SET_SINGLE_VOL,      /*达到单体电压的设定值*/
    CHARGER_STOP,               /*充电机主动停止*/
}bms_stop_reason_e;

/*车辆终止充电故障原因*/
typedef enum
{
    STOP_NORMAL = 0,            /*正常终止*/
    ISO_FAULT,                  /*绝缘故障*/
    SOCKET_OVERHEAT,            /*车辆插座过温故障*/
    CABLE_OVERHEAT,             /*车辆内部线束或相关连接器过温*/
    CONNECTOR_FAULT,            /*充电连接器故障*/
    BETTERY_OVERHEAT,           /*电池组温度过高故障*/
    RELAY_FAULT,                /*高压继电器故障*/
    CC2_VOL_FAULT,              /*检测点2电压检测故障*/
    OTHER_FAULT,                /*其他故障*/
    BMS_OVERCUR,                /*电流过大*/
    BMS_VOL_ABNORMAL,           /*电压异常*/
    PARAMETER_MISMATCH,         /*参数不匹配*/
    RX_CRM_00_TIMEOUT,          /*接收充电机辨识报文（CRM00）超时*/
    RX_CRM_AA_TIMEOUT,          /*接收充电机辨识报文（CRMAA）超时*/
    RX_CTS_CML_TIMEOUT,         /*接收充电机CTS或CML超时*/
    RX_CRO_TIMEOUT,             /*接收充电机CRO超时*/
    RX_CCS_TIMEOUT,             /*接收充电机CCS超时*/
    RX_CST_TIMEOUT,             /*接收充电机CTS超时*/
    RX_CSD_TIMEOUT,             /*接收充电机CTD超时*/
}bms_fault_reason_e;

/*充电机终止充电原因*/
typedef enum
{
    ATTAIN_SET_CONDITION = 1,   /*达到充电机设定终止条件*/
    USER_STOP,                  /*人工终止*/
    STOP_BY_FAULT,              /*故障终止*/
    BMS_STOP,                   /*车辆主动终止*/
}charger_stop_reason_e;

/*充电机终止充电故障原因*/
typedef enum
{
    CHARGER_OVERHEAT = 1,       /*充电机过温故障*/
    C_CONNECTOR_FAULT,          /*充电连接器故障*/
    OVERHEAT_INSIDE_CHARGER,    /*充电机内部过温故障*/
    REQ_CUR_CANNOT_TRANS,       /*所需电量不能传送*/
    EMERGENCY_STOP_FAULT,       /*充电机急停故障*/
    C_OTHER_FAULT,              /*其他故障*/
    SELF_CHECK_FAULT,           /*自检故障（包括绝缘检测、短路检测、粘连检测等）*/
    PRECHARGE_FAULT,            /*预充故障（包括预充电压不匹配、预充失败等）*/
    C_CUR_MISMATCH,             /*电流不匹配*/
    C_VOL_ABNORMAL,             /*电压异常*/
    C_PARAMETER_MISMATCH,       /*充电参数不匹配*/
    RX_BRM_TIMEOUT,             /*接收车辆BRM超时*/
    RX_BCP_TIMEOUT,             /*接收车辆BCP超时*/
    RX_BRO_TIMEOUT,             /*接收车辆BRO超时*/
    RX_BCS_TIMEOUT,             /*接收车辆BCS超时*/
    RX_BCL_TIMEOUT,             /*接收车辆BCL超时*/
    RX_BST_TIMEOUT,             /*接收车辆BST超时*/
    RX_BSD_TIMEOUT,             /*接收车辆BSD超时*/
    RX_BSM_TIMEOUT,             /*接收车辆BSM超时*/
}charger_fault_reason_e;



/*充电桩充电数据*/
typedef struct
{
    uint8_t    charger_version1;                /*充电机通信版本号byte1*/
    uint8_t    charger_version2;                /*充电机通信版本号byte2*/
    uint8_t    charger_version3;                /*充电机通信版本号byte3*/
    uint8_t    bms_recog_result;                /*车辆辨识结果，00未辨识，AA已辨识*/
    uint32_t   charger_num;                     /*充电机编号*/
    uint32_t   charger_area_code;               /*充电机区域编码*/
    uint16_t   charger_max_out_vol;             /*充电机最高输出充电电压，V，0.1精度*/
    uint16_t   charger_min_out_vol;             /*充电机最低输出充电电压，V，0.1精度*/
    uint16_t   charger_max_out_cur;             /*充电机最大输出充电电流，A，0.1精度，-400偏移量*/
    uint16_t   charger_min_out_cur;             /*充电机最小输出充电电流，A，0.1精度，-400偏移量*/
    uint8_t    charger_cro;                     /*充电机充电准备状态，00未就绪，AA就绪*/
    uint16_t   charger_measure_vol;             /*车辆接口当前电压测量值，V，0.1精度*/
    uint16_t   charger_measure_cur;             /*车辆接口当前电流测量值，A，0.1精度，-400偏移量*/
    uint16_t   charger_charging_time;           /*累计充电时间，min，1精度*/
    uint8_t    charger_charging_allow_status;   /*充电允许状态，00暂停，01允许*/
    charger_stop_reason_e    charger_stop_reason;    /*充电机终止充电原因*/
    charger_fault_reason_e   charger_fault_reason;   /*充电机终止充电故障原因*/
    uint16_t   charger_total_charging_time;     /*本次充电时间，min，1精度*/
    uint16_t   charger_total_charging_energy;   /*本次充电输出能量，kWh，0.1精度*/
    
}charger_charging_info_t;

/*车辆充电数据*/
typedef struct
{
    uint16_t        max_total_vol_bms;           /*车辆最高允许充电总电压，V，0.1精度*/
    uint8_t         bms_version1;                /*车辆通信版本号byte1*/
    uint8_t         bms_version2;                /*车辆通信版本号byte2*/
    uint8_t         bms_version3;                /*车辆通信版本号byte3*/
    battery_type_e  battery_type;                /*车辆电池类型*/
    uint16_t        bms_rated_capacity;          /*整车动力蓄电池系统额定容量，Ah，0.1精度*/
    uint16_t        bms_rated_vol;               /*整车动力蓄电池系统额定总电压，V，0.1精度*/
    uint32_t        bms_maker;                   /*电池生产厂商，ASCII*/
    uint32_t        bms_serial_num;              /*电池组序号*/
    uint8_t         bms_production_data_year;    /*电池生产日期，年，1985-2235，1985偏移量*/
    uint8_t         bms_production_data_month;   /*电池生产日期，月*/
    uint8_t         bms_production_data_day;     /*电池生产日期，日*/
    uint32_t        bms_charge_cycles;           /*电池组充电次数*/
    uint8_t         bms_ownership_mark;          /*电池组产品标识，0=租赁，1=车自有*/
    uint8_t         bms_vin[17];                 /*车辆识别码VIN*/
    uint32_t        bms_soft_version;            /*车辆通信协议软件版本号*/
    uint16_t        bms_soft_data_year;          /*车辆通信软件版本编译日期，年*/
    uint8_t         bms_soft_data_month;         /*车辆通信软件版本编译日期，月*/
    uint8_t         bms_soft_data_day;           /*车辆通信软件版本编译日期，日*/
    uint16_t        bms_max_single_allow_vol;    /*单体动力蓄电池最高允许充电电压,V,0.01精度*/
    uint16_t        bms_max_charge_cur;          /*最高允许充电电流，A，0.1精度，-400偏移量*/
    uint16_t        bms_rated_energy;            /*动力蓄电池标称总能量，kWh，0.1精度*/
    uint16_t        bms_max_total_vol;           /*最高允许充电总电压，BCP，V，0.1精度*/
    uint8_t         bms_max_single_allow_tem;    /*单体蓄电池最高允许温度，℃，1精度，-50偏移量*/
    uint16_t        bms_start_soc;               /*当前荷电状态，BCP，％，0.1精度*/
    uint16_t        bms_current_vol;             /*整车充电系统当前电压，V，0.1精度*/
    uint8_t         bms_bro;                     /*车辆充电准备状态，00未就绪，AA就绪*/
    uint16_t        bms_required_vol;            /*车辆接口充电电压需求，V，0.1精度*/
    uint16_t        bms_required_cur;            /*车辆接口充电电流需求，A，0.1精度，-400偏移量*/
    uint8_t         bms_charging_mode;           /*充电模式，01恒压，02恒流*/
    uint16_t        bms_measure_vol;             /*车辆接口当前电压测量值，V，0.1精度*/
    uint16_t        bms_measure_cur;             /*车辆接口当前电流测量值，A，0.1精度，-400偏移量*/
    uint16_t        bms_max_single_vol;          /*最高单体动力蓄电池电压，V，0.01精度*/
    uint8_t         bms_max_single_vol_gnum;     /*最高单体动力蓄电池电压所在组号*/
    uint16_t        bms_current_soc;             /*当前荷电状态，BCS，％，0.1精度*/
    uint16_t        bms_remaining_charging_time; /*估算剩余充电时间，min，1精度*/
    uint8_t         bms_max_single_vol_num;      /*最高单体动力蓄电池电压所在编号*/
    uint8_t         bms_max_single_tem;          /*当前单体蓄电池最高温度，℃，1精度，-50偏移量*/
    uint8_t         bms_max_single_tem_num;      /*最高动力蓄电池温度监测点编号*/
    uint8_t         bms_min_single_tem;          /*当前单体蓄电池最低温度，℃，1精度，-50偏移量*/
    uint8_t         bms_min_single_tem_num;      /*最低动力蓄电池温度监测点编号*/
    uint8_t         bms_single_vol_abnormal;     /*单体蓄电池电压异常，00正常，01过高，02过低*/
    uint8_t         bms_soc_abnormal;            /*动力蓄电池荷电状态SOC异常，00正常，01过高，02过低*/
    uint8_t         bms_cur_abnormal;            /*动力蓄电池充电过电流，00正常，01过流，02不可信*/
    uint8_t         bms_tem_abnormal;            /*动力蓄电池温度过高，00正常，01过高，02不可信*/
    uint8_t         bms_insulation_status;       /*动力蓄电池绝缘状态，00正常，01不正常，02不可信*/
    uint8_t         bms_connect_status;          /*动力蓄电池输出连接器连接状态，00正常，01不正常，02不可信*/
    uint8_t         bms_charging_allow_status;   /*充电允许状态，00禁止，01允许*/
    bms_stop_reason_e   bms_stop_reason;         /*车辆终止充电原因*/
    bms_fault_reason_e  bms_fault_reason;        /*车辆终止充电故障原因*/
    uint16_t        bms_stop_soc;                /*终止荷电状态SOC，％，0.1精度*/
    uint16_t        bms_single_min_vol;          /*单体动力蓄电池最低电压，V，0.01精度*/
    uint16_t        bms_single_max_vol;          /*单体动力蓄电池最高电压，V，0.01精度*/
    uint8_t         bms_min_tem;                 /*动力蓄电池最低温度，℃，1精度，-50偏移量*/
    uint8_t         bms_max_tem;                 /*动力蓄电池最高温度，℃，1精度，-50偏移量*/
}bms_charging_info_t;

/* ==============================  EXTERNS   =============================== */

#pragma pack()         /*恢复系统默认对其*/
/* ========================= FUNCTION PROTOTYPES =========================== */


#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
