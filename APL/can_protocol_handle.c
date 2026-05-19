/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : can_protocol_handle.h
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : can协议处理，含协议解析、收发。
                      1、系统时间尚未定义出处，所以该部分尚未完善。
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include <string.h>
#include "can_protocol_handle.h"

/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
/*针对27930.2协议的报文属性结构体*/
typedef struct
{
   Can_Frame_Attribute CVN;    /*charger version nagotiation 充电机版本协商报文*/
   Can_Frame_Attribute BVN;    /*BMS version nagotiation 车辆版本协商报文*/

   Can_Frame_Attribute CPR;    /*charger phase request 充电机阶段请求*/
   Can_Frame_Attribute BPA;    /*BMS phase ACK 车辆阶段确认结果*/
   Can_Frame_Attribute CST;    /*charger stop 充电机终止报文*/
   Can_Frame_Attribute BST;    /*BMS stop 车辆终止报文*/
   Can_Frame_Attribute CCS;    /*charger contact status 直流供电回路接触器状态*/
   Can_Frame_Attribute BCS;    /*BMS contast status 车辆供电回路接触器状态*/
   Can_Frame_Attribute CLS;    /*charger latching status 充电机电子锁状态*/
   Can_Frame_Attribute CWU;    /*charger wake up 充电机唤醒*/
   Can_Frame_Attribute BWU;    /*BMS wake up 车辆唤醒*/

   Can_Frame_Attribute CFS;    /*charger FDC support 充电机支持功能报文*/
   Can_Frame_Attribute FNR;    /*BMS FDC Negotiation result 车辆功能协商确认结果报文*/

   Can_Frame_Attribute CCP;    /*charger charging parameter 充电机充电参数*/
   Can_Frame_Attribute BCP;    /*BMS charging parameter 车辆充电参数*/

   Can_Frame_Attribute CDCP;   /*charger charging and discharging parameter 充电机充放电参数*/
   Can_Frame_Attribute BDCP;   /*BMS charging and discharging parameter 车辆充放电参数*/

   Can_Frame_Attribute CAP;    /*charging Authentication parameter 充电机鉴权参数*/
   Can_Frame_Attribute BAW;    /*BMS Authentication waiting status 车辆鉴权等待状态*/
   Can_Frame_Attribute CAR;    /*charger Authentication result 充电机鉴权结果*/

   Can_Frame_Attribute BAP;    /*BMS Authentication parameter 车辆鉴权参数*/
   Can_Frame_Attribute BAR;    /*BMS Authentication result 车辆鉴权结果*/
   Can_Frame_Attribute RAR;    /*Re authentication request 重新鉴权请求*/
   /*暂时不考虑鉴权-FDC3*/

   Can_Frame_Attribute CSI;    /*charger schedule information 充电机预约充电信息*/
   Can_Frame_Attribute BSI;    /*BMS schedule information 车辆预约充电信息*/
   Can_Frame_Attribute CSA;    /*charger schedule ACK 充电机预约充电确认*/
   Can_Frame_Attribute BSN;    /*BMS schedule negotiation 车辆预约充电确认*/
   Can_Frame_Attribute CCKI;   /*charger check information 充电机检测信息*/
   Can_Frame_Attribute BCN;    /*BMS check negotiation 车辆预约充电确认*/

   Can_Frame_Attribute CPSS;   /*charger power supply state 充电机供电状态*/
   Can_Frame_Attribute BPSS;   /*BMS power supply state 车辆供电状态*/
   Can_Frame_Attribute BPD;    /*BMS power supply demand 车辆供电需求*/
   Can_Frame_Attribute CPC;    /*charger power supply capacity 充电机供电动态输出能力*/
   Can_Frame_Attribute BPE;    /*BMS power supply end 车辆供电完成*/
   Can_Frame_Attribute CPI;    /*charger power supply information 充电机供电基本信息*/

   Can_Frame_Attribute CCR;    /*charger charging ready 充电机充电准备就绪状态*/
   Can_Frame_Attribute BCR;    /*BMS charging ready 车辆充电准备就绪状态*/
   Can_Frame_Attribute BCD;    /*BMS charging demand 车辆充电需求*/
   Can_Frame_Attribute BCI;    /*BMS charging information 车辆充电基本信息*/
   Can_Frame_Attribute CCC;    /*charger charging capacity 充电机充电动态输出能力*/
   Can_Frame_Attribute CCI;    /*charger charging information 充电机充电基本信息*/
   Can_Frame_Attribute BBI;    /*BMS battery information 车辆充电电池基本信息*/
   Can_Frame_Attribute CPS;    /*charger charging pause stats 充电机充电暂定状态*/
   Can_Frame_Attribute BPS;    /*BMS charging pause stats 车辆充放电暂停状态*/
   
   Can_Frame_Attribute CDCR;   /*charger discharging ready 充电机充放电准备就绪状态*/
   Can_Frame_Attribute BDCR;   /*BMS discharging ready 车辆充放电准备就绪状态*/
   Can_Frame_Attribute CDCD;   /*charger discharging demand 充电机放电需求*/
   Can_Frame_Attribute BDCC;   /*BMS discharging capacity 车辆放电动态输出能力*/
   Can_Frame_Attribute BDCD;   /*BMS charging demand 车辆充电需求*/
   Can_Frame_Attribute CDCC;   /*charger charging capacity 充电机充电动态输出能力*/
   Can_Frame_Attribute BDCI;   /*BMS discharging information 车辆充放电基本信息*/
   Can_Frame_Attribute CDCI;   /*charger discharging information 充电机充放电基本信息*/
   Can_Frame_Attribute BDBI;   /*BMS discharging battery information 车辆充放电电池基本信息*/
   Can_Frame_Attribute CDPS;   /*charger discharging pause stats 充电机充放电暂定状态*/
   Can_Frame_Attribute BDPS;   /*BMS discharging pause stats 车辆充放电暂停状态*/
   
   Can_Frame_Attribute BAC;    /*BMS adhesion check 车辆粘连检测*/
   Can_Frame_Attribute CACE;   /*charger adhesion check enable 充电机允许粘连检测*/
   Can_Frame_Attribute CSIF;   /*charger statistical information 充电机统计信息*/
   Can_Frame_Attribute BSIF;   /*BMS statistical information 车辆统计信息*/
}can27930_2_t;

/* ==============================  EXTERNS   =============================== */
/*对27930协议报文属性进行定义*/
const Can_Frame_Attribute frame_attribute_27930[CAN27930MAX] = 
{  /*           PGN          优先级    长度  默认发送周期  默认发送次数  临时发送周期  临时发送次数    */
   /*CHM*/    { 0x26,        6,        3,        250,        0xFFFF,      250,         0xFFFF     },
   /*BHM*/    { 0x27,        6,        2,        250,        0xFFFF,      250,         0xFFFF     },
   /*CRM*/    { 0x01,        6,        8,        250,        0xFFFF,      250,         0xFFFF     },
   /*BRM*/    { 0x02,        7,        41,       250,        0xFFFF,      250,         0xFFFF     },
   /*BCP*/    { 0x06,        7,        13,       500,        0xFFFF,      500,         0xFFFF     },
   /*CTS*/    { 0x07,        6,        7,        500,        0xFFFF,      500,         0xFFFF     },
   /*CML*/    { 0x08,        6,        8,        250,        0xFFFF,      250,         0xFFFF     },
   /*BRO*/    { 0x09,        4,        1,        250,        0xFFFF,      250,         0xFFFF     },
   /*CRO*/    { 0x0A,        4,        1,        250,        0xFFFF,      250,         0xFFFF     },
   /*BCL*/    { 0x10,        6,        5,        50,         0xFFFF,      50,          0xFFFF     },
   /*BCS*/    { 0x11,        7,        9,        250,        0xFFFF,      250,         0xFFFF     },
   /*CCS*/    { 0x12,        6,        8,        50,         0xFFFF,      50,          0xFFFF     },
   /*BSM*/    { 0x13,        6,        7,        250,        0xFFFF,      250,         0xFFFF     },
   /*BMV*/    { 0x15,        7,        100,      10000,      0xFFFF,      10000,       0xFFFF     },
   /*BMT*/    { 0x16,        7,        100,      10000,      0xFFFF,      10000,       0xFFFF     },
   /*BSP*/    { 0x17,        7,        100,      10000,      0xFFFF,      10000,       0xFFFF     },
   /*BST*/    { 0x19,        4,        4,        10,         0xFFFF,      10,          0xFFFF     },
   /*CST*/    { 0x1A,        1,        4,        10,         0xFFFF,      10,          0xFFFF     },
   /*BSD*/    { 0x1C,        6,        7,        250,        0xFFFF,      250,         0xFFFF     },
   /*CSD*/    { 0x1D,        6,        8,        250,        0xFFFF,      250,         0xFFFF     },
   /*BEM*/    { 0x1E,        2,        4,        250,        0xFFFF,      250,         0xFFFF     },
   /*CEM*/    { 0x1F,        2,        4,        250,        0xFFFF,      250,         0xFFFF     },
};

