#include <stdint.h>
#include <stdio.h>
#include "node.h"

#define DELAYTIME 300
#define QUERYAB_ON_OFF_STATE HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
#define RCMDINUM 8                                 /* RCM上DI的数量*/
#define DEBOUNCETIME 3                             /*去抖次数3次*/

extern uint8_t lockflag;                           /* lock信号有无标志*/
extern uint8_t keyflag;                            /*有无按键按下标志 0:无按键按下，1：有按键按下*/
extern uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
extern uint8_t NodeId;                             /*保存节点的编号，保存在RAM中，掉电数据丢失，节点编号可变 */
extern CAN_RxHeaderTypeDef g_can1_rxheader;        /* 接收参数句柄 */

uint16_t debounceTimeSet[8];                       /* 保存去抖次数*/
volatile uint8_t rcmDiItFlag[8] = {0};
uint8_t rcmDiInitStatus[8] = {0};                  /* RCM8个DI的初始状态*/ 

extern TIM_HandleTypeDef g_tim2_queryA_cap_handle; /* 定时器x句柄 */
extern TIM_HandleTypeDef g_tim2_queryB_cap_handle; /* 定时器x句柄 */
extern TIM_HandleTypeDef g_tim2_queryA_cap_tm_handle;

uint8_t destAddr;                                  /*远程帧的目标地址*/
uint8_t slaveAddr;                                 /*响应远程帧的从机地址*/

uint8_t RcmOutputPort = 0x00;
uint8_t canbuf[8];

extern uint8_t g_tim2_queryA_cap_sta;       /* 输入捕获状态 */
extern uint16_t g_tim2_queryA_cap_val;      /* 输入捕获值 */

extern uint8_t g_tim2_queryA_cap_tm_sta;       /* 输入捕获状态 */
extern uint16_t g_tim2_queryA_cap_tm_val;      /* 输入捕获值 */

extern uint8_t g_tim2_queryB_cap_sta;       /* 输入捕获状态 */
extern uint16_t g_tim2_queryB_cap_val;      /* 输入捕获值 */
uint8_t queryAflag = 0;                     /* queryA捕获query信号有无标志*/
uint8_t queryBflag = 0;                     /* queryB捕获query信号有无标志*/
uint8_t tmQueryAflag = 0;                   /* TM节点捕获query信号有无标志*/
uint8_t queryAflag_5ms = 0;                 /* queryA捕获query信号有无标志*/
uint8_t queryBflag_5ms = 0;                 /* queryB捕获query信号有无标志*/
uint8_t queryATmflag = 0;                   /* queryA捕获TM有无标志*/
uint8_t queryBTmflag = 0;
uint32_t queryAtemp = 0;                    /* queryA捕获7ms的计数值*/
uint32_t queryBtemp = 0;                    /* queryB捕获7ms的计数值*/
uint32_t queryATmtemp = 0;                  /* queryA捕获35ms的计数值*/
uint32_t tmqueryAtemp = 0;                  /*TM捕获7ms的计数值*/

