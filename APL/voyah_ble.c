/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : voyah_ble.c
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.05.30
* Description       :
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "voyah_ble.h"
/* ==============================  DEFINES   =============================== */
// 32位循环左移
#define ROTL32(x,n) (((x) << (n)) | ((x) >> (32-(n))))
/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */
/*对蓝牙报文属性进行定义*/
const ble_frame_attribute_t ble_frame_attribute[BLE_FRAME_MAX] = 
{               /*           cmd_num       长度      默认发送周期  默认发送次数  临时发送周期  临时发送次数    */
    /*BLE_AUTHOR*/          { 0x01,        20,        250,        0xFFFF,      250,         0xFFFF     },
    /*BLE_AUTHOR_ACK1*/     { 0x01,        4,         250,        0xFFFF,           250,         0          },
    /*BLE_AUTHOR_ACK2*/     { 0x02,        5,         250,        0,           250,         0          },
    /*BLE_AUTO_CHARGE*/     { 0x03,        4,         250,        0xFFFF,      250,         0xFFFF     },
    /*BLE_AUTO_CHARGE_ACK*/ { 0x03,        6,         500,        0,           500,         0          },
};

/*定义所有枪及其所有蓝牙报文的发送定时器*/
ble_frame_send_timer_t ble_frame_send_timer[BLE_FRAME_MAX] = {0};     /*蓝牙报文发送定时器*/

/*AES S-box 代换表*/
static const uint8_t sbox[256] = {
    0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
    0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
    0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
    0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
    0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
    0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
    0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
    0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
    0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
    0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
    0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
    0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
    0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
    0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
    0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
    0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
};

/*AES S-box 逆向代换表*/
static const uint8_t rsbox[256] = {
    0x52,0x09,0x6A,0xD5,0x30,0x36,0xA5,0x38,0xBF,0x40,0xA3,0x9E,0x81,0xF3,0xD7,0xFB,
    0x7C,0xE3,0x39,0x82,0x9B,0x2F,0xFF,0x87,0x34,0x8E,0x43,0x44,0xC4,0xDE,0xE9,0xCB,
    0x54,0x7B,0x94,0x32,0xA6,0xC2,0x23,0x3D,0xEE,0x4C,0x95,0x0B,0x42,0xFA,0xC3,0x4E,
    0x08,0x2E,0xA1,0x66,0x28,0xD9,0x24,0xB2,0x76,0x5B,0xA2,0x49,0x6D,0x8B,0xD1,0x25,
    0x72,0xF8,0xF6,0x64,0x86,0x68,0x98,0x16,0xD4,0xA4,0x5C,0xCC,0x5D,0x65,0xB6,0x92,
    0x6C,0x70,0x48,0x50,0xFD,0xED,0xB9,0xDA,0x5E,0x15,0x46,0x57,0xA7,0x8D,0x9D,0x84,
    0x90,0xD8,0xAB,0x00,0x8C,0xBC,0xD3,0x0A,0xF7,0xE4,0x58,0x05,0xB8,0xB3,0x45,0x06,
    0xD0,0x2C,0x1E,0x8F,0xCA,0x3F,0x0F,0x02,0xC1,0xAF,0xBD,0x03,0x01,0x13,0x8A,0x6B,
    0x3A,0x91,0x11,0x41,0x4F,0x67,0xDC,0xEA,0x97,0xF2,0xCF,0xCE,0xF0,0xB4,0xE6,0x73,
    0x96,0xAC,0x74,0x22,0xE7,0xAD,0x35,0x85,0xE2,0xF9,0x37,0xE8,0x1C,0x75,0xDF,0x6E,
    0x47,0xF1,0x1A,0x71,0x1D,0x29,0xC5,0x89,0x6F,0xB7,0x62,0x0E,0xAA,0x18,0xBE,0x1B,
    0xFC,0x56,0x3E,0x4B,0xC6,0xD2,0x79,0x20,0x9A,0xDB,0xC0,0xFE,0x78,0xCD,0x5A,0xF4,
    0x1F,0xDD,0xA8,0x33,0x88,0x07,0xC7,0x31,0xB1,0x12,0x10,0x59,0x27,0x80,0xEC,0x5F,
    0x60,0x51,0x7F,0xA9,0x19,0xB5,0x4A,0x0D,0x2D,0xE5,0x7A,0x9F,0x93,0xC9,0x9C,0xEF,
    0xA0,0xE0,0x3B,0x4D,0xAE,0x2A,0xF5,0xB0,0xC8,0xEB,0xBB,0x3C,0x83,0x53,0x99,0x61,
    0x17,0x2B,0x04,0x7E,0xBA,0x77,0xD6,0x26,0xE1,0x69,0x14,0x63,0x55,0x21,0x0C,0x7D
};

