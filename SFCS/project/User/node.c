#include <stdint.h>
#include <stdio.h>
#include "node.h"

#define RCMDINUM 8                                 /* RCM上DI的数量*/
#define DEBOUNCETIME 3                             /*去抖次数3次*/

extern uint8_t lockflag;                           /* lock信号有无标志*/
extern uint8_t keyflag;                            /*有无按键按下标志 0:无按键按下，1：有按键按下*/
extern uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
extern uint8_t NodeId;                             /*保存节点的编号，保存在RAM中，掉电数据丢失，节点编号可变 */
extern CAN_RxHeaderTypeDef g_can1_rxheader;        /* 接收参数句柄 */

uint16_t debounceTimeSet[8] = {5,5,5,5,5,5,5,5};                       /* 保存去抖次数*/
volatile uint8_t rcmDiItFlag[8] = {0};
uint8_t rcmDiInitStatus[8] = {0};                  /* RCM8个DI的初始状态*/ 

extern TIM_HandleTypeDef g_tim2_queryA_cap_handle; /* 定时器x句柄 */
extern TIM_HandleTypeDef g_tim2_queryB_cap_handle; /* 定时器x句柄 */

uint8_t destAddr;                                  /*远程帧的目标地址*/
uint8_t slaveAddr;                                 /*响应远程帧的从机地址*/

uint8_t RcmOutputPort = 0x00;
uint8_t canbuf[8];

uint8_t queryAflag_5ms = 0;                 /* queryA捕获5ms query信号有无标志*/
uint8_t queryBflag_5ms = 0;                 /* queryB捕获5ms query信号有无标志*/
uint8_t queryAflag = 0;                     /* queryA捕获7ms query信号有无标志*/
uint8_t queryBflag = 0;                     /* queryB捕获7ms query信号有无标志*/
uint8_t queryAflag_9ms = 0;                 /* queryA捕获9ms query信号有无标志*/
uint8_t queryBflag_9ms = 0;                 /* queryB捕获9ms query信号有无标志*/
uint8_t queryATmflag = 0;                   /* queryA捕获35ms TM返回脉冲信号有无标志*/
uint8_t tmqueryAflag_5ms = 0;               /*TM节点捕获到5ms query信号的标志*/
uint8_t tmqueryAflag = 0;                   /*TM节点捕获到7ms query信号的标志*/
uint8_t tmqueryAflag_9ms = 0;               /*TM节点捕获到9ms query信号的标志*/

extern uint8_t g_tim2_queryA_cap_sta;       /* 输入捕获状态 */
extern uint16_t g_tim2_queryA_cap_val;      /* 输入捕获值 */

extern uint8_t g_tim2_queryB_cap_sta;       /* 输入捕获状态 */
extern uint16_t g_tim2_queryB_cap_val;      /* 输入捕获值 */
uint32_t queryAtemp = 0U;                    /* queryA捕获7ms的计数值*/
uint32_t queryBtemp = 0U;                    /* queryB捕获7ms的计数值*/
uint32_t queryATmtemp = 0U;                  /* TM节点捕获5ms 7ms 9ms queryA的计数值*/
extern uint8_t query_on_switch;
/*7ms query信号捕获次数，第一次捕获到，上报节点状态*/
uint8_t elmReportCnta = 0x00;                /*ELM节点queryA捕获到7ms query信号的次数*/
uint8_t elmReportCntb = 0x00;                /*ELM节点queryB捕获到7ms query信号的次数*/
uint8_t elmVpmReportCnta = 0x00;             /*ELM+VPM节点queryA捕获到7ms query信号的次数*/
uint8_t elmVpmReportCntb = 0x00;             /*ELM+VPM节点queryB捕获到7ms query信号的次数*/
uint8_t rcmVpmReportCnta = 0x00;             /*RCM+VPM节点queryA捕获到7ms query信号的次数*/
uint8_t rcmVpmReportCntb = 0x00;             /*RCM+VPM节点queryB捕获到7ms query信号的次数*/
/*9ms query信号捕获次数，第一次捕获到，响应节点编号指令*/
uint8_t elmCapCnta = 0x00;                    /*ELM节点queryA捕获9ms query脉冲的次数*/
uint8_t elmCapCntb = 0x00;                    /*ELM节点queryB捕获9ms query脉冲的次数*/
uint8_t elmVpmCapCnta = 0x00;                 /*ELM+VPM节点queryA捕获9ms query脉冲的次数*/
uint8_t elmVpmCapCntb = 0x00;                 /*ELM+VPM节点queryB捕获9ms query脉冲的次数*/
uint8_t rcmVpmCapCnta = 0x00;                 /*RCM+VPM节点queryA捕获9ms query脉冲的次数*/
uint8_t rcmVpmCapCntb = 0x00;                 /*RCM+VPM节点queryB捕获9ms query脉冲的次数*/