/*query检测处理函数*/
void queryCheckDisposeFunc(uint8_t normalPulse,uint8_t abnormalPulse)
{
	if(queryAflag == 0x01||queryAflag_5ms == 0x01)	
    {
		if(lockflag == 0x01)		/*能检测到Lock信号*/
		{
			delay_us(DELAYTIME);
			printf("捕获到query信号，闭合电子开关 QUERYAB_ON/OFF\r\n");
			MCU_CT_QUERYA(1);
			delay_ms(normalPulse);
			MCU_CT_QUERYA(0);
			queryAflag = 0x00;
			queryAflag_5ms = 0x00;
			lockflag = 0x0;
		}
		else
		{
			delay_us(DELAYTIME);
			MCU_CT_QUERYA(1);
			delay_ms(abnormalPulse);
			MCU_CT_QUERYA(0);
			queryAflag = 0x00;
			queryAflag_5ms = 0x00;
		}
		//can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 8);         /*上报数据 8字节*/
	}

	if(queryBflag == 0x01||queryBflag_5ms == 0x01)
    {
		if(lockflag == 0x01)		/*能检测到Lock信号*/
		{
			delay_us(DELAYTIME);
			printf("捕获到query信号，闭合电子开关 QUERYAB_ON/OFF\r\n");
			MCU_CT_QUERYB(1);
			delay_ms(normalPulse);
			MCU_CT_QUERYB(0);
			queryBflag = 0x00;
			queryBflag_5ms = 0x00;
			lockflag = 0x0;
		}
		else
		{
			delay_us(DELAYTIME);
			MCU_CT_QUERYB(1);
			delay_ms(abnormalPulse);
			MCU_CT_QUERYB(0);
			queryBflag = 0x00;
			queryBflag_5ms = 0x00;
		}
		//can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 8);     /*上报数据 8字节*/
	}
	
	delay_us(10);	
	QUERYAB_ON_OFF(1);                                             /* 电子开关闭合 */
	TIM2->SR &= ~(0x02);                                           /*清除中断标志*/
	TIM2->SR &= ~(0x04);                                           /*清除中断标志*/
	
	HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);                 /* 关闭TIM2中断 */
	HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
	
	HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* 开启TIM2中断 */ //xyy1-  //xyy2+
	HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);            //xyy1-  //xyy2+
	tmPulseCapDisposeFunc(35);
	/*捕获到35ms的TM脉冲*/
	if(queryATmflag == 0x01)
	{
		/*关闭A捕获35ms脉冲*/
		HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* 开启TIM2中断 */ //xyy1-  //xyy2+
		HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		delay_ms(1);
		QUERYAB_ON_OFF(0);                                         /* 电子开关断开 */
		/*打开A和B捕获query信号*/
		HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);
		HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		HAL_NVIC_EnableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);
		HAL_TIM_IC_Start_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		queryATmflag = 0x00;
	}	
   #if 0
    queryAflag = 0x00;
    queryBflag = 0x00;
	queryAflag_5ms = 0x00;
	queryBflag_5ms = 0x00;
	queryATmflag = 0x00;
	queryBTmflag = 0x00;
    lockflag = 0x00;
    keyflag = 0x00;
	#endif
    return;
}

/*TM节点捕获到7ms query信号，返回35ms的处理函数*/
void tmCheckDisposeFunc(uint8_t normalPulse)
{
	if(tmQueryAflag == 0x01||queryAflag_5ms == 0x01)	
    {
		delay_us(DELAYTIME);
		printf("捕获到query信号，闭合电子开关 QUERYAB_ON/OFF\r\n");
		MCU_CT_QUERYA(1);
		delay_ms(normalPulse);
		MCU_CT_QUERYA(0);
		tmQueryAflag = 0x00;
		queryAflag_5ms = 0x00;
	}
	TIM2->SR &= ~(0x02);                                           /*清除中断标志*/
	
	HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* 开启ITM2中断 */
	HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
}

/*query信号7ms和5ms的脉冲信号捕获处理函数*/
void queryCapDisposeFunc()
{
	if(g_tim2_queryA_cap_sta & 0x80)                                     /* 成功捕获到了一次高电平 */
	{
		queryAtemp = g_tim2_queryA_cap_sta & 0x3F;
		queryAtemp *= 0xFFFF;                                            /* 溢出时间总和 */
		queryAtemp += g_tim2_queryA_cap_val;                             /* 得到总的高电平时间 */
		/*捕获到7ms的query信号*/
		if((queryAtemp >=(7*1000-1000))&&(queryAtemp <= (7*1000+1000)))
		{
			queryAflag = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */ //xyy1-
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);            //xyy1-
		} /*捕获到5ms的query信号*/
		if((queryAtemp >= (5*1000-1000))&&(queryAtemp <= (5*1000+1000)))
		{
			queryAflag_5ms = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		}
		g_tim2_queryA_cap_sta = 0;                                       /* 开启下一次捕获 */
	}

	if(g_tim2_queryB_cap_sta & 0x80)                                    /* 成功捕获到了一次高电平 */
	{
		queryBtemp = g_tim2_queryB_cap_sta & 0x3F;
		queryBtemp *= 0xFFFF;                                            /* 溢出时间总和 */
		queryBtemp += g_tim2_queryB_cap_val;                             /* 得到总的高电平时间 */
		/*捕获到7ms的query信号*/
		if((queryBtemp >=(7*1000-1000))&&(queryBtemp <= (7*1000+1000)))
		{
			queryBflag = 0x01;
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}/*捕获到5ms的query信号*/
		if((queryBtemp >=(5*1000-1000))&&(queryBtemp <= (5*1000+1000)))               /*捕获到5ms的query信号*/
		{
			queryBflag_5ms = 0x01;
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭ITM2中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}
		g_tim2_queryB_cap_sta = 0;                                       /* 开启下一次捕获 */
	}
}