/*对27930_2协议报文属性进行定义*/
const can27930_2_t frame_attribute_27930_2 =
{  /*           PGN/FC       优先级    长度  默认发送周期  默认发送次数  临时发送周期  临时发送次数    */
   /*CVN*/    { 0x38,        3,        8,        50,         0xFFFF,      50,          0xFFFF     },
   /*BVN*/    { 0x39,        3,        8,        50,         0xFFFF,      50,          0xFFFF     },
   /*CPR*/    { 0x01,        0,        3,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*BPA*/    { 0x02,        0,        2,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*CST*/    { 0x03,        0,        5,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*BST*/    { 0x04,        0,        5,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*CCS*/    { 0x05,        0,        3,        1000,       0xFFFF,      50,          0xFFFF     },
   /*BCS*/    { 0x06,        0,        3,        1000,       0xFFFF,      50,          0xFFFF     },
   /*CLS*/    { 0x07,        0,        2,        1000,       0xFFFF,      50,          0xFFFF     },
   /*CWU*/    { 0x08,        0,        2,        0,          0xFFFF,      0,           10000      },/*需要确认的短消息*/
   /*BWU*/    { 0x09,        0,        2,        0,          0xFFFF,      0,           10000      },/*需要确认的短消息*/
   /*CFS*/    { 0x11,        0,        57,       0,          0xFFFF,      0,           5000       },/*长消息*/
   /*FNR*/    { 0x12,        0,        8,        50,         0xFFFF,      50,          1000       },
   /*CCP*/    { 0x21,        0,        10,       0,          0xFFFF,      0,           5000       },/*长消息*/
   /*BCP*/    { 0x22,        0,        14,       0,          0xFFFF,      0,           5000       },/*长消息*/
   /*CDCP*/   { 0x23,        0,        18,       0,          0xFFFF,      0,           5000       },/*长消息*/
   /*BDCP*/   { 0x24,        0,        30,       0,          0xFFFF,      0,           5000       },/*长消息*/
   /*CAP*/    { 0x31,        0,        3,        250,        0xFFFF,      250,         0xFFFF     },
   /*BAW*/    { 0x32,        0,        2,        0,          0xFFFF,      0,           0xFFFF     },
   /*CAR*/    { 0x33,        0,        3,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*BAP*/    { 0x34,        0,        18,       0,          0xFFFF,      0,           1000       },/*长消息*/
   /*BAR*/    { 0x35,        0,        3,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*RAR*/    { 0x36,        0,        2,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*CSI*/    { 0x41,        0,        196,      0,          0xFFFF,      0,           10000      },/*长消息*/
   /*BSI*/    { 0x42,        0,        5,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*CSA*/    { 0x43,        0,        6,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*BSN*/    { 0x44,        0,        3,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*CCKI*/   { 0x51,        0,        5,        250,        0xFFFF,      250,         0xFFFF     },
   /*BCN*/    { 0x52,        0,        2,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*CPSS*/   { 0x61,        0,        2,        250,        0xFFFF,      250,         0xFFFF     },
   /*BPSS*/   { 0x62,        0,        2,        0,          0xFFFF,      0,           0xFFFF     },/*一问一答报文*/
   /*BPD*/    { 0x63,        0,        5,        1000,       0xFFFF,      50,          0xFFFF     },
   /*CPC*/    { 0x64,        0,        4,        1000,       0xFFFF,      50,          0xFFFF     },
   /*BPE*/    { 0x65,        0,        2,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*CPI*/    { 0x66,        0,        5,        1000,       0xFFFF,      1000,        0xFFFF     },
   /*CCR*/    { 0x71,        0,        2,        250,        0xFFFF,      250,         0xFFFF     },
   /*BCR*/    { 0x72,        0,        4,        0,          0xFFFF,      0,           0xFFFF     },/*一问一答报文*/
   /*BCD*/    { 0x73,        0,        6,        1000,       0xFFFF,      50,          0xFFFF     },
   /*BCI*/    { 0x74,        0,        5,        1000,       0xFFFF,      1000,        0xFFFF     },
   /*CCC*/    { 0x75,        0,        4,        1000,       0xFFFF,      50,          0xFFFF     },
   /*CCI*/    { 0x76,        0,        5,        1000,       0xFFFF,      1000,        0xFFFF     },
   /*BBI*/    { 0x77,        0,        7,        1000,       0xFFFF,      1000,        0xFFFF     },
   /*CPS*/    { 0x78,        0,        2,        0,          0xFFFF,      0,           5000       },/*需要确认的短消息*/
   /*BPS*/    { 0x79,        0,        2,        0,          0xFFFF,      0,           5000       },/*需要确认的短消息*/
   /*CDCR*/   { 0x81,        0,        2,        250,        0xFFFF,      250,         0xFFFF     },
   /*BDCR*/   { 0x82,        0,        4,        0,          0xFFFF,      0,           0xFFFF     },/*一问一答报文*/
   /*CDCD*/   { 0x83,        0,        6,        1000,       0xFFFF,      50,          0xFFFF     },
   /*BDCC*/   { 0x84,        0,        4,        1000,       0xFFFF,      50,          0xFFFF     },
   /*BDCD*/   { 0x85,        0,        6,        1000,       0xFFFF,      50,          0xFFFF     },
   /*CDCC*/   { 0x86,        0,        4,        1000,       0xFFFF,      50,          0xFFFF     },
   /*BDCI*/   { 0x87,        0,        5,        1000,       0xFFFF,      1000,        0xFFFF     },
   /*CDCI*/   { 0x88,        0,        5,        1000,       0xFFFF,      1000,        0xFFFF     },
   /*BDBI*/   { 0x89,        0,        7,        1000,       0xFFFF,      1000,        0xFFFF     },
   /*CDPS*/   { 0x8A,        0,        2,        0,          0xFFFF,      0,           5000       },/*需要确认的短消息*/
   /*BDPS*/   { 0x8B,        0,        2,        0,          0xFFFF,      0,           5000       },/*需要确认的短消息*/
   /*BAC*/    { 0x91,        0,        2,        250,        0xFFFF,      250,         0xFFFF     },
   /*CACE*/   { 0x92,        0,        2,        250,        0xFFFF,      250,         0xFFFF     },
   /*CSIF*/   { 0x93,        0,        5,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
   /*BSIF*/   { 0x94,        0,        3,        0,          0xFFFF,      0,           1000       },/*需要确认的短消息*/
};

/*定义所有枪及其所有27930协议报文的发送定时器*/
frame_send_timer_t frame_send_timer_27930[CHAR_GUN_NUM][CAN27930MAX] = {0};     /*27930协议报文发送定时器*/

/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :protocol_handle_bhm(BHM报文接收解析处理函数)
*Description:对收到的BHM报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_bhm(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t bhm_recv_flag = 1;
    uint16_t max_total_vol_bms = 0;           /*车辆最高允许充电总电压，V，0.1精度*/

    
    max_total_vol_bms = 0x0000 | data->data_s[0];
    max_total_vol_bms = max_total_vol_bms | (data->data_s[1] << 8);

    uint32_t mesure_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].max_total_vol_bms);
    share_data_write(MESURE_INFO, mesure_offset, &max_total_vol_bms, sizeof(max_total_vol_bms));
    
    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BHM].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &bhm_recv_flag, sizeof(bhm_recv_flag));

}

/*****************************************************************************************************
*Function   :protocol_handle_brm(BRM报文接收解析处理函数)
*Description:对收到的BRM报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_brm(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t brm_recv_flag = 1;
    struct brm_t
    {
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
        uint8_t         bms_ownership_mark;          /*电池组产权标识，0=租赁，1=车自有*/
        uint8_t         bms_vin[17];                 /*车辆识别码VIN*/
        uint32_t        bms_soft_version;            /*车辆通信协议软件版本号*/
        uint16_t        bms_soft_data_year;          /*车辆通信软件版本编译日期，年*/
        uint8_t         bms_soft_data_month;         /*车辆通信软件版本编译日期，月*/
        uint8_t         bms_soft_data_day;           /*车辆通信软件版本编译日期，日*/
    }brm = {0};

    brm.bms_version1 = data->data_m[2];
    brm.bms_version2 = data->data_m[1];
    brm.bms_version3 = data->data_m[0];
    if(data->data_m[3] != 0 && data->data_m[3]<= LTO)
    {
        brm.battery_type = (battery_type_e)data->data_m[3];  /*这里有越界风险，如果报文不在枚举范围内呢？*/
    }
    else
    {
        brm.battery_type = OTHER;
    }
    brm.bms_rated_capacity = 0x0000 | data->data_m[4];
    brm.bms_rated_capacity = brm.bms_rated_capacity | (data->data_m[5] << 8);
    brm.bms_rated_vol = 0x0000 | data->data_m[6];
    brm.bms_rated_vol = brm.bms_rated_vol | (data->data_m[7] << 8);
    brm.bms_maker = 0x00000000 | (data->data_m[8] << 24);
    brm.bms_maker = brm.bms_maker  | (data->data_m[9] << 16);
    brm.bms_maker = brm.bms_maker  | (data->data_m[10] << 8);
    brm.bms_maker = brm.bms_maker  | data->data_m[11];
    brm.bms_serial_num = 0x00000000 | data->data_m[12];
    brm.bms_serial_num = brm.bms_serial_num  | (data->data_m[13] << 8);
    brm.bms_serial_num = brm.bms_serial_num  | (data->data_m[14] << 16);
    brm.bms_serial_num = brm.bms_serial_num  | (data->data_m[15] << 24);
    brm.bms_production_data_year = data->data_m[16];
    brm.bms_production_data_month = data->data_m[17];
    brm.bms_production_data_day = data->data_m[18];
    brm.bms_charge_cycles = 0x00000000 | data->data_m[19];
    brm.bms_charge_cycles = brm.bms_charge_cycles | (data->data_m[20] << 8);
    brm.bms_charge_cycles = brm.bms_charge_cycles | (data->data_m[21] << 16);
    brm.bms_ownership_mark = data->data_m[22];
    memcpy(brm.bms_vin, &(data->data_m[24]), sizeof(brm.bms_vin));
    memcpy(&(brm.bms_soft_version), &(data->data_m[41]), sizeof(brm.bms_soft_version));
    brm.bms_soft_data_year = 0x0000 | data->data_m[44];
    brm.bms_soft_data_year = brm.bms_soft_data_year | (data->data_m[45] << 8);
    brm.bms_soft_data_month = data->data_m[46];
    brm.bms_soft_data_day = data->data_m[47];
    
    uint32_t mesure_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_version1);
    share_data_write(MESURE_INFO, mesure_offset, &brm, sizeof(brm));
    
    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BRM].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &brm_recv_flag, sizeof(brm_recv_flag));
}

/*****************************************************************************************************
*Function   :protocol_handle_bcp(BCP报文接收解析处理函数)
*Description:对收到的BCP报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_bcp(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t bcp_recv_flag = 1;
    struct bcp_t
    {
        uint16_t        bms_max_single_allow_vol;    /*单体动力蓄电池最高允许充电电压,V,0.01精度*/
        uint16_t        bms_max_charge_cur;          /*最高允许充电电流，A，0.1精度，-400偏移量*/
        uint16_t        bms_rated_energy;            /*动力蓄电池标称总能量，kWh，0.1精度*/
        uint16_t        bms_max_total_vol;           /*最高允许充电总电压，BCP，V，0.1精度*/
        uint8_t         bms_max_single_allow_tem;    /*单体蓄电池最高允许温度，℃，1精度，-50偏移量*/
        uint16_t        bms_start_soc;               /*当前荷电状态，BCP，％，0.1精度*/
        uint16_t        bms_current_vol;             /*整车充电系统当前电压，V，0.1精度*/
    }bcp = {0};

    bcp.bms_max_single_allow_vol = 0x0000 | data->data_m[0];
    bcp.bms_max_single_allow_vol = bcp.bms_max_single_allow_vol | (data->data_m[1] << 8);
    bcp.bms_max_charge_cur = 0x0000 | data->data_m[2];
    bcp.bms_max_charge_cur = bcp.bms_max_charge_cur | (data->data_m[3] << 8);
    bcp.bms_rated_energy = 0x0000 | data->data_m[4];
    bcp.bms_rated_energy = bcp.bms_rated_energy | (data->data_m[5] << 8);
    bcp.bms_max_total_vol = 0x0000 | data->data_m[6];
    bcp.bms_max_total_vol = bcp.bms_max_total_vol | (data->data_m[7] << 8);
    bcp.bms_max_single_allow_tem = data->data_m[8];
    bcp.bms_start_soc = 0x0000 | data->data_m[9];
    bcp.bms_start_soc = bcp.bms_start_soc | (data->data_m[10] << 8);
    bcp.bms_current_vol = 0x0000 | data->data_m[11];
    bcp.bms_current_vol = bcp.bms_current_vol | (data->data_m[12] << 8);;

    uint32_t mesure_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_max_single_allow_vol);
    share_data_write(MESURE_INFO, mesure_offset, &bcp, sizeof(bcp));
    
    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BCP].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &bcp_recv_flag, sizeof(bcp_recv_flag));
}