uint8_t elmswitchCloseFlga = 0x00;            /*ELM节点queryA电子开关闭合标志*/
uint8_t elmswitchCloseFlgb = 0x00;            /*ELM节点queryB电子开关闭合标志*/
uint8_t elmVpmswitchCloseFlga = 0x00;         /*ELM+VPM节点queryA电子开关闭合标志*/
uint8_t elmVpmswitchCloseFlgb = 0x00;         /*ELM+VPM节点queryB电子开关闭合标志*/
uint8_t rcmVpmswitchCloseFlga = 0x00;            /*RCM节点queryA电子开关闭合标志*/
uint8_t rcmVpmswitchCloseFlgb = 0x00;            /*RCM节点queryB电子开关闭合标志*/
/*query信号7ms和5ms的脉冲信号捕获处理函数*/
void queryCapDisposeFunc()
{
	if(g_tim2_queryA_cap_sta & 0x80)                                     /* 成功捕获到了一次高电平 */
	{
		queryAtemp = g_tim2_queryA_cap_sta & 0x3F;
		queryAtemp *= 0xFFFF;                                            /* 溢出时间总和 */
		queryAtemp += g_tim2_queryA_cap_val;                             /* 得到总的高电平时间 */
		/*捕获到5ms的query信号*/
		if((queryAtemp >= (5*1000-500))&&(queryAtemp <= (5*1000+500)))
		{
			queryAflag_5ms = 0x01;
			queryAflag = 0x00;
			queryAflag_9ms = 0x00;
			queryATmflag = 0x00;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}/*捕获到7ms的query信号*/
		else if((queryAtemp >=(7*1000-500))&&(queryAtemp <= (7*1000+500)))
		{
			queryAflag_5ms = 0x00;
			queryAflag = 0x01;
			queryAflag_9ms = 0x00;
			queryATmflag = 0x00;
			elmReportCnta++;
			elmVpmReportCnta++;
			rcmVpmReportCnta++;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
			
		} /*捕获到9ms的query信号*/
		else if((queryAtemp >= (9*1000-500))&&(queryAtemp <= (9*1000+500)))
		{
			queryAflag_5ms = 0x00;
			queryAflag = 0x00;
			queryAflag_9ms = 0x01;
			queryATmflag = 0x00;
			elmCapCnta++;
			elmVpmCapCnta++;
			rcmVpmCapCnta++;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}/*捕获到35ms的TM脉冲信号*/
		else if((queryAtemp >=(35*1000-500))&&(queryAtemp <= (35*1000+500)))
		{
			queryAflag_5ms = 0x00;
			queryAflag = 0x00;
			queryAflag_9ms = 0x00;
			queryATmflag = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}
		else
		{
			queryAflag_5ms = 0x00;
			queryAflag = 0x00;
			queryAflag_9ms = 0x00;
			queryATmflag = 0x00;	
		}
		g_tim2_queryA_cap_sta = 0;                                       /* 开启下一次捕获 */
	}

	if(g_tim2_queryB_cap_sta & 0x80)                                    /* 成功捕获到了一次高电平 */
	{
		queryBtemp = g_tim2_queryB_cap_sta & 0x3F;
		queryBtemp *= 0xFFFF;                                            /* 溢出时间总和 */
		queryBtemp += g_tim2_queryB_cap_val;                             /* 得到总的高电平时间 */
		/*捕获到5ms的query信号*/
		if((queryBtemp >=(5*1000-500))&&(queryBtemp <= (5*1000+500)))               /*捕获到5ms的query信号*/
		{
			queryBflag_5ms = 0x01;
			queryBflag = 0x00;
			queryBflag_9ms = 0x00;
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭ITM2中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		}/*捕获到7ms的query信号*/
		else if((queryBtemp >=(7*1000-500))&&(queryBtemp <= (7*1000+500)))
		{
			queryBflag_5ms = 0x00;
			queryBflag = 0x01;
			queryBflag_9ms = 0x00;
			elmReportCntb++;
			elmVpmReportCntb++;
			rcmVpmReportCntb++;
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		}/*捕获到9ms的query信号*/
		else if((queryBtemp >=(9*1000-500))&&(queryBtemp <= (9*1000+500)))               /*捕获到5ms的query信号*/
		{
			queryBflag_5ms = 0x00;
			queryBflag = 0x00;
			queryBflag_9ms = 0x01;
			elmCapCntb++;
			elmVpmCapCntb++;
			rcmVpmCapCntb++;
			
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭ITM2中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		}
		else
		{
			queryBflag_5ms = 0x00;
			queryBflag = 0x00;
			queryBflag_9ms = 0x00;
		}
		g_tim2_queryB_cap_sta = 0;                                       /* 开启下一次捕获 */
	}
}

