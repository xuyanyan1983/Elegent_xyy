#include "node.h"
#define CANBAUD 50

ADC_HandleTypeDef    AdcHandle1;
ADC_HandleTypeDef    AdcHandle2;
__IO uint16_t ADC1ConvertedValue1 = 0,ADC1ConvertedValue2 = 0;
uint8_t ADCPower;                           /* 18V电源电压采集的值的10倍 */
uint8_t ADCBat;                             /* 电池电压采集的值的10倍 */

NodeDataStruct g_node_data;                 /* 节点数据，代表一个节点*/

uint8_t keyflag = 0;                        /*有无按键按下标志 0:无按键按下，1：有按键按下*/
uint8_t NodeId;                             /*保存节点的编号，保存在RAM中，掉电数据丢失，节点编号可变 */
const uint8_t NodeType = RCM_VPM;      /*保存节点的类型 节点类型是只读的，保存在flash中，节点类型固定*/
uint8_t lockflag = 0;                       /* lock信号有无标志*/
extern TIM_HandleTypeDef g_tim2_queryA_cap_handle; /* 定时器x句柄 */
extern TIM_HandleTypeDef g_tim2_queryB_cap_handle; /* 定时器x句柄 */
int main(void)
{
    uint8_t key;
	uint8_t canbuf[4] = {0};
	uint32_t sjw;
	uint32_t tbs2;
	uint32_t tbs1;
	uint16_t brp;
	
	ADC_ChannelConfTypeDef sConfig1;
	ADC_ChannelConfTypeDef sConfig2;

    HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(10, 1, 7, 2, 2);   /* 设置时钟,40Mhz */
	delay_init(40);                         /* 延时初始化 系统时钟40MHZ*/
	
    //usart_init(115200);                   /* 串口初始化为115200 */
    QUERY_init();                           /* 查询信号初始化 */
    key_init();                             /* 初始化按键K1 K2 */

	if(CANBAUD == 500)                      /*CAN的波特率500Kbps*/
	{
		sjw = CAN_SJW_1TQ;
		tbs1 = CAN_BS1_10TQ;
		tbs2 = CAN_BS2_5TQ;
		brp = 5;
	}
	else if(CANBAUD == 50)                  /*CAN的波特率50Kbps*/
	{
		sjw = CAN_SJW_1TQ;
		tbs1 = CAN_BS1_4TQ;
		tbs2 = CAN_BS2_3TQ;
		brp = 100;
	}
	else if(CANBAUD == 5)                   /*CAN的波特率5Kbps*/
	{
		sjw = CAN_SJW_1TQ;
		tbs1 = CAN_BS1_4TQ;
		tbs2 = CAN_BS2_3TQ;
		brp = 1000;
	}
	else if(CANBAUD == 4)                   /*CAN的波特率4Kbps*/
	{
		sjw = CAN_SJW_1TQ;
		tbs1 = CAN_BS1_7TQ;
		tbs2 = CAN_BS2_2TQ;
		brp = 1000;
	}
	can_init(sjw, tbs2, tbs1, brp, CAN_MODE_NORMAL); /* CAN初始化, 普通模式, 波特率500Kbps */

    RCM_init();                                      /*RCM的4个输出端口初始化*/
    rcmdi_init();                                    /*RCM的8个输入端口初始化*/
    AudioCheck_init();                               /*音频检测输入端口初始化 ，根据高低电平判断音频信号有无，0：无音频，1：有音频*/
	/* lock信号不用捕获，只需要判断高低电平即可*/
    lock_init();                                     /* DC_lock和lock stop引脚初始化，DC_LOCK是高电平有效，lock_stop是低电平有效*/
	if(NodeType == TM)                               /*TM节点产生lock信号*/
	{
		/* queryA捕获query信号的高电平,timer2 is 32 bits*/
		queryA_gtim_timx_cap_chy_init(0xffff, 40 - 1);/*  以1Mhz的频率计数,1微s 捕获 */
		queryB_gtim_timx_cap_chy_init(0xffff, 40 - 1);
		/*关闭queryB的中断*/
		HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);/* 关闭TIM2通道2的中断 */
		HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);/* 关闭TIM2通道2的中断 */
		HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		gtim_pwm_lock_init();                          /*39KHzlock pwm脉冲信号产生*/
	}
	else
	{
		/* queryA捕获query信号的高电平,timer2 is 32 bits*/
		queryA_gtim_timx_cap_chy_init(0xffff, 40 - 1);                      /*  以1Mhz的频率计数,1微s 捕获 */
		queryB_gtim_timx_cap_chy_init(0xffff, 40 - 1); 
		queryA_gtim_timx_cap_tm_init(0xffff, 40 - 1); 
	}
	ADC1ConvertedValue1 = adc_sigChann_get_value(AdcHandle1,sConfig1,1);    /* 18V电源电压实际采集值 */
	/* STM32的ADC只能采集3.3V，实际电压是采集到的电压的6倍，由分压电阻计算*/
	ADCPower = (uint8_t)((ADC1ConvertedValue1*3.3*10*6)/4096);              /*乘以10倍，保留一位小数，将小数放到整数部分，取整*/         
	
	if((NodeType == ELM_VPM)||(NodeType == RCM_VPM))
	{
		ADC1ConvertedValue2 = adc_sigChann_get_value(AdcHandle2,sConfig2,2); /* 电池电压实际采集值 */
		ADCBat = (uint8_t)((ADC1ConvertedValue2*3.3*10*6)/4096);             /*乘以10倍，保留一位小数，将小数放到整数部分，取整*/ 
	}

    while (1)
    {
		if(NodeType == TM) 
		{
			/*TM捕获7ms query脉冲信号的函数*/
			tmCapQueryDisposeFunc(7);                                            /*TM捕获7ms的query信号*/
		}
		else
		{
			/*其他节点捕获7ms query脉冲信号的函数*/
			queryCapDisposeFunc();                                               /*开始捕获7ms query脉冲信号*/   
		}

		if((NodeType == ELM_VPM)||(NodeType == RCM_VPM))                     /*帶VPM的節點上有K1,K2按鈕*/
		{
			key = key_scan(0);
			if(key == KEY1_PRES || key == KEY2_PRES)
			{
				printf("有按键按下\r\n");
				keyflag = 1;
			}
		}
		
		if((NodeType != TM)&&(DC_LOCK == 1)&&(IN_BTN2 == 0))
		{
			lockflag = 0x01;                             /*检测到lock信号*/
		}
		
        if(NodeType == ELM)                              /* ELM子节点 */
        {
            g_node_data.NodeType = NodeType;             /*节点类型 */
            g_node_data.PowerVolt = ADCPower;            /* 18V电源电压 */
            g_node_data.EmerStopKeyStatus = IN_BTN2;     /*急停按钮状态*/
			queryCheckDisposeFunc(3,17);
            elmNodeDisposeFunc(g_node_data);             /*ELM子节点的处理函数 good*/
		}
        else if(NodeType == ELM_VPM)                     /* ELM_VPM子节点 */
        {
            g_node_data.NodeType = NodeType;             /*节点类型 */
            g_node_data.PowerVolt = ADCPower;            /* 18V电源电压 */
            g_node_data.BatVolt = ADCBat;                /* 电池电压 */
            g_node_data.EmerStopKeyStatus = IN_BTN2;     /* 急停按钮状态*/
			queryCheckDisposeFunc(7,17);
            elmVpmNodeDisposeFunc(g_node_data);          /*ELM VPM子节点的处理函数 */
		}
        else if(NodeType == RCM_VPM)                     /* RCM_VPM子节点 */
        {
            g_node_data.NodeType = NodeType;             /*节点类型 */
            g_node_data.PowerVolt = ADCPower;            /* 18V电源电压 */
            g_node_data.BatVolt = ADCBat;                /* 电池电压 */
            g_node_data.EmerStopKeyStatus = IN_BTN2;     /*急停按钮 0x00: 松开 0x01: 按下*/
			queryCheckDisposeFunc(9,17);
            rcmVpmNodeDisposeFunc(g_node_data);          /*RCM VPM子节点的处理函数 */
		}
        else if(NodeType == TM)                          /* TM子节点 */
        { 
            g_node_data.NodeType = NodeType;
			g_node_data.PowerVolt = ADCPower;            /* 18V电源电压 */
			/*TM捕获到query信号，返回35ms脉冲的函数*/
			tmCheckDisposeFunc(35);
			tmNodeDisposeFunc(g_node_data);              /*TM子节点的处理函数 */
        }
        else
        {
            /*节点类型错误*/
            printf("节点类型错误\r\n");
        }
    }
}