/*****************************************************************************************************
*Function   :protocol_handle_bro(BRO报文接收解析处理函数)
*Description:对收到的BRO报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_bro(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t bro_recv_flag = 1;
    uint8_t bms_bro = 0;                     /*车辆充电准备状态，00未就绪，AA就绪*/
    
    bms_bro = data->data_s[0];

    uint32_t mesure_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_bro);
    share_data_write(MESURE_INFO, mesure_offset, &bms_bro, sizeof(bms_bro));
    
    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BRO].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &bro_recv_flag, sizeof(bro_recv_flag));
}

/*****************************************************************************************************
*Function   :protocol_handle_bcl(BCL报文接收解析处理函数)
*Description:对收到的BCL报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_bcl(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t bcl_recv_flag = 1;
    struct bcl_t
    {
        uint16_t        bms_required_vol;            /*车辆接口充电电压需求，V，0.1精度*/
        uint16_t        bms_required_cur;            /*车辆接口充电电流需求，A，0.1精度，-400偏移量*/
        uint8_t         bms_charging_mode;           /*充电模式，01恒压，02恒流*/
    }bcl = {0};
    
    bcl.bms_required_vol = 0x0000 | data->data_s[0];
    bcl.bms_required_vol = bcl.bms_required_vol | (data->data_s[1] << 8);
    bcl.bms_required_cur = 0x0000 | data->data_s[2];
    bcl.bms_required_cur = bcl.bms_required_cur | (data->data_s[3] << 8);
    bcl.bms_charging_mode = data->data_s[4];

    uint32_t mesure_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_required_vol);
    share_data_write(MESURE_INFO, mesure_offset, &bcl, sizeof(bcl));
    
    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BCL].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &bcl_recv_flag, sizeof(bcl_recv_flag));
}

