#ifndef __NODE_H
#define __NODE_H

#include<stdint.h>
#include<stdio.h>

#include "./SYSTEM/sys/sys.h"
//#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/CAN/can.h"
#include "./BSP/QUERY/query.h"
//#include "./BSP/RS485/rs485.h"
#include "./BSP/key/key.h"
#include "./BSP/QUERYTIMER/queryabcap.h"
#include "./BSP/GPIO/gpio.h"
#include "./BSP/RCMDI/rcmdi.h"
#include "./BSP/LOCKPWM/lockpwm.h"
#include "./BSP/ATIMER/atim.h"

#define ADC_DMA_BUF_SIZE        50 * 2      /* ADC DMA采集 BUF大小, 应等于ADC通道数的整数倍 */

//节点类型枚举
typedef enum
{
    ELM         = 0x01, //急停节点
    ELM_VPM     = 0x02, //急停+语音节点
    RCM_VPM     = 0x03, //急停+IO+语音节点
    TM          = 0x04  //终端节点
} NodeTypeDef;

//节点状态
typedef enum
{
    NODENOEXIST = 0x00, //节点不存在
    NODEEXIST = 0x01,   //节点存在
} NodeStatusDef;

//端口状态
typedef enum
{
    LOWLEVEL = 0x00,  //低电平
    HIGHLEVEL = 0x01, //高电平
}RcmPortStatusDef;

//LOCK信号状态
typedef enum
{
    LOCKNOEXIST = 0x00, //lock不存在
    LOCKEXIST = 0x01,   //lock存在
} LockStatusDef;

//query信号状态
typedef enum
{
    QUERYNOEXIST = 0x00, //query信号不存在
    QUERYEXIST = 0x01,   //query信号存在
} QueryStatusDef;

//key1 key2状态
typedef enum
{
    KEYPUSH = 0x00, //K按键按下
    KEY = 0x01,     //K按键松开
} KeyStatusDef;

//音频状态
typedef enum
{
    AUDIONOEXIST = 0x00, //音频信号不存在
    AUDIOEXIST = 0x01,   //音频信号存在
} AudioStatusDef;

typedef struct
{
    uint8_t  AudioStatus : 1; //音频线状态，=0未检测到音频线，=1检测到音频线
    uint8_t ChargeStatus : 1; //充电状态，=0未充电，=1已充电
    uint8_t Key1Key2Status : 1; //K1，K2的状态，=0均未按下，=1其中一个按下
    uint8_t other : 5;
}StatusBit;

typedef struct {
    uint8_t NodeType;                //节点类型
    uint8_t NodeId;                  //节点编号
    uint8_t PowerVolt;               //18V电源电压
    uint8_t BatVolt;                 //电池电压
	uint8_t EmerStopKeyStatus;       //急停按钮状态
    uint8_t LockSignalStatus;        //lock信号的状态
    StatusBit AudioChargeKeyStatus;
	uint8_t RcmInputPort;            //RCM节点的8个输入端口状态：D0-7=1~8通道，1：高电平，0：低电平
    uint8_t RcmOutputPort;           //RCM节点的4个输出端口状态：D0-D3=1~4通道,1：高电平，0：低电平
}NodeDataStruct;

uint8_t *adc_nch_get_data(void);                //多通道采集
uint8_t elmNodeDisposeFunc(NodeDataStruct nodeData);
uint8_t elmVpmNodeDisposeFunc(NodeDataStruct nodeData);
uint8_t rcmVpmNodeDisposeFunc(NodeDataStruct nodeData);
void rcmRs485DisposeFunc(uint8_t rs485buf[],uint8_t rs485RecLen);
uint8_t tmNodeDisposeFunc(NodeDataStruct nodeData);
void AllRcmNodeDoPortDisconnect(NodeDataStruct nodeData);
uint8_t rcmDiDebounceDisposeFunc(uint16_t debounceTimeSet[]);
void queryCheckDisposeFunc(uint8_t Normalpulse,uint8_t Abnormalpulse);
void queryCapDisposeFunc();
void tmPulseCapDisposeFunc(uint8_t capturePulse);
void tmCheckDisposeFunc(uint8_t normalPulse);
void tmCapQueryDisposeFunc();
#endif