/*AES轮常数表*/
static const uint32_t rcon[10] = {
    0x01000000,0x02000000,0x04000000,0x08000000,0x10000000,
    0x20000000,0x40000000,0x80000000,0x1B000000,0x36000000
};

static uint8_t ble_rx_buf[UART_DATA_MAX];      /*BLE03发送缓冲区，大小根据实际需求定义*/

bool ble_init_flag = false;   /*BLE03初始化标志位*/
atk_ble03_init_data_t ble_init_data = 
{
    .ble_name = "VOYAHROBOT1234",
    .spp_name = "VOYAHROBOT1234",
    .ble_mac = "226644889922",
    .uuid = "FF00",
    .uuid1 = "FF01",
    .uuid2 = "FF02",
    .uuid3 = "FF03",
};

/* 车辆VIN码的唯一定义（避免在头文件中重复定义） */
uint8_t VIN[17] = "LMMMMMMMMMMMM1102";   /*车辆VIN码，长度为17字节*/
/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :voyah_ble_send_timer(蓝牙发送计时函数)
*Description:先根据输入报文序号，先检查对应临时报文发送标志位是否被置位，如果被置位，则按照临时报文
             发送规则进行计时，如果达到发送条件则反馈发送使能，如果没有被置位，则按照默认报文发送规则进行计时，如果达到发送
             条件则反馈发送使能。
*Input      :uint8_t frame_id                       报文序号（数组）
*Output     :
*Returns    :bool      true:达到发送条件  false:未达到发送条件
*Note       :
*****************************************************************************************************/
bool voyah_ble_send_timer(uint8_t frame_id)
{
    bool ret = false;
    /*从共享区读取临时报文发送标志位*/
    uint8_t send_flag2 = 0;
    uint32_t prot_offset = OFFSET_OF(control_info_t, ble_uart_control[frame_id].send_flag2);
    share_data_read(CONTROL_INFO, prot_offset, &send_flag2, sizeof(send_flag2));

    /*如果临时报文发送标志位被置位，则按照临时报文发送规则进行发送*/
    if(send_flag2 == 1)
    {
        /*以下所有报文发送节点都是定时器为0，目的是为了检查到发送标志位置位后，尽可能第一时间发出报文*/
        if(ble_frame_send_timer[frame_id].send_timer2 == 0)
        {
            ret = true;
        }
        /*这里也要注意下，在临时报文发送期间，默认发送计时器也要同步更新，为了确保临时报文发送期间，不影响默认报文的时间计算*/
        ble_frame_send_timer[frame_id].send_timer++;
        if(ble_frame_send_timer[frame_id].send_timer >= ble_frame_attribute[frame_id].send_cycle)
        {
            ble_frame_send_timer[frame_id].send_timer = 0;
            ble_frame_send_timer[frame_id].send_count++;
        }

        ble_frame_send_timer[frame_id].send_timer2++;
        if(ble_frame_send_timer[frame_id].send_timer2 >= ble_frame_attribute[frame_id].send_cycle2)
        {
            ble_frame_send_timer[frame_id].send_timer2 = 0;
            ble_frame_send_timer[frame_id].send_count2++;
            /*如果临时报文有规定有效的发送时间且到达，则取消临时报文发送 */
            if(ble_frame_attribute[frame_id].send_time2 != 0xFFFF
                && ble_frame_send_timer[frame_id].send_count2 >= ble_frame_attribute[frame_id].send_time2)
            {
                uint8_t send_flag2 = 0;
                uint32_t prot_offset = OFFSET_OF(control_info_t, ble_uart_control[frame_id].send_flag2);
                share_data_write(CONTROL_INFO, prot_offset, &send_flag2, sizeof(send_flag2));
                ble_frame_send_timer[frame_id].send_count2 = 0;
            }
        }
    }
    /*如果临时报文发送标志位没有被置位，则按照默认报文发送规则进行发送*/
    else 
    {
        if(ble_frame_send_timer[frame_id].send_timer == 0)
        {
            ret = true;
        }
        ble_frame_send_timer[frame_id].send_timer++;
        if(ble_frame_send_timer[frame_id].send_timer >= ble_frame_attribute[frame_id].send_cycle)
        {
            ble_frame_send_timer[frame_id].send_timer = 0;
            ble_frame_send_timer[frame_id].send_count++;
            /*如果默认报文有规定有效的发送时间且到达，则取消默认报文发送 */
            if(ble_frame_attribute[frame_id].send_time != 0xFFFF
                && ble_frame_send_timer[frame_id].send_count >= ble_frame_attribute[frame_id].send_time)
            {
                uint8_t send_flag = 0;
                uint32_t prot_offset = OFFSET_OF(control_info_t, ble_uart_control[frame_id].send_flag);
                share_data_write(CONTROL_INFO, prot_offset, &send_flag, sizeof(send_flag));
                ble_frame_send_timer[frame_id].send_count = 0;
            }
        }
    }
    return ret;
}