/*****************************************************************************************************
*Function   :protocol_handle_bcs(BCS报文接收解析处理函数)
*Description:对收到的BCS报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_bcs(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t bcs_recv_flag = 1;
    struct bcs_t
    {
        uint16_t        bms_measure_vol;             /*车辆接口当前电压测量值，V，0.1精度*/
        uint16_t        bms_measure_cur;             /*车辆接口当前电流测量值，A，0.1精度，-400偏移量*/
        uint16_t        bms_max_single_vol;          /*最高单体动力蓄电池电压，V，0.01精度*/
        uint8_t         bms_max_single_vol_gnum;     /*最高单体动力蓄电池电压所在组号*/
        uint16_t        bms_current_soc;             /*当前荷电状态，BCS，％，0.1精度*/
        uint16_t        bms_remaining_charging_time; /*估算剩余充电时间，min，1精度*/
    }bcs = {0};

    bcs.bms_measure_vol = 0x0000 | data->data_m[0];
    bcs.bms_measure_vol = bcs.bms_measure_vol | (data->data_m[1] << 8);
    bcs.bms_measure_cur = 0x0000 | data->data_m[2];
    bcs.bms_measure_cur = bcs.bms_measure_cur | (data->data_m[3] << 8);
    bcs.bms_max_single_vol = 0x0000 | data->data_m[4];
    bcs.bms_max_single_vol = bcs.bms_max_single_vol | ((data->data_m[5] & 0x0F) << 8);
    bcs.bms_max_single_vol_gnum = data->data_m[5] >> 4;
    bcs.bms_current_soc = 0x0000 | data->data_m[6];
    bcs.bms_current_soc = bcs.bms_current_soc | (data->data_m[7] << 8);
    bcs.bms_remaining_charging_time = 0x0000 | data->data_m[8];
    bcs.bms_remaining_charging_time = bcs.bms_remaining_charging_time | (data->data_m[9] << 8);

    uint32_t mesure_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_measure_vol);
    share_data_write(MESURE_INFO, mesure_offset, &bcs, sizeof(bcs));

    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BCS].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &bcs_recv_flag, sizeof(bcs_recv_flag));
}

/*****************************************************************************************************
*Function   :protocol_handle_bsm(BSM报文接收解析处理函数)
*Description:对收到的BSM报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_bsm(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t bsm_recv_flag = 1;
    struct bsm_t
    {
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
    }bsm = {0};

    bsm.bms_max_single_vol_num = data->data_s[0];
    bsm.bms_max_single_tem = data->data_s[1];
    bsm.bms_max_single_tem_num = data->data_s[2];
    bsm.bms_min_single_tem = data->data_s[3];
    bsm.bms_min_single_tem_num = data->data_s[4];
    bsm.bms_single_vol_abnormal = 0x03 & (data->data_s[5] >> 0);
    bsm.bms_soc_abnormal = 0x03 & (data->data_s[5] >> 2);
    bsm.bms_cur_abnormal = 0x03 & (data->data_s[5] >> 4);
    bsm.bms_tem_abnormal = 0x03 & (data->data_s[5] >> 6);
    bsm.bms_insulation_status = 0x03 & (data->data_s[6] >> 0);
    bsm.bms_connect_status = 0x03 & (data->data_s[6] >> 2);
    bsm.bms_charging_allow_status = 0x03 & (data->data_s[6] >> 4);

    uint32_t mesure_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_max_single_vol_num);
    share_data_write(MESURE_INFO, mesure_offset, &bsm, sizeof(bsm));

    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BSM].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &bsm_recv_flag, sizeof(bsm_recv_flag));
}

/*****************************************************************************************************
*Function   :protocol_handle_bst(BST报文接收解析处理函数)
*Description:对收到的BST报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_bst(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t bst_recv_flag = 1;
    bms_stop_reason_e   bms_stop_reason;         /*车辆终止充电原因*/
    bms_fault_reason_e  bms_fault_reason;        /*车辆终止充电故障原因*/

    if(0x01 & (data->data_s[0] >> 0))
    {
        bms_stop_reason = ATTAIN_SOC;
    }
    else if(0x01 & (data->data_s[0] >> 2))
    {
        bms_stop_reason = ATTAIN_SET_VOL;
    }
    else if(0x01 & (data->data_s[0] >> 4))
    {
        bms_stop_reason = ATTAIN_SET_SINGLE_VOL;
    }
    else if(0x01 & (data->data_s[0] >> 6))
    {
        bms_stop_reason = CHARGER_STOP;
    }
    else
    {
        bms_stop_reason = STOP_ABNORMAL;
    }
    
    if(0x01 & (data->data_s[2] >> 0))
    {
        bms_fault_reason = ISO_FAULT;
    }
    else if(0x01 & (data->data_s[2] >> 2))
    {
        bms_fault_reason = SOCKET_OVERHEAT;
    }
    else if(0x01 & (data->data_s[2] >> 4))
    {
        bms_fault_reason = CABLE_OVERHEAT;
    }
    else if(0x01 & (data->data_s[2] >> 6))
    {
        bms_fault_reason = CONNECTOR_FAULT;
    }
    else if(0x01 & (data->data_s[1] >> 0))
    {
        bms_fault_reason = BETTERY_OVERHEAT;
    }
    else if(0x01 & (data->data_s[1] >> 2))
    {
        bms_fault_reason = RELAY_FAULT;
    }
    else if(0x01 & (data->data_s[1] >> 4))
    {
        bms_fault_reason = CC2_VOL_FAULT;
    }
    else if(0x01 & (data->data_s[1] >> 6))
    {
        bms_fault_reason = OTHER_FAULT;
    }
    else if(0x01 & (data->data_s[3] >> 0))
    {
        bms_fault_reason = BMS_OVERCUR;
    }
    else if(0x01 & (data->data_s[3] >> 2))
    {
        bms_fault_reason = BMS_VOL_ABNORMAL;
    }
    else if(0x01 & (data->data_s[3] >> 4))
    {
        bms_fault_reason = PARAMETER_MISMATCH;
    }
    else
    {
        bms_fault_reason = STOP_NORMAL;
    }

    uint32_t stop_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_stop_reason);
    share_data_write(MESURE_INFO, stop_offset, &bms_stop_reason, sizeof(bms_stop_reason));

    uint32_t fault_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_fault_reason);
    share_data_write(MESURE_INFO, fault_offset, &bms_fault_reason, sizeof(bms_fault_reason));

    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BSM].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &bst_recv_flag, sizeof(bst_recv_flag));
}

/*****************************************************************************************************
*Function   :protocol_handle_bsd(BSD报文接收解析处理函数)
*Description:对收到的BSD报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_bsd(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t bsd_recv_flag = 1;
    struct bsd_t
    {
        uint16_t        bms_stop_soc;                /*终止荷电状态SOC，％，0.1精度*/ /*特别注意，与协议精度不同需处理*/
        uint16_t        bms_single_min_vol;          /*单体动力蓄电池最低电压，V，0.01精度*/
        uint16_t        bms_single_max_vol;          /*单体动力蓄电池最高电压，V，0.01精度*/
        uint8_t         bms_min_tem;                 /*动力蓄电池最低温度，℃，1精度，-50偏移量*/
        uint8_t         bms_max_tem;                 /*动力蓄电池最高温度，℃，1精度，-50偏移量*/
    }bsd = {0};

    bsd.bms_stop_soc = data->data_s[0] * 10;
    bsd.bms_single_min_vol = 0x0000 | data->data_s[1];
    bsd.bms_single_min_vol = bsd.bms_single_min_vol | (data->data_s[2] << 8);
    bsd.bms_single_max_vol = 0x0000 | data->data_s[3];
    bsd.bms_single_max_vol = bsd.bms_single_max_vol | (data->data_s[4] << 8);
    bsd.bms_min_tem = data->data_s[5];
    bsd.bms_max_tem = data->data_s[6];

    uint32_t mesure_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_stop_soc);
    share_data_write(MESURE_INFO, mesure_offset, &bsd, sizeof(bsd)); 

    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BSD].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &bsd_recv_flag, sizeof(bsd_recv_flag));
}