/*TM节点捕获query信号的处理函数，TM只有queryA捕获*/
void tmCapQueryDisposeFunc(uint8_t capturePulse)
{
	if(g_tim2_queryA_cap_sta & 0x80)                                     /* 成功捕获到了一次高电平 */
	{
		tmqueryAtemp = g_tim2_queryA_cap_sta & 0x3F;
		tmqueryAtemp *= 0xFFFF;                                            /* 溢出时间总和 */
		tmqueryAtemp += g_tim2_queryA_cap_val;                             /* 得到总的高电平时间 */
		/*捕获到7ms的query信号*/
		if((tmqueryAtemp >=(7*1000-1000))&&(tmqueryAtemp <= (7*1000+1000)))
		{
			tmQueryAflag = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			tmqueryAtemp = 0;
		}
		g_tim2_queryA_cap_sta = 0;                                       /* 开启下一次捕获 */
	}
}

/*queryA捕获35ms的TM返回的脉冲*/
void tmPulseCapDisposeFunc(uint8_t capturePulse)
{
	if(g_tim2_queryA_cap_tm_sta & 0x80)                                  /* 成功捕获到了一次高电平 */
	{
		queryATmtemp = g_tim2_queryA_cap_tm_sta & 0x3F;
		queryATmtemp *= 0xFFFF;                                          /* 溢出时间总和 */
		queryATmtemp += g_tim2_queryA_cap_tm_val;                        /* 得到总的高电平时间 */
		/*捕获到35ms的TM脉冲信号*/
		if((queryATmtemp >=(capturePulse*1000-1000))&&(queryATmtemp <= (capturePulse*1000+1000)))
		{
			queryATmflag = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			queryATmtemp = 0;
		}
		g_tim2_queryA_cap_tm_sta = 0;                                    /* 开启下一次捕获 */
	}
}

uint8_t elmNodeDisposeFunc(NodeDataStruct nodeData)
{
    uint8_t canReceiBuf[8];
    uint8_t *recvbuf = canReceiBuf;
    uint8_t AudioStatus;
	uint8_t Extcmd;
	uint8_t param1;

	delay_us(500);
	if(lockflag == 0x01)                                          /*能检测到Lock信号*/
	{
		nodeData.LockSignalStatus = 0x01;                         /*能检测到Lock信号*/
	}
	else
	{
		nodeData.LockSignalStatus = 0x00;                         /*能检测不到Lock信号*/
	}
	
	if(AUDIO_OUT == 1)
    {
        AudioStatus = 0x01;                                        /* 检测到音频信号*/
    }
    else
    {
        AudioStatus = 0x00;                                        /* 没有检测到音频信号*/
    }
	
    CAN_EN(0);                                                     /* can使能*/
	
    if(!can_receive_msg(g_can1_rxheader.ExtId, recvbuf))
    {
		Extcmd = (g_can1_rxheader.ExtId >> 24)&0xFF;
		param1 = (uint16_t)g_can1_rxheader.ExtId >>8;
		destAddr = (g_can1_rxheader.ExtId>>16)&0x7F;
        if(Extcmd == 0x01)                                         /*子节点接收到"设置节点编号"的命令*/
        {
            NodeId = param1;                                       /*节点编号提取出来赋给NodeId 保存起来*/
			slaveAddr = NodeId;                                    /*从机地址等于节点编号*/
			if(slaveAddr>=1&&slaveAddr<=61)
			{
				can_send_msg_data(0x11800100|(NodeId<<16), canbuf, 0); /*回送节点编号，发送节点类型 0x01*/ 
			}
        }
		else
		{
			;
		}
    }
    //CAN_EN(1);                                                   /*CAN禁止 */
	canbuf[0] = nodeData.NodeType;                                 /*子节点类型*/
	canbuf[1] = nodeData.PowerVolt;                                /*18V工作电压采集值*/
	canbuf[2] = nodeData.EmerStopKeyStatus;                        /*急停按钮状态*/
	canbuf[3] = nodeData.LockSignalStatus;                         /*lock闭锁信号状态*/
	canbuf[4] = AudioStatus;                                       /*音频信号状态*/
    return 0;
}