/*TM节点捕获5ms 7ms 9ms query信号的处理函数*/
void tmCapQueryDisposeFunc()
{
	if(g_tim2_queryA_cap_sta & 0x80)                                     /* 成功捕获到了一次高电平 */
	{
		queryATmtemp = g_tim2_queryA_cap_sta & 0x3F;
		queryATmtemp *= 0xFFFF;                                            /* 溢出时间总和 */
		queryATmtemp += g_tim2_queryA_cap_val;                             /* 得到总的高电平时间 */
		/*捕获到5ms的query信号*/
		if((queryATmtemp >=5*1000-500)&&(queryATmtemp <= 5*1000+500))
		{
			tmqueryAflag_5ms = 0x01;
			tmqueryAflag = 0x00;
			tmqueryAflag_9ms = 0x00;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}/*捕获到7ms的query信号*/
		else if((queryATmtemp >=7*1000-500)&&(queryATmtemp <= 7*1000+500))
		{
			tmqueryAflag_5ms = 0x00;
			tmqueryAflag = 0x01;
			tmqueryAflag_9ms = 0x00;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}/*捕获到9ms的query信号*/
		else if((queryATmtemp >=9*1000-500)&&(queryATmtemp <= 9*1000+500))
		{
			tmqueryAflag_5ms = 0x00;
			tmqueryAflag = 0x00;
			tmqueryAflag_9ms = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* 关闭TIM2捕获中断 */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}
		else
		{
			tmqueryAflag_5ms = 0x00;
			tmqueryAflag = 0x00;
			tmqueryAflag_9ms = 0x00;
		}
		g_tim2_queryA_cap_sta = 0;                                       /* 开启下一次捕获 */
	}
}

/*query检测处理函数*/
void queryCheckDisposeFunc(uint8_t normalPulse,uint8_t abnormalPulse)
{
	if(queryAflag_5ms == 0x01||queryAflag == 0x01||queryAflag_9ms == 0x01)	
    {
		query_on_switch = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
		if(query_on_switch == 0x00)
		{
			if(lockflag == 0x01)		/*能检测到Lock信号*/
			{
				delay_ms(2);
				MCU_CT_QUERYA(1);
				delay_ms(normalPulse);
				MCU_CT_QUERYA(0);
			}
			else
			{
				delay_ms(2);
				MCU_CT_QUERYA(1);
				delay_ms(abnormalPulse);
				MCU_CT_QUERYA(0);
			}
			delay_us(100);
			QUERYAB_ON_OFF(1);
			query_on_switch = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
			if((queryAflag == 0x01)&&(query_on_switch == 0x01))
			{
				elmswitchCloseFlga = 0x01;
				elmVpmswitchCloseFlga = 0x01;
				rcmVpmswitchCloseFlga = 0x01;
			}
		}
		queryAflag = 0x00;
		queryAflag_5ms = 0x00;
		queryAflag_9ms = 0x00;
		lockflag = 0x00;
		queryBflag = 0x00;
	}

	if(queryBflag_5ms == 0x01||queryBflag == 0x01||queryBflag_9ms == 0x01)
    {
		query_on_switch = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
		if(query_on_switch == 0x00)
		{
			if(lockflag == 0x01)		/*能检测到Lock信号*/
			{
				delay_ms(2);
				MCU_CT_QUERYB(1);
				delay_ms(normalPulse);
				MCU_CT_QUERYB(0);
			}
			else
			{
				delay_ms(2);
				MCU_CT_QUERYB(1);
				delay_ms(abnormalPulse);
				MCU_CT_QUERYB(0);
			}
			delay_us(100);
			QUERYAB_ON_OFF(1);
			query_on_switch = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
			
			/*捕获到7ms query信号闭合电子开关后，节点要上报状态信息*/
			if((queryBflag == 0x01)&&(query_on_switch == 0x01))                   /*只有捕获到7ms的query信号，并且电子开关闭合，则电子开关标志置1*/
			{
				elmswitchCloseFlgb = 0x01;
				elmVpmswitchCloseFlgb = 0x01;
				rcmVpmswitchCloseFlgb = 0x01;
			}
		}
		queryBflag = 0x00;
		queryBflag_5ms = 0x00;
		queryBflag_9ms = 0x00;
		lockflag = 0x0;
		queryAflag = 0x00;
	}
	if(query_on_switch == 0x01)
	{
		TIM2->SR &= ~(0x02);                                           /*清除中断标志*/
		TIM2->SR &= ~(0x04);                                           /*清除中断标志*/
		
		HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);                 /* 关闭TIM2中断 */
		HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		
		HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* 开启TIM2中断 */
		HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		/*捕获到TM返回的35ms脉冲*/
		if(queryATmflag == 0x01)
		{
			delay_us(100);
			QUERYAB_ON_OFF(0);                                         /* 电子开关断开 */
			queryATmflag = 0x00;
			
			elmReportCnta = 0x00;
			elmReportCntb = 0x00;
			elmVpmReportCnta = 0x00;
			elmVpmReportCntb = 0x00;
			rcmVpmReportCnta = 0x00;
			rcmVpmReportCntb = 0x00;
			
			elmCapCnta = 0x00;
			elmCapCntb = 0x00;
			elmVpmCapCnta = 0x00;
			elmVpmCapCntb = 0x00;
			rcmVpmCapCnta = 0x00;
			rcmVpmCapCntb = 0x00;
		}
	}
	query_on_switch = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
	if(query_on_switch == 0)
	{
		HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);
		HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		HAL_NVIC_EnableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);
		HAL_TIM_IC_Start_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		
		//HAL_NVIC_EnableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);
		//HAL_TIM_IC_Start_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
	}
    return;
}