/*****************************************************************************************************
*Function   :protocol_handle_bem(BEM报文接收解析处理函数)
*Description:对收到的BEM报文按照通信协议进行解析，然后将解析结果以及报文接收标志位写入共享内存中
*Input      :uint8_t      char_gun_num    充电枪号
             J1939_Data*  data            出队的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_handle_bem(uint8_t char_gun_num, J1939_Data* data)
{
    uint8_t bem_recv_flag = 1;
    bms_fault_reason_e  bms_fault_reason;        /*车辆终止充电故障原因*/

    if(0x01 & (data->data_s[0] >> 0))
    {
        bms_fault_reason = RX_CRM_00_TIMEOUT;
    }
    else if(0x01 & (data->data_s[0] >> 2))
    {
        bms_fault_reason = RX_CRM_AA_TIMEOUT;
    }
    else if(0x01 & (data->data_s[0] >> 4))
    {
        bms_fault_reason = RX_CTS_CML_TIMEOUT;
    }
    else if(0x01 & (data->data_s[0] >> 6))
    {
        bms_fault_reason = RX_CRO_TIMEOUT;
    }
    else if(0x01 & (data->data_s[1] >> 0))
    {
        bms_fault_reason = RX_CCS_TIMEOUT;
    }
    else if(0x01 & (data->data_s[1] >> 2))
    {
        bms_fault_reason = RX_CST_TIMEOUT;
    }
    else if(0x01 & (data->data_s[1] >> 4))
    {
        bms_fault_reason = RX_CSD_TIMEOUT;
    }

    uint32_t fault_offset = OFFSET_OF(mesure_info_t, bms_charging_info[char_gun_num].bms_fault_reason);
    share_data_write(MESURE_INFO, fault_offset, &bms_fault_reason, sizeof(bms_fault_reason));

    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][BEM].receive_flag);
    share_data_write(CONTROL_INFO, prot_offset, &bem_recv_flag, sizeof(bem_recv_flag));
}

/*****************************************************************************************************
*Function   :can_protocol_rx（CAN协议接收处理）
*Description:遍历每个CAN通道，调用J1939出队函数，如果出队成功（即接收到有效数据），则根据接收到的报文PGN，对其进行对应的
             解析接收处理。
*Input      :
*Output     :
*Returns    :
*Note       :目前这个函数里还仅接收处理了27930-2015的CAN报文。
*****************************************************************************************************/
void can_protocol_rx(void)
{
    CAN_CHNL chnl;
    uint8_t char_gun_num = 0;
    J1939_Data data = {0};
    for(chnl = CAN_DATA_QUEUE_CHNL_1; chnl < CAN_DATA_CHNL_NUM_MAX; chnl++)
    {
        if(chnl == BMS1_CAN)
        {
            char_gun_num = 0;
        }
        // else if(chnl == BMS2_CAN)
        // {
        //     char_gun_num = 1;
        // }
        if(true == J1939_rx_dequeue(chnl, &data))
        {
            if(data.pgn == frame_attribute_27930[BHM].pgn)
            {
                protocol_handle_bhm(char_gun_num, &data);
            }
            else if(data.pgn == frame_attribute_27930[BRM].pgn)
            {
                protocol_handle_brm(char_gun_num, &data);
            }
            else if(data.pgn == frame_attribute_27930[BCP].pgn)
            {
                protocol_handle_bcp(char_gun_num, &data);
            }
            else if(data.pgn == frame_attribute_27930[BRO].pgn)
            {
                protocol_handle_bro(char_gun_num, &data);
            }
            else if(data.pgn == frame_attribute_27930[BCL].pgn)
            {
                protocol_handle_bcl(char_gun_num, &data);
            }
            else if(data.pgn == frame_attribute_27930[BCS].pgn)
            {
                protocol_handle_bcs(char_gun_num, &data);
            }
            else if(data.pgn == frame_attribute_27930[BSM].pgn)
            {
                protocol_handle_bsm(char_gun_num, &data);
            }
            else if(data.pgn == frame_attribute_27930[BMV].pgn)
            {
                /*暂不对BMV报文进行解析*/
            }
            else if(data.pgn == frame_attribute_27930[BMT].pgn)
            {
                /*暂不对BMT报文进行解析*/
            }
            else if(data.pgn == frame_attribute_27930[BSP].pgn)
            {
                /*暂不对BSP报文进行解析*/
            }
            else if(data.pgn == frame_attribute_27930[BST].pgn)
            {
                protocol_handle_bst(char_gun_num, &data);
            }
            else if(data.pgn == frame_attribute_27930[BSD].pgn)
            {
                protocol_handle_bsd(char_gun_num, &data);
            }
            else if(data.pgn == frame_attribute_27930[BEM].pgn)
            {
                protocol_handle_bem(char_gun_num, &data);
            }
            else break;
        }
    }
}

/*****************************************************************************************************
*Function   :can_tx_send_control_27930(CAN发送控制函数针对27930专用)
*Description:先根据输入的枪号确定对应要发送的CAN通道，然后调用J1939入队函数，将报文入队。
*Input      :uint8_t char_gun_num                   充电枪号
             J1939_Data* data                       要发送的J1939数据结构体
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void can_tx_send_control_27930(uint8_t char_gun_num, J1939_Data* data)
{
    CAN_CHNL chnl;
    /*根据枪号，识别对应的CAN通道 */
    if(0 == char_gun_num)
    {
        chnl = BMS1_CAN;
    }
    // else if(1 == char_gun_num)
    // {
    //     chnl = BMS2_CAN;
    // }
    else
    {
        return;
    }
    J1939_tx_enqueue(chnl, data);
}

/*****************************************************************************************************
*Function   :can_send_timer(CAN发送计时函数)
*Description:先根据输入的枪号和报文序号，先检查对应临时报文发送标志位是否被置位，如果被置位，则按照临时报文
             发送规则进行计时，如果达到发送条件则反馈发送使能，如果没有被置位，则按照默认报文发送规则进行计时，如果达到发送
             条件则反馈发送使能。
*Input      :uint8_t char_gun_num                   充电枪号
             uint8_t frame_id                       27930报文序号（数组）
*Output     :
*Returns    :bool      true:达到发送条件  false:未达到发送条件
*Note       :
*****************************************************************************************************/
bool can_send_timer(uint8_t char_gun_num, uint8_t frame_id)
{
    bool ret = false;
    /*从共享区读取临时报文发送标志位*/
    uint8_t send_flag2 = 0;
    uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][frame_id].send_flag2);
    share_data_read(CONTROL_INFO, prot_offset, &send_flag2, sizeof(send_flag2));

    /*如果临时报文发送标志位被置位，则按照临时报文发送规则进行发送*/
    if(send_flag2 == 1)
    {
        /*以下所有报文发送节点都是定时器为0，目的是为了检查到发送标志位置位后，尽可能第一时间发出报文*/
        if(frame_send_timer_27930[char_gun_num][frame_id].send_timer2 == 0)
        {
            ret = true;
        }
        /*这里也要注意下，在临时报文发送期间，默认发送计时器也要同步更新，为了确保临时报文发送期间，不影响默认报文的时间计算*/
        frame_send_timer_27930[char_gun_num][frame_id].send_timer++;
        if(frame_send_timer_27930[char_gun_num][frame_id].send_timer >= frame_attribute_27930[frame_id].send_cycle)
        {
            frame_send_timer_27930[char_gun_num][frame_id].send_timer = 0;
            frame_send_timer_27930[char_gun_num][frame_id].send_count++;
        }

        frame_send_timer_27930[char_gun_num][frame_id].send_timer2++;
        if(frame_send_timer_27930[char_gun_num][frame_id].send_timer2 >= frame_attribute_27930[frame_id].send_cycle2)
        {
            frame_send_timer_27930[char_gun_num][frame_id].send_timer2 = 0;
            frame_send_timer_27930[char_gun_num][frame_id].send_count2++;
            /*如果临时报文有规定有效的发送时间且到达，则取消临时报文发送 */
            if(frame_attribute_27930[frame_id].send_time2 != 0xFFFF
                && frame_send_timer_27930[char_gun_num][frame_id].send_count2 >= frame_attribute_27930[frame_id].send_time2)
            {
                uint8_t send_flag2 = 0;
                uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][frame_id].send_flag2);
                share_data_write(CONTROL_INFO, prot_offset, &send_flag2, sizeof(send_flag2));
                frame_send_timer_27930[char_gun_num][frame_id].send_count2 = 0;
            }
        }
    }
    /*如果临时报文发送标志位没有被置位，则按照默认报文发送规则进行发送*/
    else 
    {
        if(frame_send_timer_27930[char_gun_num][frame_id].send_timer == 0)
        {
            ret = true;
        }
        frame_send_timer_27930[char_gun_num][frame_id].send_timer++;
        if(frame_send_timer_27930[char_gun_num][frame_id].send_timer >= frame_attribute_27930[frame_id].send_cycle)
        {
            frame_send_timer_27930[char_gun_num][frame_id].send_timer = 0;
            frame_send_timer_27930[char_gun_num][frame_id].send_count++;
            /*如果默认报文有规定有效的发送时间且到达，则取消默认报文发送 */
            if(frame_attribute_27930[frame_id].send_time != 0xFFFF
                && frame_send_timer_27930[char_gun_num][frame_id].send_count >= frame_attribute_27930[frame_id].send_time)
            {
                uint8_t send_flag = 0;
                uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[char_gun_num][frame_id].send_flag);
                share_data_write(CONTROL_INFO, prot_offset, &send_flag, sizeof(send_flag));
                frame_send_timer_27930[char_gun_num][CHM].send_count = 0;
            }
        }
    }
    return ret;
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_chm(CHM报文发送处理函数)
*Description:将CHM报文相关属性信息进行赋值，然后从共享内存中读取报文数据信息并填充到报文数据域中，最后调用
             CAN发送控制函数进行发送。