/*****************************************************************************************************
*Function   :16字节密钥生成函数
*Description:16字节密钥生成函数，前12字节为固定值，后4字节为VIN码的最后4字节
*Input      :uint8_t* vin    车辆VIN码，长度为17字节
*Output     :uint8_t key[AES_KEY_SIZE]  生成的AES加密密钥，长度为16字节
*Returns    :
*Note       :
*****************************************************************************************************/
void ble_gen_key(const uint8_t* vin, uint8_t key[AES_KEY_SIZE])
{
    /*生成AES加密密钥，长度为16字节，前12字节为固定值，后4字节为VIN码的最后4字节*/
    memcpy(key, FIXED_KEY, FIXED_KEY_LEN);
    memcpy(key + FIXED_KEY_LEN, vin + 13, 4);
}

/*****************************************************************************************************
*Function   :PKCS7数据填充函数
*Description:报文数据填充，有效数据不足16字节*N时，进行数据填充，填充方式为PKCS7
*Input      :uint8_t data_len  明文数据长度（字节）
             uint8_t* data     待填充的明文数据缓冲区指针，大小需至少为 data_len + (16 - data_len % 16) 字节
*Output     :uint8_t* data     填充后的数据缓冲区
*Returns    :填充后的数据长度（字节）
*Note       :
*****************************************************************************************************/
uint8_t ble_pkcs7_pad(uint8_t* data, uint8_t data_len)
{
    /*报文数据填充，有效数据不足16字节*N时，进行数据填充，填充方式为PKCS7*/
    uint8_t pad_len = (data_len % AES_BLOCK_SIZE) ? (AES_BLOCK_SIZE - (data_len % AES_BLOCK_SIZE)) : AES_BLOCK_SIZE;
    for(uint8_t i = 0; i < pad_len; i++)
    {
        data[data_len + i] = pad_len;
    }
    return data_len + pad_len;
}