uint8_t elmVpmNodeDisposeFunc(NodeDataStruct nodeData)
{
    uint8_t canReceiBuf[8];
    uint8_t *recvbuf = canReceiBuf;
	uint8_t Extcmd;
    uint8_t param1;

	delay_us(500);
	
	if(keyflag == 0x01)
    {
        nodeData.AudioChargeKeyStatus.Key1Key2Status = 0x01;          /*有按键按下*/
    }
    else
    {
         nodeData.AudioChargeKeyStatus.Key1Key2Status = 0x00;         /*没有按键按下*/
    }
	
    if(AUDIO_OUT == 1)
    {
        nodeData.AudioChargeKeyStatus.AudioStatus = 0x01;             /* 检测到音频信号*/
    }
    else
    {
        nodeData.AudioChargeKeyStatus.AudioStatus = 0x00;             /* 没有检测到音频信号*/
    }
	
	if(lockflag == 0x01)                                              /*能检测到Lock信号*/
	{
		nodeData.LockSignalStatus = 0x01;                             /*能检测到Lock信号*/
	}
	else
	{
		nodeData.LockSignalStatus = 0x00;                             /*能检测不到Lock信号*/
	}
	
    CAN_EN(0); /* can使能*/
    if(!can_receive_msg(g_can1_rxheader.ExtId, recvbuf))
    { 
		Extcmd = (g_can1_rxheader.ExtId >> 24)&0xFF;
        param1 = (uint16_t)g_can1_rxheader.ExtId >>8;
		destAddr = (g_can1_rxheader.ExtId>>16)&0x7F;
        if(Extcmd == 0x01)                                            /*子节点接收到"设置节点编号"的命令*/
        {
            NodeId = param1;                                          /*节点编号提取出来赋给NodeId */
			slaveAddr = NodeId;                                       /*从机地址等于节点编号*/
			if(slaveAddr>=1 && slaveAddr<=61)
			{
				can_send_msg_data(0x11800200|(NodeId<<16), canbuf, 0);    /*回送节点编号，发送节点类型 0x02*/ 
			}
        }
        else if((Extcmd == 0x17)&&(destAddr == slaveAddr))            /*子节点接收到"充电控制"的命令*/
        {
            if(param1 == 0x01)  
            {
                printf("允许充电\r\n");
                BAT_CHARGE_EN(1);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 1;        /* 已充电*/
                can_send_msg_data(0x17800100|(NodeId<<16), canbuf, 0); /* 返回充电状态*/
            }
            else
            {
                printf("禁止充电\r\n");
                BAT_CHARGE_EN(0);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 0;        /* 未充电*/
                can_send_msg_data(0x17800000|(NodeId<<16), canbuf, 0); /* 返回充电状态*/
            }
        }
		else
		{
			;
		}
    }
    //CAN_EN(1);                                                   /*CAN禁止 */
	canbuf[0] = nodeData.NodeType;                                 /*子节点类型*/
	canbuf[1] = nodeData.PowerVolt;                                /*18V工作电压采集值*/
	canbuf[2] = nodeData.BatVolt;                                  /*电池电压采集值*/
	canbuf[3] = nodeData.EmerStopKeyStatus;                        /*急停按钮状态*/
	canbuf[4] = nodeData.LockSignalStatus;                         /*lock闭锁信号状态*/
	canbuf[5] = nodeData.AudioChargeKeyStatus.AudioStatus|(nodeData.AudioChargeKeyStatus.ChargeStatus<<1)|(nodeData.AudioChargeKeyStatus.Key1Key2Status<<2);
    return 0;
}

void AllRcmNodeDoPortDisconnect(NodeDataStruct nodeData)
{
    uint8_t i;
	
    for(i=0;i<62;i++)
    {
        if(nodeData.NodeId == (i+1))
        {
            RCM_DO1(1);
            RCM_DO2(1);
            RCM_DO3(1);
            RCM_DO4(1);
        }
    }
}