*Input      :uint8_t char_gun_num    充电枪号
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_chm(uint8_t char_gun_num)
{
    J1939_Data frame_chm = {0};
    struct chm_t
    {
        uint8_t    charger_version1;                /*充电机通信版本号byte1*/
        uint8_t    charger_version2;                /*充电机通信版本号byte2*/
        uint8_t    charger_version3;                /*充电机通信版本号byte3*/
    }chm = {0};

    frame_chm.p = frame_attribute_27930[CHM].priority;
    frame_chm.pgn = frame_attribute_27930[CHM].pgn;
    frame_chm.da = BMS_CAN_ADDR;
    frame_chm.sa = CHARGER_CAN_ADDR;
    frame_chm.len = frame_attribute_27930[CHM].size;
    if(true == can_send_timer(char_gun_num, CHM))
    {
        /*达到发送条件，继续往下执行*/
        uint32_t prot_offset = OFFSET_OF(control_info_t, charger_charging_info[char_gun_num].charger_version1);
        share_data_read(CONTROL_INFO, prot_offset, &chm, sizeof(chm));

        frame_chm.data_s[0] = chm.charger_version3;
        frame_chm.data_s[1] = chm.charger_version2;
        frame_chm.data_s[2] = chm.charger_version1;

        can_tx_send_control_27930(char_gun_num, &frame_chm);
    }
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_crm(CRM报文发送处理函数)
*Description:将CRM报文相关属性信息进行赋值，然后调用报文计时函数检查其是否达到发送条件，如达到条件则从共享内存中读取报文
             数据信息并填充到报文数据域中，最后调用CAN发送控制函数进行发送。
*Input      :uint8_t char_gun_num    充电枪号
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_crm(uint8_t char_gun_num)
{
    J1939_Data frame_crm = {0};
    struct crm_t
    {
        uint8_t    bms_recog_result;                /*车辆辨识结果*/
        uint32_t   charger_num;                     /*充电机编号*/
        uint32_t   charger_area_code;               /*充电机区域编码*/
    }crm = {0};

    if(true == can_send_timer(char_gun_num, CRM))
    {
        /*达到发送条件，继续往下执行*/
        uint32_t mesure_offset = OFFSET_OF(control_info_t, charger_charging_info[char_gun_num].bms_recog_result);
        share_data_read(CONTROL_INFO, mesure_offset, &crm, sizeof(crm));

        frame_crm.p = frame_attribute_27930[CRM].priority;
        frame_crm.pgn = frame_attribute_27930[CRM].pgn;
        frame_crm.len = frame_attribute_27930[CRM].size;
        frame_crm.da = BMS_CAN_ADDR;
        frame_crm.sa = CHARGER_CAN_ADDR;
        frame_crm.data_s[0] = crm.bms_recog_result;
        frame_crm.data_s[1] = 0x00 | (crm.charger_num >> 0);
        frame_crm.data_s[2] = 0x00 | (crm.charger_num >> 8);
        frame_crm.data_s[3] = 0x00 | (crm.charger_num >> 16);
        frame_crm.data_s[4] = 0x00 | (crm.charger_num >> 24);
        frame_crm.data_s[5] = 0x00 | (crm.charger_area_code >> 0);
        frame_crm.data_s[6] = 0x00 | (crm.charger_area_code >> 8);
        frame_crm.data_s[7] = 0x00 | (crm.charger_area_code >> 16);

        can_tx_send_control_27930(char_gun_num, &frame_crm);
    }
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_cts(CTS报文发送处理函数)
*Description:将CTS报文相关属性信息进行赋值，然后调用报文计时函数检查其是否达到发送条件，如达到条件则从共享内存中读取报文
             数据信息并填充到报文数据域中，最后调用CAN发送控制函数进行发送。
*Input      :uint8_t char_gun_num    充电枪号
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_cts(uint8_t char_gun_num)
{
    J1939_Data frame_cts = {0};
    struct cts_t
    {
        uint8_t      year;                /*年，2000偏移量*/
        uint8_t      month;               /*月*/
        uint8_t      day;                 /*日*/
        uint8_t      hour;                /*时*/
        uint8_t      minute;              /*分*/
        uint8_t      second;              /*秒*/
    }cts = {0};

    if(true == can_send_timer(char_gun_num, CTS))
    {
        /*达到发送条件，继续往下执行*/
        uint32_t system_offset = OFFSET_OF(system_info_t, system_time);
        share_data_read(SYSTEM_INFO, system_offset, &cts, sizeof(cts));

        frame_cts.p = frame_attribute_27930[CTS].priority;
        frame_cts.pgn = frame_attribute_27930[CTS].pgn;
        frame_cts.len = frame_attribute_27930[CTS].size;
        frame_cts.da = BMS_CAN_ADDR;
        frame_cts.sa = CHARGER_CAN_ADDR;
        frame_cts.data_s[0] = 0x00 | (cts.year + 2000);
        frame_cts.data_s[1] = 0x00 | ((cts.year + 2000) >> 8);
        frame_cts.data_s[2] = cts.month;
        frame_cts.data_s[3] = cts.day;
        frame_cts.data_s[4] = cts.hour;
        frame_cts.data_s[5] = cts.minute;
        frame_cts.data_s[6] = cts.second;

        can_tx_send_control_27930(char_gun_num, &frame_cts);
    }
    
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_cml(CML报文发送处理函数)
*Description:将CML报文相关属性信息进行赋值，然后调用报文计时函数检查其是否达到发送条件，如达到条件则从共享内存中读取报文
             数据信息并填充到报文数据域中，最后调用CAN发送控制函数进行发送。
*Input      :uint8_t char_gun_num    充电枪号
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_cml(uint8_t char_gun_num)
{
    J1939_Data frame_cml = {0};
    struct cml_t
    {
        uint16_t   charger_max_out_vol;             /*充电机最高输出充电电压，V，0.1精度*/
        uint16_t   charger_min_out_vol;             /*充电机最低输出充电电压，V，0.1精度*/
        uint16_t   charger_max_out_cur;             /*充电机最大输出充电电流，A，0.1精度，-400偏移量*/
        uint16_t   charger_min_out_cur;             /*充电机最小输出充电电流，A，0.1精度，-400偏移量*/
    }cml = {0};

    if(true == can_send_timer(char_gun_num, CML))
    {
        /*达到发送条件，继续往下执行*/
        uint32_t mesure_offset = OFFSET_OF(control_info_t, charger_charging_info[char_gun_num].charger_max_out_vol);
        share_data_read(CONTROL_INFO, mesure_offset, &cml, sizeof(cml));

        frame_cml.p = frame_attribute_27930[CML].priority;
        frame_cml.pgn = frame_attribute_27930[CML].pgn;
        frame_cml.len = frame_attribute_27930[CML].size;
        frame_cml.da = BMS_CAN_ADDR;
        frame_cml.sa = CHARGER_CAN_ADDR;
        frame_cml.data_s[0] = 0x00 | (cml.charger_max_out_vol >> 0);
        frame_cml.data_s[1] = 0x00 | (cml.charger_max_out_vol >> 8);
        frame_cml.data_s[2] = 0x00 | (cml.charger_min_out_vol >> 0);
        frame_cml.data_s[3] = 0x00 | (cml.charger_min_out_vol >> 8);
        frame_cml.data_s[4] = 0x00 | (cml.charger_max_out_cur >> 0);
        frame_cml.data_s[5] = 0x00 | (cml.charger_max_out_cur >> 8);
        frame_cml.data_s[6] = 0x00 | (cml.charger_min_out_cur >> 0);
        frame_cml.data_s[7] = 0x00 | (cml.charger_min_out_cur >> 8);

        can_tx_send_control_27930(char_gun_num, &frame_cml);
    }
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_cro(CRO报文发送处理函数)
*Description:将CRO报文相关属性信息进行赋值，然后调用报文计时函数检查其是否达到发送条件，如达到条件则从共享内存中读取报文
             数据信息并填充到报文数据域中，最后调用CAN发送控制函数进行发送。
*Input      :uint8_t char_gun_num    充电枪号
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_cro(uint8_t char_gun_num)
{
    J1939_Data frame_cro = {0};
    uint8_t    charger_cro = 0;                     /*充电机充电准备状态，00未就绪，AA就绪*/

    if(true == can_send_timer(char_gun_num, CRO))
    {
        /*达到发送条件，继续往下执行*/
        uint32_t mesure_offset = OFFSET_OF(control_info_t, charger_charging_info[char_gun_num].charger_cro);
        share_data_read(CONTROL_INFO, mesure_offset, &charger_cro, sizeof(charger_cro));

        frame_cro.p = frame_attribute_27930[CRO].priority;
        frame_cro.pgn = frame_attribute_27930[CRO].pgn;
        frame_cro.len = frame_attribute_27930[CRO].size;
        frame_cro.da = BMS_CAN_ADDR;
        frame_cro.sa = CHARGER_CAN_ADDR;
        frame_cro.data_s[0] = charger_cro;

        can_tx_send_control_27930(char_gun_num, &frame_cro);
    }
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_ccs(CCS报文发送处理函数)
*Description:将CCS报文相关属性信息进行赋值，然后调用报文计时函数检查其是否达到发送条件，如达到条件则从共享内存中读取报文
             数据信息并填充到报文数据域中，最后调用CAN发送控制函数进行发送。
*Input      :uint8_t char_gun_num    充电枪号
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_ccs(uint8_t char_gun_num)
{
    J1939_Data frame_ccs = {0};
    struct ccs_t
    {
        uint16_t   charger_measure_vol;             /*车辆接口当前电压测量值*/
        uint16_t   charger_measure_cur;             /*车辆接口当前电流测量值*/
        uint16_t   charger_charging_time;           /*累计充电时间，min，1精度*/
        uint8_t    charger_charging_allow_status;   /*充电允许状态，00暂停，01允许*/
    }ccs = {0};

    if(true == can_send_timer(char_gun_num, CCS))
    {
        /*达到发送条件，继续往下执行*/
        uint32_t mesure_offset = OFFSET_OF(control_info_t, charger_charging_info[char_gun_num].charger_measure_vol);
        share_data_read(CONTROL_INFO, mesure_offset, &ccs, sizeof(ccs));

        frame_ccs.p = frame_attribute_27930[CCS].priority;
        frame_ccs.pgn = frame_attribute_27930[CCS].pgn;
        frame_ccs.len = frame_attribute_27930[CCS].size;
        frame_ccs.da = BMS_CAN_ADDR;
        frame_ccs.sa = CHARGER_CAN_ADDR;
        frame_ccs.data_s[0] = 0x00 | (ccs.charger_measure_vol >> 0);
        frame_ccs.data_s[1] = 0x00 | (ccs.charger_measure_vol >> 8);
        frame_ccs.data_s[2] = 0x00 | (ccs.charger_measure_cur >> 0);
        frame_ccs.data_s[3] = 0x00 | (ccs.charger_measure_cur >> 8);
        frame_ccs.data_s[4] = 0x00 | (ccs.charger_charging_time >> 0);
        frame_ccs.data_s[5] = 0x00 | (ccs.charger_charging_time >> 8);
        frame_ccs.data_s[6] = ccs.charger_charging_allow_status;

        can_tx_send_control_27930(char_gun_num, &frame_ccs);
    }
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_cst(CST报文发送处理函数)
*Description:将CST报文相关属性信息进行赋值，然后调用报文计时函数检查其是否达到发送条件，如达到条件则从共享内存中读取报文
             数据信息并填充到报文数据域中，最后调用CAN发送控制函数进行发送。
*Input      :uint8_t char_gun_num    充电枪号
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_cst(uint8_t char_gun_num)
{
    J1939_Data frame_cst = {0};
    struct cst_t
    {
        charger_stop_reason_e    charger_stop_reason;    /*充电机终止充电原因*/
        charger_fault_reason_e   charger_fault_reason;   /*充电机终止充电故障原因*/
    }cst;

    if(true == can_send_timer(char_gun_num, CST))
    { 
        uint32_t mesure_offset = OFFSET_OF(control_info_t, charger_charging_info[char_gun_num].charger_stop_reason);
        share_data_read(CONTROL_INFO, mesure_offset, &cst, sizeof(cst));

        frame_cst.p = frame_attribute_27930[CST].priority;
        frame_cst.pgn = frame_attribute_27930[CST].pgn;
        frame_cst.len = frame_attribute_27930[CST].size;
        frame_cst.da = BMS_CAN_ADDR;
        frame_cst.sa = CHARGER_CAN_ADDR;
        frame_cst.data_s[0] = 0x01 << ((cst.charger_stop_reason - 1) * 2);
        if(0 != cst.charger_fault_reason && cst.charger_fault_reason <= C_PARAMETER_MISMATCH)
        {
            switch(cst.charger_fault_reason)
            {
                case CHARGER_OVERHEAT:
                    frame_cst.data_s[1] |= 0x01 << 0;
                    break;
                case C_CONNECTOR_FAULT:
                    frame_cst.data_s[1] |= 0x01 << 2;
                    break;
                case OVERHEAT_INSIDE_CHARGER:
                    frame_cst.data_s[1] |= 0x01 << 4;
                    break;
                case REQ_CUR_CANNOT_TRANS:
                    frame_cst.data_s[1] |= 0x01 << 6;
                    break;
                case EMERGENCY_STOP_FAULT:
                    frame_cst.data_s[2] |= 0x01 << 0;
                    break;
                case C_OTHER_FAULT:
                    frame_cst.data_s[2] |= 0x01 << 2;
                    break;
                case SELF_CHECK_FAULT:
                    frame_cst.data_s[2] |= 0x01 << 4;
                    break;
                case PRECHARGE_FAULT:
                    frame_cst.data_s[2] |= 0x01 << 6;
                    break;
                case C_CUR_MISMATCH:
                    frame_cst.data_s[3] |= 0x01 << 0;
                    break;
                case C_VOL_ABNORMAL:
                    frame_cst.data_s[3] |= 0x01 << 2;
                    break;
                case C_PARAMETER_MISMATCH:
                    frame_cst.data_s[3] |= 0x01 << 4;
                    break;
                default:
                    break;
            }
        }

        can_tx_send_control_27930(char_gun_num, &frame_cst);
    }
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_csd(CSD报文发送处理函数)
*Description:将CSD报文相关属性信息进行赋值，然后调用报文计时函数检查其是否达到发送条件，如达到条件则从共享内存中读取报文
             数据信息并填充到报文数据域中，最后调用CAN发送控制函数进行发送。
*Input      :uint8_t char_gun_num    充电枪号
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_csd(uint8_t char_gun_num)
{
    J1939_Data frame_csd = {0};
    struct csd_t
    {
        uint16_t   charger_total_charging_time;     /*本次充电时间，min，1精度*/
        uint16_t   charger_total_charging_energy;   /*本次充电输出能量，kWh，0.1精度*/
    }csd = {0};
    uint32_t   charger_num = 0;                     /*充电机编号*/

    if(true == can_send_timer(char_gun_num, CSD))
    {
        uint32_t mesure_offset = OFFSET_OF(control_info_t, charger_charging_info[char_gun_num].charger_total_charging_time);
        share_data_read(CONTROL_INFO, mesure_offset, &csd, sizeof(csd));

        uint32_t charger_num_offset = OFFSET_OF(control_info_t, charger_charging_info[char_gun_num].charger_num);
        share_data_read(CONTROL_INFO, charger_num_offset, &charger_num, sizeof(charger_num));
        frame_csd.p = frame_attribute_27930[CSD].priority;
        frame_csd.pgn = frame_attribute_27930[CSD].pgn;
        frame_csd.len = frame_attribute_27930[CSD].size;
        frame_csd.da = BMS_CAN_ADDR;
        frame_csd.sa = CHARGER_CAN_ADDR;
        frame_csd.data_s[0] = 0x00 | (csd.charger_total_charging_time >> 0);
        frame_csd.data_s[1] = 0x00 | (csd.charger_total_charging_time >> 8);
        frame_csd.data_s[2] = 0x00 | (csd.charger_total_charging_energy >> 0);
        frame_csd.data_s[3] = 0x00 | (csd.charger_total_charging_energy >> 8);
        frame_csd.data_s[4] = 0x00 | (charger_num >> 0);
        frame_csd.data_s[5] = 0x00 | (charger_num >> 8);
        frame_csd.data_s[6] = 0x00 | (charger_num >> 16);
        frame_csd.data_s[7] = 0x00 | (charger_num >> 24);

        can_tx_send_control_27930(char_gun_num, &frame_csd);
    }
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_cem(CEM报文发送处理函数)
*Description:将CEM报文相关属性信息进行赋值，然后调用报文计时函数检查其是否达到发送条件，如达到条件则从共享内存中读取报文
             数据信息并填充到报文数据域中，最后调用CAN发送控制函数进行发送。
*Input      :uint8_t char_gun_num    充电枪号
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_cem(uint8_t char_gun_num)
{
    J1939_Data frame_cem = {0};
    charger_fault_reason_e   charger_fault_reason;   /*充电机终止充电故障原因*/

    if(true == can_send_timer(char_gun_num, CEM))
    { 
        uint32_t mesure_offset = OFFSET_OF(control_info_t, charger_charging_info[char_gun_num].charger_fault_reason);
        share_data_read(CONTROL_INFO, mesure_offset, &charger_fault_reason, sizeof(charger_fault_reason));

        frame_cem.p = frame_attribute_27930[CEM].priority;
        frame_cem.pgn = frame_attribute_27930[CEM].pgn;
        frame_cem.len = frame_attribute_27930[CEM].size;
        frame_cem.da = 0xF4;
        frame_cem.sa = 0x56;
        if(0 != charger_fault_reason && charger_fault_reason >= RX_BRM_TIMEOUT)
        {
            switch (charger_fault_reason)
            {
                case RX_BRM_TIMEOUT:
                    frame_cem.data_s[0] |= 0x01 << 0;
                    break;
                case RX_BCP_TIMEOUT:
                    frame_cem.data_s[1] |= 0x01 << 0;
                    break;
                case RX_BRO_TIMEOUT:
                    frame_cem.data_s[1] |= 0x01 << 2;
                    break;
                case RX_BCS_TIMEOUT:
                    frame_cem.data_s[2] |= 0x01 << 0;
                    break;
                case RX_BCL_TIMEOUT:
                    frame_cem.data_s[2] |= 0x01 << 2;
                    break;
                case RX_BST_TIMEOUT:
                    frame_cem.data_s[2] |= 0x01 << 4;
                    break;
                case RX_BSD_TIMEOUT:
                    frame_cem.data_s[3] |= 0x01 << 0;
                    break;
                case RX_BSM_TIMEOUT:
                    frame_cem.data_s[3] |= 0x01 << 2;
                    break;
                default:
                    break;
            }
        }

        can_tx_send_control_27930(char_gun_num, &frame_cem);
    }
}

/*****************************************************************************************************
*Function   :can_protocol_tx（CAN协议发送处理）
*Description:这个函数是计划遍历所有的报文控制属性表，然后检查报文发送标志位，如果发现报文发送标志位被置位，
             则调用对应的报文发送处理函数进行发送。
*Input      :
*Output     :
*Returns    :
*Note       :目前这个函数里还仅发送处理了27930-2015的CAN报文。
*****************************************************************************************************/
void can_protocol_tx(void)
{
    /*把完成的报文控制属性表从共享内存中读取出来*/
    control_info_t control_info;
    share_data_read(CONTROL_INFO, 0, &control_info, sizeof(control_info_t));

    /*以下是27930报文发送遍历处理*/
    uint8_t  frame;
    uint8_t char_gun_num;
    for(char_gun_num = 0; char_gun_num < CHAR_GUN_NUM; char_gun_num++)
    {
        for(frame = 0; frame < CAN27930MAX; frame++)
        {
            uint8_t bms_send_flag = control_info.bms_can_control[char_gun_num][frame].send_flag;
            if(bms_send_flag == 1)
            {
                switch (frame)
                {
                    case CHM:
                        protocol_tx_handle_chm(char_gun_num);
                        break;
                   case CRM:
                       protocol_tx_handle_crm(char_gun_num);
                       break;
                    case CTS:
                        protocol_tx_handle_cts(char_gun_num);
                        break;
                   case CML:
                       protocol_tx_handle_cml(char_gun_num);
                       break;
                   case CRO:
                       protocol_tx_handle_cro(char_gun_num);
                       break;
                   case CCS:
                       protocol_tx_handle_ccs(char_gun_num);
                       break;
                   case CST:
                       protocol_tx_handle_cst(char_gun_num);
                       break;
                   case CSD:
                       protocol_tx_handle_csd(char_gun_num);
                       break;
                   case CEM:
                       protocol_tx_handle_cem(char_gun_num);
                       break;
                    default:
                        break;
                }
            }
            /*这里要特别注意下，如果发现报文发送标志位为置为0，则说明该报文已经发送完成，则将报文发送定时器清零*/
            else
            {
                memset(&frame_send_timer_27930[char_gun_num][frame], 0, sizeof(frame_send_timer_t));
            }
        }
    }
}
/*****************************************************************************************************
*Function   :can_protocol_handle_task（CAN协议处理任务）
*Description:该函数负责从J1939接收队列中接收CAN报文数据并对其进行解析处理，同时负责轮询CAN报文控制属性表，
             按照通讯协议要求对CAN报文进行数据整理并发送。
*Input      :
*Output     :
*Returns    :
*Note       :内部包含计数器，对任务周期要求为1ms
*****************************************************************************************************/
void can_protocol_handle_task(void)
{
    can_protocol_rx();
    can_protocol_tx();
}
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