/*****************************************************************************************************
*Function   :CRC16-IBM算法函数
*Description:CRC16-IBM算法实现，初始值为0x0000，输入数据不反转，输出CRC不反转，最终结果取反
*Input      :uint8_t* data  待计算CRC的数据缓冲区指针
             uint8_t len    待计算CRC的数据长度（字节）
*Output     :
*Returns    :CRC16计算结果
*Note       :
*****************************************************************************************************/
uint16_t ble_crc16_ibm(const uint8_t* data, uint8_t len)
{
    /*CRC16-IBM算法实现，初始值为0x0000，输入数据不反转，输出CRC不反转，最终结果取反*/
    uint16_t crc = 0x0000;
    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc = (crc >> 1) ^ 0xA001; // CRC-16-IBM
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/*****************************************************************************************************
*Function   :AES密钥扩展
*Description:AES-128密钥扩展函数，将16字节的AES密钥扩展为44个32位字（轮密钥）
*Input      :uint8_t *key  AES加密密钥，长度为16字节
*Output     :uint32_t *w   扩展后的轮密钥数组，长度为44个32位字
*Returns    :
*Note       :
*****************************************************************************************************/
static void aes_key_expand(const uint8_t *key, uint32_t *w)
{
    int i;
    for (i = 0; i < 4; i++)
        w[i] = (key[i*4] << 24) | (key[i*4+1] << 16) | (key[i*4+2] << 8) | key[i*4+3];

    for (i = 4; i < 44; i++)
    {
        uint32_t temp = w[i-1];
        if (i % 4 == 0)
        {
            temp = ROTL32(temp, 8);
            temp = (sbox[(temp>>24)&0xFF]<<24) | (sbox[(temp>>16)&0xFF]<<16) |
                   (sbox[(temp>>8)&0xFF]<<8) | sbox[temp&0xFF];
            temp ^= rcon[i/4 - 1];
        }
        w[i] = w[i-4] ^ temp;
    }
}

/*****************************************************************************************************
*Function   :AES字节替换
*Description:AES字节替换函数，使用AES S-box对状态缓冲区中的每个字节进行替换
*Input      :uint8_t *state   AES加密过程中的状态缓冲区，长度为16字节
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void aes_subbytes(uint8_t *state)
{
    for (int i = 0; i < AES_BLOCK_SIZE; i++)
    {
        state[i] = sbox[state[i]];
    }
}

/*****************************************************************************************************
*Function   :AES行移位
*Description:AES行移位函数，对状态缓冲区中的字节进行行移位操作，第一行不变，第二行左移1字节，第三行左移2字节，
            第四行左移3字节
*Input      :uint8_t *state   AES加密过程中的状态缓冲区，长度为16字节
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void aes_shiftrows(uint8_t *state)
{
    uint8_t t;
    t = state[1]; state[1] = state[5]; state[5] = state[9]; state[9] = state[13]; state[13] = t;
    t = state[2]; state[2] = state[10]; state[10] = t;
    t = state[6]; state[6] = state[14]; state[14] = t;
    t = state[3]; state[3] = state[15]; state[15] = state[11]; state[11] = state[7]; state[7] = t;
}

/*****************************************************************************************************
*Function   :AES GF(2^8) 乘法
*Description:AES GF(2^8) 乘法函数，在AES列混合过程中使用，计算两个字节在GF(2^8)上的乘积
*Input      :uint8_t a   乘数a
             uint8_t b   乘数b
*Output     :
*Returns    :乘积结果
*Note       :
*****************************************************************************************************/
static uint8_t aes_gmul(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    for (int i = 0; i < 8; i++)
    {
        if (b & 1) p ^= a;
        uint8_t hi = a & 0x80;
        a <<= 1;
        if (hi) a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

/*****************************************************************************************************
*Function   :AES列混合
*Description:AES列混合函数，对状态缓冲区中的字节进行列混合操作，每列的4个字节通过GF(2^8)乘法和异或运算进行混合
*Input      :uint8_t *state  AES加密过程中的状态缓冲区，长度为16字节
*Output     :uint8_t *state  AES加密过程中的状态缓冲区，长度为16字节
*Returns    :
*Note       :
*****************************************************************************************************/
static void aes_mixcolumns(uint8_t *state)
{
    for (int i = 0; i < 16; i += 4)
    {
        uint8_t a = state[i], b = state[i+1], c = state[i+2], d = state[i+3];
        state[i]   = aes_gmul(a,2) ^ aes_gmul(b,3) ^ c ^ d;
        state[i+1] = a ^ aes_gmul(b,2) ^ aes_gmul(c,3) ^ d;
        state[i+2] = a ^ b ^ aes_gmul(c,2) ^ aes_gmul(d,3);
        state[i+3] = aes_gmul(a,3) ^ b ^ c ^ aes_gmul(d,2);
    }
}

/*****************************************************************************************************
*Function   :AES轮密钥加
*Description:把当前轮的轮密钥与状态缓冲区进行异或运算，完成AES加密过程中的轮密钥加步骤
*Input      :uint32_t *w  AES密钥扩展后生成的轮密钥数组，长度为44个32位字
             int round    当前轮数，范围为0-10
*Output     :uint8_t *state   AES加密过程中的状态缓冲区，长度为16字节
*Returns    :
*Note       :
*****************************************************************************************************/
static void aes_addroundkey(uint8_t *state, const uint32_t *w, int round)
{
    for (int i = 0; i < 4; i++)
    {
        uint32_t k = w[round*4 + i];
        state[i*4]   ^= (k >> 24) & 0xFF;
        state[i*4+1] ^= (k >> 16) & 0xFF;
        state[i*4+2] ^= (k >> 8) & 0xFF;
        state[i*4+3] ^= k & 0xFF;
    }
}

/*****************************************************************************************************
*Function   :AES128-ECB 单块加密（16字节）
*Description:
*Input      :uint8_t *in   待加密的16字节明文数据缓冲区指针
             uint8_t *key  AES加密密钥，长度为16字节
*Output     :uint8_t *out  加密后的16字节密文数据缓冲区指针
*Returns    :
*Note       :这些地方涉及AES128-ECB加密算法，几乎看不懂。就在这里崩了
*****************************************************************************************************/
void aes128_ecb_encrypt(const uint8_t *in, uint8_t *out, const uint8_t *key)
{
    uint32_t w[44];     /*AES-128密钥扩展后生成的44个32位字（轮密钥）*/
    uint8_t state[AES_BLOCK_SIZE];   /*AES加密过程中的状态缓冲区*/

    // 密钥扩展
    aes_key_expand(key, w);
    memcpy(state, in, AES_BLOCK_SIZE);

    // 初始轮密钥加
    aes_addroundkey(state, w, 0);

    // 10轮加密
    for (int round = 1; round <= 10; round++)
    {
        aes_subbytes(state);
        aes_shiftrows(state);
        if (round < 10)
            aes_mixcolumns(state);
        aes_addroundkey(state, w, round);
    }

    memcpy(out, state, AES_BLOCK_SIZE);
}

/*****************************************************************************************************
*Function   :ble_pack（BLE报文封装函数）
*Description:BLE报文封装：业务数据 → 发送帧
*Input      :ble_data_t *data  待发送的明文数据结构体指针
             uint8_t *vin      车辆VIN码，长度为17字节
*Output     :ble_frame_data_t *frame  最终输出的蓝牙报文结构体指针
*Returns    :最终输出报文的总长度（字节）
*Note       :
*****************************************************************************************************/
uint8_t ble_pack(const ble_plain_t *data, const uint8_t *vin, ble_frame_data_t *frame)
{
    uint8_t key[AES_KEY_SIZE] = {0}; /*AES加密密钥*/
    uint8_t pad_buf[BT_MAX_PLAIN_LEN] = {0}; /*填充后的数据缓冲区，大小根据实际需求定义*/

    // 2. PKCS7填充
    memcpy(pad_buf, data->data, data->plain_len);
    frame->encrypt_len = ble_pkcs7_pad(pad_buf, data->plain_len);

    // 3. 生成密钥并加密
    ble_gen_key(vin, key);
    for (int i = 0; i < frame->encrypt_len; i += AES_BLOCK_SIZE)
    {
        aes128_ecb_encrypt(pad_buf + i, frame->encrypt_data + i, key);
    }

    // 4. 填充帧头与长度
    frame->frame_head = BLE_FRAME_HEAD;
    frame->effective_len  = data->plain_len;

    // 5. 计算CRC：Len + 加密数据
    uint8_t crc_buf[1 + BT_MAX_ENC_LEN];
    crc_buf[0] = frame->effective_len;
    memcpy(&crc_buf[1], frame->encrypt_data, frame->encrypt_len);
    frame->check_num = ble_crc16_ibm(crc_buf, 1 + frame->encrypt_len);

    // 返回总帧长度
    return 1 + 1 + frame->encrypt_len + 2;
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_author_ack1（鉴权应答1）
*Description:从共享内容中读取鉴权结果和车辆VIN码，封装成蓝牙报文并发送
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_author_ack1(void)
{
    if(true == voyah_ble_send_timer(BLE_AUTHOR_ACK1))
    {
        author_ret_e             author_ret;             /*鉴权结果*/
        uint32_t mesure_offset = OFFSET_OF(mesure_info_t, ble_frame.author_ret);
        share_data_read(MESURE_INFO, mesure_offset, &author_ret, sizeof(author_ret_e));

        ble_plain_t              plain_data;      /*蓝牙报文明文数据结构体*/
        plain_data.plain_len = ble_frame_attribute[BLE_AUTHOR_ACK1].size;
        plain_data.data[0] = CHAEGER_BLE_ADDR;    /*充电机蓝牙协议地址*/
        plain_data.data[1] = VEHICLE_BLE_ADDR;    /*车辆蓝牙协议地址*/
        plain_data.data[2] = ble_frame_attribute[BLE_AUTHOR_ACK1].cmd_num; /*报文命令字*/
    //    plain_data.data[3] = (uint8_t)author_ret; /*鉴权结果*/
        plain_data.data[3] = GUN_DISCONNECTED;        /*鉴权结果*/
        ble_frame_data_t         frame_data;      /*蓝牙报文最终发送数据结构体*/

        // 发送
        uart_data_s ble_tx_data = {0};
        ble_tx_data.len = ble_pack(&plain_data, VIN, &frame_data);
        ble_tx_data.data[0] = frame_data.frame_head;
        ble_tx_data.data[1] = frame_data.effective_len;
        memcpy(&ble_tx_data.data[2], frame_data.encrypt_data, frame_data.encrypt_len);
        ble_tx_data.data[2 + frame_data.encrypt_len] = frame_data.check_num & 0xFF;
        ble_tx_data.data[3 + frame_data.encrypt_len] = (frame_data.check_num >> 8) & 0xFF;
        ble_tx_data.timeout = ble_tx_data.len;
        uart_tx_enqueue(UART_DATA_QUEUE_CHNL_1, &ble_tx_data);
    }
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_author_ack2（鉴权应答2）
*Description:从共享内容中读取鉴权失败原因和设备类型，封装成蓝牙报文并发送
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_author_ack2(void)
{
    struct
    {
        author_fail_reason_e     fail_reason;            /*鉴权失败原因*/
        charger_device_type_e    device_type;            /*设备类型*/
    }author_ack2;
    uint32_t mesure_offset = OFFSET_OF(mesure_info_t, ble_frame.fail_reason);
    share_data_read(MESURE_INFO, mesure_offset, &author_ack2, sizeof(author_ack2));

    ble_plain_t              plain_data;      /*蓝牙报文明文数据结构体*/
    plain_data.plain_len = ble_frame_attribute[BLE_AUTHOR_ACK2].size;
    plain_data.data[0] = CHAEGER_BLE_ADDR;    /*充电机蓝牙协议地址*/
    plain_data.data[1] = VEHICLE_BLE_ADDR;    /*车辆蓝牙协议地址*/
    plain_data.data[2] = ble_frame_attribute[BLE_AUTHOR_ACK2].cmd_num; /*报文命令字*/
    plain_data.data[3] = (uint8_t)author_ack2.device_type; /*设备类型*/
    plain_data.data[4] = (uint8_t)author_ack2.fail_reason; /*鉴权失败原因*/

    ble_frame_data_t         frame_data;      /*蓝牙报文最终发送数据结构体*/

    // 发送
    uart_data_s ble_tx_data = {0};
    ble_tx_data.len = ble_pack(&plain_data, VIN, &frame_data);
    ble_tx_data.data[0] = frame_data.frame_head;
    ble_tx_data.data[1] = frame_data.effective_len;
    memcpy(&ble_tx_data.data[2], frame_data.encrypt_data, frame_data.encrypt_len);
    ble_tx_data.data[2 + frame_data.encrypt_len] = (frame_data.check_num >> 8) & 0xFF;
    ble_tx_data.data[3 + frame_data.encrypt_len] = frame_data.check_num & 0xFF;
    ble_tx_data.timeout = ble_tx_data.len;
    uart_tx_enqueue(UART_DATA_QUEUE_CHNL_1, &ble_tx_data);
}

/*****************************************************************************************************
*Function   :protocol_tx_handle_auto_charge_ack（自动充电请求应答）
*Description:从共享内容中读取自动充电请求应答信息，封装成蓝牙报文并发送
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void protocol_tx_handle_auto_charge_ack(void)
{
    struct
    {
        vehicle_position_e       vehicle_position;       /*车辆位置判断*/
        charger_arm_status_e     charger_arm_status;     /*机械臂运行状态*/
        arm_fail_reason_e        arm_fail_reason;        /*机械臂失败原因*/
    }auto_charge_ack;

    if(ble_frame_send_timer[BLE_AUTO_CHARGE_ACK].send_timer <= ble_frame_attribute[BLE_AUTO_CHARGE_ACK].send_time)
    {
        ble_frame_send_timer[BLE_AUTO_CHARGE_ACK].send_timer++;
        return;
    }
    else
    {
        uint32_t mesure_offset = OFFSET_OF(mesure_info_t, ble_frame.vehicle_position);
        share_data_read(MESURE_INFO, mesure_offset, &auto_charge_ack, sizeof(auto_charge_ack));

        ble_plain_t              plain_data;      /*蓝牙报文明文数据结构体*/
        plain_data.plain_len = ble_frame_attribute[BLE_AUTO_CHARGE_ACK].size;
        plain_data.data[0] = CHAEGER_BLE_ADDR;    /*充电机蓝牙协议地址*/
        plain_data.data[1] = VEHICLE_BLE_ADDR;    /*车辆蓝牙协议地址*/
        plain_data.data[2] = ble_frame_attribute[BLE_AUTO_CHARGE_ACK].cmd_num; /*报文命令字*/
        plain_data.data[3] = (uint8_t)auto_charge_ack.vehicle_position; /*车辆位置判断*/
        plain_data.data[4] = (uint8_t)auto_charge_ack.charger_arm_status; /*机械臂运行状态*/
        plain_data.data[5] = (uint8_t)auto_charge_ack.arm_fail_reason; /*机械臂失败原因*/

        ble_frame_data_t         frame_data;      /*蓝牙报文最终发送数据结构体*/

        // 发送
        uart_data_s ble_tx_data = {0};
        ble_tx_data.len = ble_pack(&plain_data, VIN, &frame_data);
        ble_tx_data.data[0] = frame_data.frame_head;
        ble_tx_data.data[1] = frame_data.effective_len;
        memcpy(&ble_tx_data.data[2], frame_data.encrypt_data, frame_data.encrypt_len);
        ble_tx_data.data[2 + frame_data.encrypt_len] = (frame_data.check_num >> 8) & 0xFF;
        ble_tx_data.data[3 + frame_data.encrypt_len] = frame_data.check_num & 0xFF;
        ble_tx_data.timeout = ble_tx_data.len;
        uart_tx_enqueue(UART_DATA_QUEUE_CHNL_1, &ble_tx_data);
        ble_frame_send_timer[BLE_AUTO_CHARGE_ACK].send_timer = 0;
    }
}

/*****************************************************************************************************
*Function   :ble_protocol_tx（蓝牙协议发送）
*Description:蓝牙协议发送函数，遍历每个枪号和每个报文类型，根据共享内存中的控制属性表判断是否需要发送报文，
             并调用相应的处理函数进行报文封装和发送
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void ble_protocol_tx(void)
{
    /*把完成的报文控制属性表从共享内存中读取出来*/
    control_info_t control_info;
    share_data_read(CONTROL_INFO, 0, &control_info, sizeof(control_info_t));

    /*以下是蓝牙报文发送遍历处理*/
    uint8_t  frame;

    for(frame = 0; frame < BLE_FRAME_MAX; frame++)
    {
        uint8_t send_flag = control_info.ble_uart_control[frame].send_flag;
        if(send_flag == 1)
        {
            switch(frame)
            {
            case BLE_AUTHOR:
                /* code */
                break;
            case BLE_AUTHOR_ACK1:
                    protocol_tx_handle_author_ack1();
                break;
            case BLE_AUTHOR_ACK2:
//                protocol_tx_handle_author_ack2();
                break;
            case BLE_AUTO_CHARGE:
                /* code */
                break;
            case BLE_AUTO_CHARGE_ACK:
//                    protocol_tx_handle_auto_charge_ack();
                break;
            
            default:
                break;
            } 
        }
        /*这里要特别注意下，如果发现报文发送标志位为置为0，则说明该报文已经发送完成，则将报文发送定时器清零*/
        else
        {
            memset(&ble_frame_send_timer[frame], 0, sizeof(ble_frame_send_timer_t));
        }
    }
}

/* ========================= 报文接收解析 =========================== */
/*****************************************************************************************************
*Function   :PKCS7数据去除填充函数
*Description:去除报文数据的PKCS7填充
*Input      :uint8_t* data  带有PKCS7填充的数据缓冲区指针
             uint8_t data_len 数据长度（字节）
*Output     :uint8_t* data  去除填充后的数据缓冲区
*Returns    :去除填充后的数据长度（字节）
*Note       :
*****************************************************************************************************/
uint8_t ble_pkcs7_unpad(uint8_t* data, uint8_t data_len)
{
    if(data_len <= 0)
    {
        return 0;
    }

    /*最后一个字节为填充长度*/
    uint8_t pad_len = data[data_len - 1];

    /*检查填充是否合法*/
    if (pad_len < 1 || pad_len > AES_BLOCK_SIZE)
    {
        return data_len;
    }

    /*检查所有填充字节是否一致*/
    for (uint8_t i = data_len - pad_len; i < data_len ; i++)
    {
        if (data[i] != pad_len)
        {            
            return data_len;
        }
    }
    /*返回真实数据长度*/
    return data_len - pad_len;
}

/*****************************************************************************************************
*Function   :aes_invsubbytes（AES逆字节替换）
*Description:执行AES逆字节替换操作
*Input      :uint8_t *state  状态缓冲区指针
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void aes_invsubbytes(uint8_t *state)
{
    for (int i = 0; i < 16; i++)
    {
        state[i] = rsbox[state[i]];
    }
}

/*****************************************************************************************************
*Function   :aes_invshiftrows（AES逆行移位）
*Description:执行AES逆行移位操作
*Input      :uint8_t *state  状态缓冲区指针
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void aes_invshiftrows(uint8_t *state)
{
    uint8_t t;
    t = state[1]; state[1] = state[13]; state[13] = state[9]; state[9] = state[5]; state[5] = t;
    t = state[2]; state[2] = state[10]; state[10] = t;
    t = state[6]; state[6] = state[14]; state[14] = t;
    t = state[3]; state[3] = state[7]; state[7] = state[11]; state[11] = state[15]; state[15] = t;
}

/*****************************************************************************************************
*Function   :aes_invmixcolumns（AES逆列混合）
*Description:执行AES逆列混合操作
*Input      :uint8_t *state  状态缓冲区指针
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void aes_invmixcolumns(uint8_t *state)
{
    for (int i = 0; i < 16; i += 4)
    {
        uint8_t a = state[i], b = state[i+1], c = state[i+2], d = state[i+3];
        state[i]   = aes_gmul(a,14) ^ aes_gmul(b,11) ^ aes_gmul(c,13) ^ aes_gmul(d,9);
        state[i+1] = aes_gmul(a,9) ^ aes_gmul(b,14) ^ aes_gmul(c,11) ^ aes_gmul(d,13);
        state[i+2] = aes_gmul(a,13) ^ aes_gmul(b,9) ^ aes_gmul(c,14) ^ aes_gmul(d,11);
        state[i+3] = aes_gmul(a,11) ^ aes_gmul(b,13) ^ aes_gmul(c,9) ^ aes_gmul(d,14);
    }
}

/*****************************************************************************************************
*Function   :aes128_ecb_decrypt（AES128-ECB 单块解密（16字节））
*Description:执行AES128-ECB单块解密操作
*Input      :const uint8_t *in   输入数据指针
             const uint8_t *key  密钥指针
*Output     :uint8_t *out   输出数据指针
*Returns    :
*Note       :
*****************************************************************************************************/
void aes128_ecb_decrypt(const uint8_t *in, uint8_t *out, const uint8_t *key)
{
    uint32_t w[44];     /*AES-128密钥扩展后生成的44个32位字（轮密钥）*/
    uint8_t state[AES_BLOCK_SIZE];   /*AES解密过程中的状态缓冲区*/

    aes_key_expand(key, w);
    memcpy(state, in, AES_BLOCK_SIZE);

    aes_addroundkey(state, w, 10);

    for (int round = 9; round >= 1; round--)
    {
        aes_invshiftrows(state);
        aes_invsubbytes(state);
        aes_addroundkey(state, w, round);
        aes_invmixcolumns(state);
    }

    aes_invshiftrows(state);
    aes_invsubbytes(state);
    aes_addroundkey(state, w, 0);

    memcpy(out, state, AES_BLOCK_SIZE);
}

/*****************************************************************************************************
*Function   :ble_unpack（解析接收蓝牙报文）
*Description:先校验帧头是否为协议帧头，如是则继续，否则返回错误；然后进行CRC校验，如校验成功则继续，否则返回CRC校验失败；
             然后对数据进行解密，解密后对数据进行去填充，如去填充后的数据有效长度与传输的目标长度一致，则继续，
             否则返回长度错误；最后将解密后的数据复制到业务数据结构体中
*Input      :const ble_frame_data_t *frame  接收到的蓝牙报文结构体指针
             const uint8_t *vin  车辆VIN码，长度为17字节
*Output     :ble_plain_t *data   解密后的业务数据结构体指针
*Returns    :0=成功，-1=帧头错误，-2=CRC错误，-3=长度错误
*Note       :
*****************************************************************************************************/
int ble_unpack(const ble_frame_data_t *frame, const uint8_t *vin, ble_plain_t *data)
{
    uint8_t key[AES_KEY_SIZE];
    uint8_t dec_buf[BT_MAX_ENC_LEN];
    int dec_len;

    if (frame == NULL || vin == NULL || data == NULL)
    {
        return -3;
    }

    // 1. 校验帧头
    if (frame->frame_head != BLE_FRAME_HEAD)
        return -1;

    if (frame->encrypt_len > BT_MAX_ENC_LEN || frame->effective_len > BT_MAX_PLAIN_LEN)
    {
        return -2;
    }

    if ((frame->encrypt_len % AES_BLOCK_SIZE) != 0)
    {
        return -2;
    }

    // 2. 校验CRC
    uint8_t crc_buf[1 + BT_MAX_ENC_LEN];
    crc_buf[0] = frame->effective_len;
    memcpy(crc_buf + 1, frame->encrypt_data, frame->encrypt_len);
    if (ble_crc16_ibm(crc_buf, 1 + frame->encrypt_len) != frame->check_num)
    {
        return -2;
    }

    // 3. 解密
    ble_gen_key(vin, key);
    dec_len = frame->encrypt_len;
    for (int i = 0; i < dec_len; i += AES_BLOCK_SIZE)
        aes128_ecb_decrypt(frame->encrypt_data + i, dec_buf + i, key);

    // 4. 去填充
    dec_len = ble_pkcs7_unpad(dec_buf, dec_len);
    if (dec_len != frame->effective_len)
    {
        return -3;
    }

    // 5. 复制业务数据
    memset(data, 0, sizeof(*data));
    data->plain_len = (uint8_t)dec_len;
    memcpy(data->data, dec_buf, dec_len);

    return 0;
}

/*****************************************************************************************************
*Function   :ble_protocol_rx（BLE协议接收处理）
*Description:该函数负责处理接收到的BLE报文数据，首先从BLE接收队列中获取数据，如果接收到数据则进行解析处理，
             如果报文头为BLE_FRAME_HEAD，则继续解析有效数据长度和报文内容，否则直接返回。
*Input      :
*Output     :
*Returns    :
*Note       :报文解析函数待完善，当前仅打印接收到的报文内容
*****************************************************************************************************/
void ble_protocol_rx(void)
{
    uint8_t len = 0;             /*报文长度*/
    uint8_t effective_len = 0;   /*有效数据长度*/
    if(ble_rx_dequeue(len, ble_rx_buf) == true)
    {
        /*接收到BLE数据，进行解析处理*/
        printf("recv bt: %.*s\n", len, ble_rx_buf);
        if(ble_rx_buf[0] == BLE_FRAME_HEAD)
        {
            effective_len = ble_rx_buf[1];
            /*解析处理报文*/
            // ble_unpack(&ble_rx_buf, VIN, &data);
        }
        else
        {
            return;
        }
    }
}

/*****************************************************************************************************
*Function   :ble_protocol_handle_task（BLE协议处理任务）
*Description:首先判断BLE模块是否初始化完成，如果未完成则调用初始化函数进行初始化；
             如果已完成则调用接收处理函数和发送处理函数。
*Input      :
*Output     :
*Returns    :
*Note       :须放在任务函数中，内部包含计数器，对任务周期要求为1ms
*****************************************************************************************************/
void ble_protocol_handle_task(void)
{
    if(ble_init_flag == false)
    {
        ble_init_flag = atk_ble03_init(&ble_init_data);
    }
    else
    {
        if(VIN[0] == 0)
        {
            uint32_t vin_offset = OFFSET_OF(mesure_info_t, ble_frame.bms_vin);
            share_data_read(MESURE_INFO, vin_offset, VIN, sizeof(VIN));
        }
        else
        {
            ble_protocol_rx();
            ble_protocol_tx();
        }

    }

}
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