/*TM捕获到5ms 7ms 9ms query后，返回35ms脉冲的处理函数*/
void tmCheckDisposeFunc(uint8_t normalPulse)
{
	if(tmqueryAflag_5ms == 0x01||tmqueryAflag == 0x01||tmqueryAflag_9ms == 0x01)
    {
		delay_ms(2);
		MCU_CT_QUERYA(1);
		delay_ms(normalPulse);
		MCU_CT_QUERYA(0);
		
		tmqueryAflag_5ms = 0x00;
		tmqueryAflag = 0x00;
		tmqueryAflag_9ms = 0x00;
	}
	//TIM2->SR &= ~(0x02);                                           /*清除中断标志*/
	HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* 开启ITM2中断 */
	HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
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
        
        if((Extcmd == 0x01)&&((elmCapCnta == 0x01)||(elmCapCntb == 0x01)))/*子节点接收到"设置节点编号"的命令*/
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

	canbuf[0] = nodeData.NodeType;                                 /*子节点类型*/
	canbuf[1] = nodeData.PowerVolt;                                /*18V工作电压采集值*/
	canbuf[2] = nodeData.EmerStopKeyStatus;                        /*急停按钮状态*/
	canbuf[3] = nodeData.LockSignalStatus;                         /*lock闭锁信号状态*/
	canbuf[4] = AudioStatus;                                       /*音频信号状态*/
	/*第一次捕获到7ms query信号,上报数据*/
	if(((elmswitchCloseFlga == 0x01)||(elmswitchCloseFlgb == 0x01))&&((elmReportCnta == 0x01)||(elmReportCntb == 0x01)))
	{
		can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 5);     /*当电子开关闭合时，上报数据 5字节*/
		elmswitchCloseFlga = 0x00;
		elmswitchCloseFlgb = 0x00;
	}
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
                     
        if((Extcmd == 0x01)&&((elmVpmCapCnta == 0x01)||(elmVpmCapCntb == 0x01)))/*子节点接收到"设置节点编号"的命令*/
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
                BAT_CHARGE_EN(1);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 1;        /* 已充电*/
                can_send_msg_data(0x17800100|(NodeId<<16), canbuf, 0); /* 返回充电状态*/
            }
            else
            {
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
	
	/*第一次捕获到7ms query信号,上报数据*/
	if(((elmVpmswitchCloseFlga == 0x01)||(elmVpmswitchCloseFlgb == 0x01))&&((elmVpmReportCnta == 0x01)||(elmVpmReportCntb == 0x01)))
	{
		can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 6);     /*当电子开关闭合时，上报数据 6字节*/
		elmVpmswitchCloseFlga = 0x00;
		elmVpmswitchCloseFlgb = 0x00;
	}
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
	uint8_t startfailcmd;
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
        if((Extcmd == 0x01)&&((rcmVpmCapCnta == 0x01)||(rcmVpmCapCntb == 0x01)))/*子节点接收到"设置节点编号"的命令*/
		{
			NodeId = param1;                                        /*节点编号提取出来赋给NodeId */
			slaveAddr = NodeId;                                     /*从机地址等于节点编号*/
			if(slaveAddr>=1 && slaveAddr<=61)                       /*节点编号在1~61之*/
			{
				can_send_msg_data(0x11800300|(NodeId<<16), canbuf, 0);  /*发送节点类型 0x03*/
			}
        }
		else if((Extcmd == 0x03)&&(destAddr == slaveAddr))          /*子节点接收到"输出端口控制"的命令*/
		{
			/*以前是0x01:启动(灯亮)  0x00:停车(灯灭)*/
			/*改成0x00:启动(灯亮)  0x01:停车(灯灭)
			收到0反转成1 ，收到1反转成0*/
			
			if(param1 == 0x01)                                      /*参数1代表DO编号*/
			{
				if(param2 == 0x00)                                  /*参数2代表DO的电平，0低电平，1高电平*/
				{
					//RCM_DO1(0);
					//RcmOutputPort &= (~(1<<0));
					RCM_DO1(1);
					RcmOutputPort |= (1<<0);
				}
				else
				{	
					//RCM_DO1(1);
					//RcmOutputPort |= (1<<0);
					RCM_DO1(0);
					RcmOutputPort &= (~(1<<0));
				}
			}
			else if(param1 == 0x02)
			{
				if(param2 == 0x00)
				{
					//RCM_DO2(0);
					//RcmOutputPort &= (~(1<<1));
					RCM_DO2(1);
					RcmOutputPort |= (1<<1); 
				}
				else
				{
					//RCM_DO2(1);
					//RcmOutputPort |= (1<<1);
					RCM_DO2(0);
					RcmOutputPort &= (~(1<<1));
				}
			}
			else if(param1 == 0x03)
			{
				if(param2 == 0x00)
				{
					//RCM_DO3(0);
					//RcmOutputPort &= (~(1<<2));
					RCM_DO3(1);
					RcmOutputPort |= (1<<2);
				}
				else
				{
					//RCM_DO3(1);
					//RcmOutputPort |= (1<<2);
					RCM_DO3(0);
					RcmOutputPort &= (~(1<<2));
				}
			}
			else if(param1 == 0x04)
			{
				if(param2 == 0x00)
				{
					//RCM_DO4(0);
					//RcmOutputPort &= (~(1<<3));
					RCM_DO4(1);
					RcmOutputPort |= (1<<3);
				}
				else
				{
					//RCM_DO4(1);
					//RcmOutputPort |= (1<<3);
					RCM_DO4(0);
					RcmOutputPort &= (~(1<<3));
				}
			}
			canbuf[7] = RcmOutputPort;
			can_send_msg_data(g_can1_rxheader.ExtId, canbuf, 0);
			#if 1
			if(!can_receive_msg(g_can1_rxheader.ExtId, recvbuf))
			{
				startfailcmd = (uint8_t)g_can1_rxheader.ExtId;
				if(startfailcmd == 0x00)
				{
					canbuf[7] = 0x0F;
					//can_send_msg_data(g_can1_rxheader.ExtId, canbuf, 0);
				}
			}
			#endif
		}   /* “去抖时间设置指令”从1E改成08 */
		else if((Extcmd == 0x08)&&(destAddr == slaveAddr))        /*子节点接收到"去抖时间设置"的命令*/
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
		else if((Extcmd == 0x17)&&(destAddr == slaveAddr))               /*子节点接收到"充电控制"的命令*/
        {
            if(param1 == 0x01)  
            {
                BAT_CHARGE_EN(1);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 1;          /* 已充电*/
                can_send_msg_data(0x17800100|(NodeId<<16), canbuf, 0);   /* 返回充电状态*/
            }
            else
            {
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
			else
			{
				;
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
	/*第一次捕获到7ms query信号,上报数据*/
	if(((rcmVpmswitchCloseFlga == 0x01)||(rcmVpmswitchCloseFlgb == 0x01))&&((rcmVpmReportCnta == 0x01)||(rcmVpmReportCntb == 0x01)))
	{
		can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 8);     /*当电子开关闭合时，上报数据 8字节*/
		rcmVpmswitchCloseFlga = 0x00;
		rcmVpmswitchCloseFlgb = 0x00;
	}
	return 0;
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
	if(tmqueryAflag == 0x01)
	{
		can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 2);
	}		
    return 0;
}

