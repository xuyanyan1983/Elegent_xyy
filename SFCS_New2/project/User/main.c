#include "node.h"
#define CANBAUD 50

#define ELMWIDE 11                          /*ELM����������*/
#define ELMVPMWIDE 13                       /*ELM+VPM����������*/
#define RCMVPMWIDE 15                       /*RCM+VPM����������*/
#define TMWIDE 35                           /*TM����������*/                
#define FAULTWIDEOFFSET 7                   /*����ʱ�������ƫ����*/

ADC_HandleTypeDef    AdcHandle1;
ADC_HandleTypeDef    AdcHandle2;
ADC_HandleTypeDef    AdcHandle3;
__IO uint16_t ADC1ConvertedValue1 = 0,ADC1ConvertedValue2 = 0,ADC1ConvertedValue3 = 0;
uint8_t ADCPower;                           /* 18V��Դ��ѹ�ɼ���ֵ��10�� */
uint8_t ADCBat;                             /* ��ص�ѹ�ɼ���ֵ��10�� */
uint8_t ADCBatCur;                          /* ��ص����ɼ���ֵ��10�� */

NodeDataStruct g_node_data;                 /* �ڵ����ݣ�����һ���ڵ�*/

uint8_t keyflag = 0;                        /*���ް������±�־ 0:�ް������£�1���а�������*/
uint8_t NodeId = 0x00;                      /*����ڵ�ı�ţ�������RAM�У��������ݶ�ʧ���ڵ��ſɱ� */
const uint8_t NodeType = RCM_VPM;//ELM_VPM; //RCM_VPM;/*����ڵ������ �ڵ�������ֻ���ģ�������flash�У��ڵ����͹̶�*/
uint8_t lockflag = 0;                       /* lock�ź����ޱ�־*/
extern TIM_HandleTypeDef g_tim2_queryA_cap_handle; /* ��ʱ��x��� */
extern TIM_HandleTypeDef g_tim2_queryB_cap_handle; /* ��ʱ��x��� */
uint8_t query_on_switch = 0;

