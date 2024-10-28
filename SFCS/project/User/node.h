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

#define ADC_DMA_BUF_SIZE        50 * 2      /* ADC DMA�ɼ� BUF��С, Ӧ����ADCͨ������������ */

//�ڵ�����ö��
typedef enum
{
    ELM         = 0x01, //��ͣ�ڵ�
    ELM_VPM     = 0x02, //��ͣ+�����ڵ�
    RCM_VPM     = 0x03, //��ͣ+IO+�����ڵ�
    TM          = 0x04  //�ն˽ڵ�
} NodeTypeDef;

//�ڵ�״̬
typedef enum
{
    NODENOEXIST = 0x00, //�ڵ㲻����
    NODEEXIST = 0x01,   //�ڵ����
} NodeStatusDef;

//�˿�״̬
typedef enum
{
    LOWLEVEL = 0x00,  //�͵�ƽ
    HIGHLEVEL = 0x01, //�ߵ�ƽ
}RcmPortStatusDef;

//LOCK�ź�״̬
typedef enum
{
    LOCKNOEXIST = 0x00, //lock������
    LOCKEXIST = 0x01,   //lock����
} LockStatusDef;

//query�ź�״̬
typedef enum
{
    QUERYNOEXIST = 0x00, //query�źŲ�����
    QUERYEXIST = 0x01,   //query�źŴ���
} QueryStatusDef;

//key1 key2״̬
typedef enum
{
    KEYPUSH = 0x00, //K��������
    KEY = 0x01,     //K�����ɿ�
} KeyStatusDef;

//��Ƶ״̬
typedef enum
{
    AUDIONOEXIST = 0x00, //��Ƶ�źŲ�����
    AUDIOEXIST = 0x01,   //��Ƶ�źŴ���
} AudioStatusDef;

typedef struct
{
    uint8_t  AudioStatus : 1; //��Ƶ��״̬��=0δ��⵽��Ƶ�ߣ�=1��⵽��Ƶ��
    uint8_t ChargeStatus : 1; //���״̬��=0δ��磬=1�ѳ��
    uint8_t Key1Key2Status : 1; //K1��K2��״̬��=0��δ���£�=1����һ������
    uint8_t other : 5;
}StatusBit;

typedef struct {
    uint8_t NodeType;                //�ڵ�����
    uint8_t NodeId;                  //�ڵ���
    uint8_t PowerVolt;               //18V��Դ��ѹ
    uint8_t BatVolt;                 //��ص�ѹ
	uint8_t EmerStopKeyStatus;       //��ͣ��ť״̬
    uint8_t LockSignalStatus;        //lock�źŵ�״̬
    StatusBit AudioChargeKeyStatus;
	uint8_t RcmInputPort;            //RCM�ڵ��8������˿�״̬��D0-7=1~8ͨ����1���ߵ�ƽ��0���͵�ƽ
    uint8_t RcmOutputPort;           //RCM�ڵ��4������˿�״̬��D0-D3=1~4ͨ��,1���ߵ�ƽ��0���͵�ƽ
}NodeDataStruct;

uint8_t *adc_nch_get_data(void);                //��ͨ���ɼ�
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