/*收到去抖时间指令后，处理函数*/
uint8_t rcmDiDebounceDisposeFunc(uint16_t debounceTimeSet[])
{
	uint8_t tempValue[3] = {0};                                               /*去抖三次,采集的值*/
	uint8_t j;
	uint8_t rcmDiStatusFlag = 0;                                              /*RCM节点的DI状态变化标志，0无变化，1有变化*/
	uint8_t rcmDiValue[8] = {0};   
	
	rcmDiValue[0] = RCMDI1;
	rcmDiValue[1] = RCMDI2;
	rcmDiValue[2] = RCMDI3;
	rcmDiValue[3] = RCMDI4;
	rcmDiValue[4] = RCMDI5;
	rcmDiValue[5] = RCMDI6;
	rcmDiValue[6] = RCMDI7;
	rcmDiValue[7] = RCMDI8;
	
	if(rcmDiItFlag[0] == 1)                                                   /*标志位为1表示DI有变化*/
	{
		if(rcmDiValue[0] != rcmDiInitStatus[0])                               /*DI不等于初始值，有变化*/
		{
			delay_ms(debounceTimeSet[0]);                                     /* 去抖*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*采集三次*/
			{
				tempValue[j] = rcmDiValue[0];                                 /*保存采集三次的值*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*判断三次采集的值相同，DI1没有变化*/
			{
				printf("DI的值有变化,上报数据\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[0] = rcmDiValue[0];
			}
			else                                                              /*判断三次采集的值不相同，DI有变化，上报*/
			{
				rcmDiInitStatus[0] = tempValue[2];
				delay_ms(debounceTimeSet[0]);
			}
		}
	}
	if(rcmDiItFlag[1] == 1)                                                   /*标志位为1表示DI有变化*/
	{
		if(rcmDiValue[1] != rcmDiInitStatus[1])                               /*DI不等于初始值，有变化*/
		{
			delay_ms(debounceTimeSet[1]);                                     /* 去抖*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*采集三次*/
			{
				tempValue[j] = rcmDiValue[1];                                 /*保存采集三次的值*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*判断三次采集的值相同，DI1没有变化*/
			{
				printf("DI的值有变化,上报数据\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[1] = rcmDiValue[1];
			}
			else                                                              /*判断三次采集的值不相同，DI有变化，上报*/
			{
				delay_ms(debounceTimeSet[1]);
			}
		}
	}
	if(rcmDiItFlag[2] == 1)                                                   /*标志位为1表示DI有变化*/
	{
		if(rcmDiValue[2] != rcmDiInitStatus[2])                               /*DI不等于初始值，有变化*/
		{
			delay_ms(debounceTimeSet[2]);                                     /* 去抖*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*采集三次*/
			{
				tempValue[j] = rcmDiValue[2];                                 /*保存采集三次的值*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*判断三次采集的值相同，DI1没有变化*/
			{
				printf("DI的值有变化,上报数据\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[2] = rcmDiValue[2];
			}
			else                                                              /*判断三次采集的值不相同，DI有变化，上报*/
			{
				delay_ms(debounceTimeSet[2]);
			}
		}
	}
	if(rcmDiItFlag[3] == 1)                                                   /*标志位为1表示DI有变化*/
	{
		if(rcmDiValue[3] != rcmDiInitStatus[3])                               /*DI不等于初始值，有变化*/
		{
			delay_ms(debounceTimeSet[3]);                                     /* 去抖*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*采集三次*/
			{
				tempValue[j] = rcmDiValue[3];                                 /*保存采集三次的值*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*判断三次采集的值相同，DI1没有变化*/
			{
				printf("DI的值有变化,上报数据\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[3] = rcmDiValue[3];
			}
			else                                                              /*判断三次采集的值不相同，DI有变化，上报*/
			{
				delay_ms(debounceTimeSet[3]);
			}
		}
	}
	if(rcmDiItFlag[4] == 1)                                                   /*标志位为1表示DI有变化*/
	{
		if(rcmDiValue[4] != rcmDiInitStatus[4])                               /*DI不等于初始值，有变化*/
		{
			delay_ms(debounceTimeSet[4]);                                     /* 去抖*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*采集三次*/
			{
				tempValue[j] = rcmDiValue[4];                                 /*保存采集三次的值*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*判断三次采集的值相同，DI1没有变化*/
			{
				printf("DI的值有变化,上报数据\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[4] = rcmDiValue[4];
			}
			else                                                              /*判断三次采集的值不相同，DI有变化，上报*/
			{
				delay_ms(debounceTimeSet[4]);
			}
		}
	}
	if(rcmDiItFlag[5] == 1)                                                   /*标志位为1表示DI有变化*/
	{
		if(rcmDiValue[5] != rcmDiInitStatus[5])                               /*DI不等于初始值，有变化*/
		{
			delay_ms(debounceTimeSet[5]);                                     /* 去抖*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*采集三次*/
			{
				tempValue[j] = rcmDiValue[5];                                 /*保存采集三次的值*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*判断三次采集的值相同，DI1没有变化*/
			{
				printf("DI的值有变化,上报数据\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[5] = rcmDiValue[5];
			}
			else                                                              /*判断三次采集的值不相同，DI有变化，上报*/
			{
				delay_ms(debounceTimeSet[5]);
			}
		}
	}
	if(rcmDiItFlag[6] == 1)                                                   /*标志位为1表示DI有变化*/
	{
		if(rcmDiValue[6] != rcmDiInitStatus[6])                               /*DI不等于初始值，有变化*/
		{
			delay_ms(debounceTimeSet[6]);                                     /* 去抖*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*采集三次*/
			{
				tempValue[j] = rcmDiValue[6];                                 /*保存采集三次的值*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*判断三次采集的值相同，DI1没有变化*/
			{
				printf("DI的值有变化,上报数据\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[1] = rcmDiValue[6];
			}
			else                                                              /*判断三次采集的值不相同，DI有变化，上报*/
			{
				delay_ms(debounceTimeSet[6]);
			}
		}
	}
	if(rcmDiItFlag[7] == 1)                                                   /*标志位为1表示DI有变化*/
	{
		if(rcmDiValue[7] != rcmDiInitStatus[7])                               /*DI不等于初始值，有变化*/
		{
			delay_ms(debounceTimeSet[7]);                                     /* 去抖*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*采集三次*/
			{
				tempValue[j] = rcmDiValue[7];                                 /*保存采集三次的值*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*判断三次采集的值相同，DI1没有变化*/
			{
				printf("DI的值有变化,上报数据\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[7] = rcmDiValue[7];
			}
			else                                                              /*判断三次采集的值不相同，DI有变化，上报*/
			{
				delay_ms(debounceTimeSet[7]);
			}
		}
	}
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	return rcmDiStatusFlag;
}
	
uint8_t rcmVpmNodeDisposeFunc(NodeDataStruct nodeData)
{
    uint8_t canReceiBuf[8];
    uint8_t *recvbuf = canReceiBuf;
    uint8_t Extcmd;
    uint8_t param1;
    uint8_t param2;
	uint8_t rcmDiId;
	uint8_t rcmDiStatusFlag = 0;                                    /*RCM节点的DI状态变化标志，0无变化，1有变化*/
	uint8_t rs485ComAddr;                                           /*RS485的通信地址*/
    uint32_t rs485Baud;                                             /*RS485的通信波特率*/
	
	delay_us(500);
	if(nodeData.EmerStopKeyStatus == 0x01)                          /*急停按钮按下，断开所有DO*/ 
	{
		RCM_DO1(0);                                                 /*低电平断开*/
		RCM_DO2(0);
		RCM_DO3(0);
		RCM_DO4(0);
	}
	if(keyflag == 0x01)
    {
        nodeData.AudioChargeKeyStatus.Key1Key2Status = 1;          /*有按键按下*/
    }
    else
    {
         nodeData.AudioChargeKeyStatus.Key1Key2Status = 0;         /*没有按键按下*/
    }
    
    if(AUDIO_OUT == 1)
    {
        nodeData.AudioChargeKeyStatus.AudioStatus = 1;             /* 检测到音频信号*/
    }
    else
    {
        nodeData.AudioChargeKeyStatus.AudioStatus = 0;             /* 没有检测到音频信号*/
    }
	
	if(lockflag == 0x01)                                           /*能检测到Lock信号*/
	{
		nodeData.LockSignalStatus = 0x01;                          /*能检测到Lock信号*/
	}
	else
	{
		nodeData.LockSignalStatus = 0x00;                          /*能检测不到Lock信号*/
	}

	canbuf[0] = nodeData.NodeType;                                 /*子节点类型*/
	canbuf[1] = nodeData.PowerVolt;                                /*18V工作电压采集值*/
	canbuf[2] = nodeData.BatVolt;                                  /*电池电压采集值*/
	canbuf[3] = nodeData.EmerStopKeyStatus;                        /*急停按钮状态*/
	canbuf[4] = nodeData.LockSignalStatus;                         /*lock闭锁信号状态*/
	
    CAN_EN(0);                                                      /* can使能*/
    if(!can_receive_msg(g_can1_rxheader.ExtId, recvbuf))
    {
        Extcmd = (g_can1_rxheader.ExtId >> 24)&0xFF;
        param1 = (uint16_t)g_can1_rxheader.ExtId >>8;
        param2 = (uint8_t)g_can1_rxheader.ExtId;
		destAddr = (g_can1_rxheader.ExtId>>16)&0x7F;
        if(Extcmd == 0x01)                                          /*子节点接收到"设置节点编号"的命令*/
        {
            NodeId = param1;                                        /*节点编号提取出来赋给NodeId */
			slaveAddr = NodeId;                                     /*从机地址等于节点编号*/
			if(slaveAddr>=1 && slaveAddr<=61)
			{
				can_send_msg_data(0x11800300|(NodeId<<16), canbuf, 0);  /*发送节点类型 0x03*/
			}
        }
		else if((Extcmd == 0x03)&&(destAddr == slaveAddr))          /*子节点接收到"输出端口控制"的命令*/
		{
			if(param1 == 0x01)                                      /*参数1代表DO编号*/
			{
				if(param2 == 0x00)                                  /*参数2代表DO的电平，0低电平，1高电平*/
				{
					RCM_DO1(0);
					RcmOutputPort &= (~(1<<0));
				}
				else
				{
					RCM_DO1(1);
					RcmOutputPort |= (1<<0);
				}
			}
			else if(param1 == 0x02)
			{
				if(param2 == 0x00)
				{
					RCM_DO2(0);
					RcmOutputPort &= (~(1<<1));
				}
				else
				{
					RCM_DO2(1);
					RcmOutputPort |= (1<<1);
				}
			}
			else if(param1 == 0x03)
			{
				if(param2 == 0x00)
				{
					RCM_DO3(0);
					RcmOutputPort &= (~(1<<2));
				}
				else
				{
					RCM_DO3(1);
					RcmOutputPort |= (1<<2);
				}
			}
			else if(param1 == 0x04)
			{
				if(param2 == 0x00)
				{
					RCM_DO4(0);
					RcmOutputPort &= (~(1<<3));
				}
				else
				{
					RCM_DO4(1);
					RcmOutputPort |= (1<<3);
				}
			}
			canbuf[7] = RcmOutputPort;
			can_send_msg_data(g_can1_rxheader.ExtId, canbuf, 0);
		}
		else if((Extcmd == 0x17)&&(destAddr == slaveAddr))               /*子节点接收到"充电控制"的命令*/
        {
            if(param1 == 0x01)  
            {
                printf("允许充电\r\n");
                BAT_CHARGE_EN(1);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 1;          /* 已充电*/
                can_send_msg_data(0x17800100|(NodeId<<16), canbuf, 0);   /* 返回充电状态*/
            }
            else
            {
                printf("禁止充电\r\n");
                BAT_CHARGE_EN(0);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 0;           /* 未充电*/
                can_send_msg_data(0x17800000|(NodeId<<16), canbuf, 0);    /* 返回充电状态*/
            }
			canbuf[5] = nodeData.AudioChargeKeyStatus.AudioStatus|(nodeData.AudioChargeKeyStatus.ChargeStatus<<1)|(nodeData.AudioChargeKeyStatus.Key1Key2Status<<2);
        }
		else if((Extcmd == 0x18)&&(destAddr == slaveAddr))                /*子节点接收到"RS485通信地址"的命令*/
		{
			/*参数1是通信地址，参数2是通信波特率*/
			if(param2 == 0)
			{
				rs485ComAddr = 0;                                         /*清除当前通信地址*/
			}
			else if(param2 == 1)
			{
				rs485Baud = 1200;
			}
			else if(param2 == 2)
			{
				rs485Baud = 2400;
			}
			else if(param2 == 3)
			{
				rs485Baud = 4800;
			}
			else if(param2 == 4)
			{
				rs485Baud = 9600;
			}
			else if(param2 == 5)
			{
				rs485Baud = 19200;
			}
			else if(param2 == 6)
			{
				rs485Baud = 38400;
			}
			else if(param2 == 7)
			{
				rs485Baud = 57600;
			}
			else if(param2 == 8)
			{
				rs485Baud = 115200;                               /*默认波特率*/
			}
			else if(param2 == 9)
			{
				rs485Baud = 460800;
			}
			can_send_msg_data(g_can1_rxheader.ExtId, canbuf, 0);
		}
		else if((Extcmd == 0x1E)&&(destAddr == slaveAddr))        /*子节点接收到"去抖时间设置"的命令*/
		{
			rcmDiId = param2>>4;                                  /* 保存“去抖的DI号”*/
			if(rcmDiId == 1)
			{
				debounceTimeSet[0] = param1|((param2&0x0F)<<8);   /* 保存“去抖时间”*/
			}
			else if(rcmDiId == 2)
			{
				debounceTimeSet[1] = param1|((param2&0x0F)<<8);   /* 保存“去抖时间”*/
			}
			else if(rcmDiId == 3)
			{
				debounceTimeSet[2] = param1|((param2&0x0F)<<8);   /* 保存“去抖时间”*/
			}
			else if(rcmDiId == 4)
			{
				debounceTimeSet[3] = param1|((param2&0x0F)<<8);   /* 保存“去抖时间”*/
			}
			else if(rcmDiId == 5)
			{
				debounceTimeSet[4] = param1|((param2&0x0F)<<8);   /* 保存“去抖时间”*/
			}
			else if(rcmDiId == 6)
			{
				debounceTimeSet[5] = param1|((param2&0x0F)<<8);   /* 保存“去抖时间”*/
			}
			else if(rcmDiId == 7)
			{
				debounceTimeSet[6] = param1|((param2&0x0F)<<8);   /* 保存“去抖时间”*/
			}
			else if(rcmDiId == 8)
			{
				debounceTimeSet[7] = param1|((param2&0x0F)<<8);   /* 保存“去抖时间”*/
			}
			can_send_msg_data(g_can1_rxheader.ExtId, canbuf, 0);
		}
		else
		{
			;
		}
	}
	rcmDiStatusFlag = rcmDiDebounceDisposeFunc(debounceTimeSet);
	if(rcmDiStatusFlag == 1)
	{
		nodeData.RcmInputPort = RCMDI1|RCMDI2<<1|RCMDI3<<2|RCMDI4<<3|RCMDI5<<4|RCMDI6<<5|RCMDI7<<6|RCMDI8<<7;
		canbuf[6] = nodeData.RcmInputPort;
		can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 8);         /*当DI有变化，上报数据 8字节*/
		rcmDiStatusFlag = 0;
		rcmDiItFlag[0] = 0;
		rcmDiItFlag[1] = 0;
		rcmDiItFlag[2] = 0;
		rcmDiItFlag[3] = 0;
		rcmDiItFlag[4] = 0;
		rcmDiItFlag[5] = 0;
		rcmDiItFlag[6] = 0;
		rcmDiItFlag[7] = 0;
	}
	//CAN_EN(1);                                                       /* can禁止*/ 
}

uint8_t tmNodeDisposeFunc(NodeDataStruct nodeData)
{
    uint8_t canReceiBuf[8];
    uint8_t *recvbuf = canReceiBuf;
	uint8_t Extcmd;
    uint8_t param1;

	delay_us(500);
	
    CAN_EN(0); /* can使能*/
	canbuf[0] = nodeData.NodeType;                                 /*子节点类型*/
	canbuf[1] = nodeData.PowerVolt;  
    if(!can_receive_msg(g_can1_rxheader.ExtId,recvbuf))  
    {
		Extcmd = (g_can1_rxheader.ExtId >> 24)&0xFF;
        param1 = (uint16_t)g_can1_rxheader.ExtId >>8;
		destAddr = (g_can1_rxheader.ExtId>>16)&0x7F;
        if(Extcmd == 0x01)                                        /*子节点接收到"设置节点编号"的命令*/
        {
            NodeId = param1;                                      /*节点编号提取出来赋给NodeId */
			slaveAddr = NodeId;                                   /*从机地址等于节点编号*/
			if(slaveAddr == 0x3E)
			{
				can_send_msg_data(0x11800400|(NodeId<<16), canbuf, 0);/*发送节点类型 0x04*/
			}				
        }
		else
		{
			;
		}
    }                                                
    //CAN_EN(1);                                                   /*CAN禁止 */
#if 0
    if(queryAflag == 0x01)
    {
		delay_ms(1);
		printf("捕获到query信号，闭合电子开关 QUERYAB_ON/OFF\r\n");
		MCU_CT_QUERYA(1);
		delay_ms(35);
		MCU_CT_QUERYA(0);
		queryAflag = 0x00;
		queryAflag_5ms = 0x00;
		//can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 2);     /*上报数据 2字节*/
    }
	TIM2->SR &= ~(0x02);                                           /*清除中断标志*/
	HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* 开启ITM2中断 */
	HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
	#endif
    return 0;
}