int main(void)
{
    uint8_t key;
	uint32_t sjw;
	uint32_t tbs2;
	uint32_t tbs1;
	uint16_t brp;
	
	ADC_ChannelConfTypeDef sConfig1;        /*18V�ĵ�Դ��ѹ�ɼ�*/
	ADC_ChannelConfTypeDef sConfig2;        /*��ص�ѹ�ɼ�*/
	ADC_ChannelConfTypeDef sConfig3;        /*��ص����ɼ�*/

    HAL_Init();                             /* ��ʼ��HAL�� */
    sys_stm32_clock_init(10, 1, 7, 2, 2);   /* ����ʱ��,40Mhz */
	delay_init(40);                         /* ��ʱ��ʼ�� ϵͳʱ��40MHZ*/
	
    //usart_init(115200);                   /* ���ڳ�ʼ��Ϊ115200 */
    QUERY_init();                           /* ��ѯ�źų�ʼ�� */
    key_init();                             /* ��ʼ������K1 K2 */
	//gtim_pwm_query_init();                  /*����5ms��query�ź�*/

	if(CANBAUD == 500)                      /*CAN�Ĳ�����500Kbps*/
	{
		sjw = CAN_SJW_1TQ;
		tbs1 = CAN_BS1_10TQ;
		tbs2 = CAN_BS2_5TQ;
		brp = 5;
	}
	else if(CANBAUD == 50)                  /*CAN�Ĳ�����50Kbps*/
	{
		sjw = CAN_SJW_1TQ;
		tbs1 = CAN_BS1_4TQ;
		tbs2 = CAN_BS2_3TQ;
		brp = 100;
	}
	else if(CANBAUD == 5)                   /*CAN�Ĳ�����5Kbps*/
	{
		sjw = CAN_SJW_1TQ;
		tbs1 = CAN_BS1_4TQ;
		tbs2 = CAN_BS2_3TQ;
		brp = 1000;
	}
	else if(CANBAUD == 4)                   /*CAN�Ĳ�����4Kbps*/
	{
		sjw = CAN_SJW_1TQ;
		tbs1 = CAN_BS1_7TQ;
		tbs2 = CAN_BS2_2TQ;
		brp = 1000;
	}
	can_init(sjw, tbs2, tbs1, brp, CAN_MODE_NORMAL); /* CAN��ʼ��, ��ͨģʽ, ������500Kbps */

    RCM_init();                                      /*RCM��4������˿ڳ�ʼ��*/
    rcmdi_init();                                    /*RCM��8������˿ڳ�ʼ��*/
    AudioCheck_init();                               /*��Ƶ�������˿ڳ�ʼ�� �����ݸߵ͵�ƽ�ж���Ƶ�ź����ޣ�0������Ƶ��1������Ƶ*/
	/* lock�źŲ��ò���ֻ��Ҫ�жϸߵ͵�ƽ����*/
    lock_init();                                     /* DC_lock��lock stop���ų�ʼ����DC_LOCK�Ǹߵ�ƽ��Ч��lock_stop�ǵ͵�ƽ��Ч*/
	if(NodeType == TM)                               /*TM�ڵ����lock�ź�*/
	{
		/* queryA����query�źŵĸߵ�ƽ,timer2 is 32 bits*/
		queryA_gtim_timx_cap_chy_init(0xFFFF, 40 - 1);/*  ��1Mhz��Ƶ�ʼ���,1΢s ���� */
		queryB_gtim_timx_cap_chy_init(0xFFFF, 40 - 1);
		/*�ر�queryB���ж�*/
		HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);/* �ر�TIM2ͨ��2���ж� */
		HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		
		HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);/* �ر�TIM2ͨ��2���ж� */
		HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		
		gtim_pwm_lock_init();                          /*39KHzlock pwm�����źŲ���*/
	}
	else
	{
		queryA_gtim_timx_cap_chy_init(0xFFFF, 40 - 1);
		queryB_gtim_timx_cap_chy_init(0xFFFF, 40 - 1);
	}
	
	/* ��Դ��ѹx*200k /(1M+200k)  = pwe��Դ��ѹ�Ĳɼ�ֵ ->  x= pwe*6 
		�ɼ���ѹpwe*3.3/4096 ����ʵ�ʲɼ��ĵ�ѹֵ����12λ��ADC��
	*/
	ADC1ConvertedValue1 = adc_sigChann_get_value(AdcHandle1,sConfig1,1);    /* 18V��Դ��ѹʵ�ʲɼ�ֵ */
	/* STM32��ADCֻ�ܲɼ�3.3V��ʵ�ʵ�ѹ�ǲɼ����ĵ�ѹ��6�����ɷ�ѹ�������*/
	ADCPower = (uint8_t)((ADC1ConvertedValue1*3.3*10*6)/4096);              /*����10��������һλС������С���ŵ��������֣�ȡ��*/         
	
	if((NodeType == ELM_VPM)||(NodeType == RCM_VPM))
	{
		ADC1ConvertedValue2 = adc_sigChann_get_value(AdcHandle2,sConfig2,2); /* ��ص�ѹʵ�ʲɼ�ֵ */
		ADCBat = (uint8_t)((ADC1ConvertedValue2*3.3*10*6)/4096);             /*����10��������һλС������С���ŵ��������֣�ȡ��*/

		ADC1ConvertedValue3 = adc_sigChann_get_value(AdcHandle3,sConfig3,3); /* ��ص�ѹʵ�ʲɼ�ֵ */
		ADCBatCur = (uint8_t)((ADC1ConvertedValue3*3.3*10*6)/(4096*0.2));    /*����10��������һλС������С���ŵ��������֣�ȡ��,��ѹ���Ե���ת�ɵ�������λ��΢������A��*/
	}

	HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* �ر�ITM2�ж� */
    while (1)
    {
		if((NodeType == ELM_VPM)||(NodeType == RCM_VPM))                     /*��VPM�Ĺ��c����K1,K2���o*/
		{
			key = key_scan(0);
			if(key == KEY1_PRES || key == KEY2_PRES)
			{
				keyflag = 1;
			}
		}
		
		if((NodeType != TM)&&(DC_LOCK == 1)&&(IN_BTN2 == 0))
		{
			lockflag = 0x01;                             /*��⵽lock�ź�*/
		}

		if(NodeType == TM) 
		{
			tmCapQueryDisposeFunc();                     /*TM����5ms 7ms 9ms��query�ź�*/
		}
		else
		{
			query_on_switch = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
			queryCapDisposeFunc();                       /*��ʼ����5ms 7ms 9ms query�����ź�*/   
		}
		
        if(NodeType == ELM)                              /* ELM�ӽڵ� */
        {
            g_node_data.NodeType = NodeType;             /*�ڵ����� */
            g_node_data.PowerVolt = ADCPower;            /* 18V��Դ��ѹ */
            g_node_data.EmerStopKeyStatus = IN_BTN2;     /*��ͣ��ť״̬*/
			queryCheckDisposeFunc(ELMWIDE,ELMWIDE+FAULTWIDEOFFSET);
            elmNodeDisposeFunc(g_node_data);             /*ELM�ӽڵ�Ĵ����� good*/
		}
        else if(NodeType == ELM_VPM)                     /* ELM_VPM�ӽڵ� */
        {
            g_node_data.NodeType = NodeType;             /*�ڵ����� */
            g_node_data.PowerVolt = ADCPower;            /* 18V��Դ��ѹ */
            g_node_data.BatVolt = ADCBat;                /* ��ص�ѹ */
            g_node_data.EmerStopKeyStatus = IN_BTN2;     /* ��ͣ��ť״̬*/
			queryCheckDisposeFunc(ELMVPMWIDE,ELMVPMWIDE+FAULTWIDEOFFSET);
            elmVpmNodeDisposeFunc(g_node_data);          /*ELM VPM�ӽڵ�Ĵ����� */
		}
        else if(NodeType == RCM_VPM)                     /* RCM_VPM�ӽڵ� */
        {
            g_node_data.NodeType = NodeType;             /*�ڵ����� */
            g_node_data.PowerVolt = ADCPower;            /* 18V��Դ��ѹ */
            g_node_data.BatVolt = ADCBat;                /* ��ص�ѹ */
            g_node_data.EmerStopKeyStatus = IN_BTN2;     /*��ͣ��ť 0x00: �ɿ� 0x01: ����*/
			queryCheckDisposeFunc(RCMVPMWIDE,RCMVPMWIDE+FAULTWIDEOFFSET);
            rcmVpmNodeDisposeFunc(g_node_data);          /*RCM VPM�ӽڵ�Ĵ����� */
		}
        else if(NodeType == TM)                          /* TM�ӽڵ� */
        { 
            g_node_data.NodeType = NodeType;
			g_node_data.PowerVolt = ADCPower;            /* 18V��Դ��ѹ */
			tmCheckDisposeFunc(TMWIDE);
			tmNodeDisposeFunc(g_node_data);              /*TM�ӽڵ�Ĵ����� */
        }
        else
        {
            /*�ڵ����ʹ���*/
			;
        }
    }
}


