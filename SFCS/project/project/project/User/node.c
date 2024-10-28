#include <stdint.h>
#include <stdio.h>
#include "node.h"

#define DELAYTIME 300
#define QUERYAB_ON_OFF_STATE HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
#define RCMDINUM 8                                 /* RCM��DI������*/
#define DEBOUNCETIME 3                             /*ȥ������3��*/

extern uint8_t lockflag;                           /* lock�ź����ޱ�־*/
extern uint8_t keyflag;                            /*���ް������±�־ 0:�ް������£�1���а�������*/
extern uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
extern uint8_t NodeId;                             /*����ڵ�ı�ţ�������RAM�У��������ݶ�ʧ���ڵ��ſɱ� */
extern CAN_RxHeaderTypeDef g_can1_rxheader;        /* ���ղ������ */

uint16_t debounceTimeSet[8];                       /* ����ȥ������*/
volatile uint8_t rcmDiItFlag[8] = {0};
uint8_t rcmDiInitStatus[8] = {0};                  /* RCM8��DI�ĳ�ʼ״̬*/ 

extern TIM_HandleTypeDef g_tim2_queryA_cap_handle; /* ��ʱ��x��� */
extern TIM_HandleTypeDef g_tim2_queryB_cap_handle; /* ��ʱ��x��� */
extern TIM_HandleTypeDef g_tim2_queryA_cap_tm_handle;

uint8_t destAddr;                                  /*Զ��֡��Ŀ���ַ*/
uint8_t slaveAddr;                                 /*��ӦԶ��֡�Ĵӻ���ַ*/

uint8_t RcmOutputPort = 0x00;
uint8_t canbuf[8];

extern uint8_t g_tim2_queryA_cap_sta;       /* ���벶��״̬ */
extern uint16_t g_tim2_queryA_cap_val;      /* ���벶��ֵ */

extern uint8_t g_tim2_queryA_cap_tm_sta;       /* ���벶��״̬ */
extern uint16_t g_tim2_queryA_cap_tm_val;      /* ���벶��ֵ */

extern uint8_t g_tim2_queryB_cap_sta;       /* ���벶��״̬ */
extern uint16_t g_tim2_queryB_cap_val;      /* ���벶��ֵ */
uint8_t queryAflag = 0;                     /* queryA����query�ź����ޱ�־*/
uint8_t queryBflag = 0;                     /* queryB����query�ź����ޱ�־*/
uint8_t tmQueryAflag = 0;                   /* TM�ڵ㲶��query�ź����ޱ�־*/
uint8_t queryAflag_5ms = 0;                 /* queryA����query�ź����ޱ�־*/
uint8_t queryBflag_5ms = 0;                 /* queryB����query�ź����ޱ�־*/
uint8_t queryATmflag = 0;                   /* queryA����TM���ޱ�־*/
uint8_t queryBTmflag = 0;
uint32_t queryAtemp = 0;                    /* queryA����7ms�ļ���ֵ*/
uint32_t queryBtemp = 0;                    /* queryB����7ms�ļ���ֵ*/
uint32_t queryATmtemp = 0;                  /* queryA����35ms�ļ���ֵ*/
uint32_t tmqueryAtemp = 0;                  /*TM����7ms�ļ���ֵ*/

/*query��⴦����*/
void queryCheckDisposeFunc(uint8_t normalPulse,uint8_t abnormalPulse)
{
	if(queryAflag == 0x01||queryAflag_5ms == 0x01)	
    {
		if(lockflag == 0x01)		/*�ܼ�⵽Lock�ź�*/
		{
			delay_us(DELAYTIME);
			printf("����query�źţ��պϵ��ӿ��� QUERYAB_ON/OFF\r\n");
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
		//can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 8);         /*�ϱ����� 8�ֽ�*/
	}

	if(queryBflag == 0x01||queryBflag_5ms == 0x01)
    {
		if(lockflag == 0x01)		/*�ܼ�⵽Lock�ź�*/
		{
			delay_us(DELAYTIME);
			printf("����query�źţ��պϵ��ӿ��� QUERYAB_ON/OFF\r\n");
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
		//can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 8);     /*�ϱ����� 8�ֽ�*/
	}
	
	delay_us(10);	
	QUERYAB_ON_OFF(1);                                             /* ���ӿ��رպ� */
	TIM2->SR &= ~(0x02);                                           /*����жϱ�־*/
	TIM2->SR &= ~(0x04);                                           /*����жϱ�־*/
	
	HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);                 /* �ر�TIM2�ж� */
	HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
	
	HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* ����TIM2�ж� */ //xyy1-  //xyy2+
	HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);            //xyy1-  //xyy2+
	tmPulseCapDisposeFunc(35);
	/*����35ms��TM����*/
	if(queryATmflag == 0x01)
	{
		/*�ر�A����35ms����*/
		HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* ����TIM2�ж� */ //xyy1-  //xyy2+
		HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		delay_ms(1);
		QUERYAB_ON_OFF(0);                                         /* ���ӿ��ضϿ� */
		/*��A��B����query�ź�*/
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

/*TM�ڵ㲶��7ms query�źţ�����35ms�Ĵ�����*/
void tmCheckDisposeFunc(uint8_t normalPulse)
{
	if(tmQueryAflag == 0x01||queryAflag_5ms == 0x01)	
    {
		delay_us(DELAYTIME);
		printf("����query�źţ��պϵ��ӿ��� QUERYAB_ON/OFF\r\n");
		MCU_CT_QUERYA(1);
		delay_ms(normalPulse);
		MCU_CT_QUERYA(0);
		tmQueryAflag = 0x00;
		queryAflag_5ms = 0x00;
	}
	TIM2->SR &= ~(0x02);                                           /*����жϱ�־*/
	
	HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* ����ITM2�ж� */
	HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
}

/*query�ź�7ms��5ms�������źŲ�������*/
void queryCapDisposeFunc()
{
	if(g_tim2_queryA_cap_sta & 0x80)                                     /* �ɹ�������һ�θߵ�ƽ */
	{
		queryAtemp = g_tim2_queryA_cap_sta & 0x3F;
		queryAtemp *= 0xFFFF;                                            /* ���ʱ���ܺ� */
		queryAtemp += g_tim2_queryA_cap_val;                             /* �õ��ܵĸߵ�ƽʱ�� */
		/*����7ms��query�ź�*/
		if((queryAtemp >=(7*1000-1000))&&(queryAtemp <= (7*1000+1000)))
		{
			queryAflag = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* �ر�TIM2�����ж� */ //xyy1-
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);            //xyy1-
		} /*����5ms��query�ź�*/
		if((queryAtemp >= (5*1000-1000))&&(queryAtemp <= (5*1000+1000)))
		{
			queryAflag_5ms = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* �ر�TIM2�ж� */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
		}
		g_tim2_queryA_cap_sta = 0;                                       /* ������һ�β��� */
	}

	if(g_tim2_queryB_cap_sta & 0x80)                                    /* �ɹ�������һ�θߵ�ƽ */
	{
		queryBtemp = g_tim2_queryB_cap_sta & 0x3F;
		queryBtemp *= 0xFFFF;                                            /* ���ʱ���ܺ� */
		queryBtemp += g_tim2_queryB_cap_val;                             /* �õ��ܵĸߵ�ƽʱ�� */
		/*����7ms��query�ź�*/
		if((queryBtemp >=(7*1000-1000))&&(queryBtemp <= (7*1000+1000)))
		{
			queryBflag = 0x01;
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* �ر�TIM2�����ж� */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}/*����5ms��query�ź�*/
		if((queryBtemp >=(5*1000-1000))&&(queryBtemp <= (5*1000+1000)))               /*����5ms��query�ź�*/
		{
			queryBflag_5ms = 0x01;
			HAL_NVIC_DisableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);              /* �ر�ITM2�ж� */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);
		}
		g_tim2_queryB_cap_sta = 0;                                       /* ������һ�β��� */
	}
}

/*TM�ڵ㲶��query�źŵĴ�������TMֻ��queryA����*/
void tmCapQueryDisposeFunc(uint8_t capturePulse)
{
	if(g_tim2_queryA_cap_sta & 0x80)                                     /* �ɹ�������һ�θߵ�ƽ */
	{
		tmqueryAtemp = g_tim2_queryA_cap_sta & 0x3F;
		tmqueryAtemp *= 0xFFFF;                                            /* ���ʱ���ܺ� */
		tmqueryAtemp += g_tim2_queryA_cap_val;                             /* �õ��ܵĸߵ�ƽʱ�� */
		/*����7ms��query�ź�*/
		if((tmqueryAtemp >=(7*1000-1000))&&(tmqueryAtemp <= (7*1000+1000)))
		{
			tmQueryAflag = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* �ر�TIM2�����ж� */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			tmqueryAtemp = 0;
		}
		g_tim2_queryA_cap_sta = 0;                                       /* ������һ�β��� */
	}
}

/*queryA����35ms��TM���ص�����*/
void tmPulseCapDisposeFunc(uint8_t capturePulse)
{
	if(g_tim2_queryA_cap_tm_sta & 0x80)                                  /* �ɹ�������һ�θߵ�ƽ */
	{
		queryATmtemp = g_tim2_queryA_cap_tm_sta & 0x3F;
		queryATmtemp *= 0xFFFF;                                          /* ���ʱ���ܺ� */
		queryATmtemp += g_tim2_queryA_cap_tm_val;                        /* �õ��ܵĸߵ�ƽʱ�� */
		/*����35ms��TM�����ź�*/
		if((queryATmtemp >=(capturePulse*1000-1000))&&(queryATmtemp <= (capturePulse*1000+1000)))
		{
			queryATmflag = 0x01;
			HAL_NVIC_DisableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);              /* �ر�TIM2�����ж� */
			HAL_TIM_IC_Stop_IT(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);
			queryATmtemp = 0;
		}
		g_tim2_queryA_cap_tm_sta = 0;                                    /* ������һ�β��� */
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
	if(lockflag == 0x01)                                          /*�ܼ�⵽Lock�ź�*/
	{
		nodeData.LockSignalStatus = 0x01;                         /*�ܼ�⵽Lock�ź�*/
	}
	else
	{
		nodeData.LockSignalStatus = 0x00;                         /*�ܼ�ⲻ��Lock�ź�*/
	}
	
	if(AUDIO_OUT == 1)
    {
        AudioStatus = 0x01;                                        /* ��⵽��Ƶ�ź�*/
    }
    else
    {
        AudioStatus = 0x00;                                        /* û�м�⵽��Ƶ�ź�*/
    }
	
    CAN_EN(0);                                                     /* canʹ��*/
	
    if(!can_receive_msg(g_can1_rxheader.ExtId, recvbuf))
    {
		Extcmd = (g_can1_rxheader.ExtId >> 24)&0xFF;
		param1 = (uint16_t)g_can1_rxheader.ExtId >>8;
		destAddr = (g_can1_rxheader.ExtId>>16)&0x7F;
        if(Extcmd == 0x01)                                         /*�ӽڵ���յ�"���ýڵ���"������*/
        {
            NodeId = param1;                                       /*�ڵ�����ȡ��������NodeId ��������*/
			slaveAddr = NodeId;                                    /*�ӻ���ַ���ڽڵ���*/
			if(slaveAddr>=1&&slaveAddr<=61)
			{
				can_send_msg_data(0x11800100|(NodeId<<16), canbuf, 0); /*���ͽڵ��ţ����ͽڵ����� 0x01*/ 
			}
        }
		else
		{
			;
		}
    }
    //CAN_EN(1);                                                   /*CAN��ֹ */
	canbuf[0] = nodeData.NodeType;                                 /*�ӽڵ�����*/
	canbuf[1] = nodeData.PowerVolt;                                /*18V������ѹ�ɼ�ֵ*/
	canbuf[2] = nodeData.EmerStopKeyStatus;                        /*��ͣ��ť״̬*/
	canbuf[3] = nodeData.LockSignalStatus;                         /*lock�����ź�״̬*/
	canbuf[4] = AudioStatus;                                       /*��Ƶ�ź�״̬*/
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
        nodeData.AudioChargeKeyStatus.Key1Key2Status = 0x01;          /*�а�������*/
    }
    else
    {
         nodeData.AudioChargeKeyStatus.Key1Key2Status = 0x00;         /*û�а�������*/
    }
	
    if(AUDIO_OUT == 1)
    {
        nodeData.AudioChargeKeyStatus.AudioStatus = 0x01;             /* ��⵽��Ƶ�ź�*/
    }
    else
    {
        nodeData.AudioChargeKeyStatus.AudioStatus = 0x00;             /* û�м�⵽��Ƶ�ź�*/
    }
	
	if(lockflag == 0x01)                                              /*�ܼ�⵽Lock�ź�*/
	{
		nodeData.LockSignalStatus = 0x01;                             /*�ܼ�⵽Lock�ź�*/
	}
	else
	{
		nodeData.LockSignalStatus = 0x00;                             /*�ܼ�ⲻ��Lock�ź�*/
	}
	
    CAN_EN(0); /* canʹ��*/
    if(!can_receive_msg(g_can1_rxheader.ExtId, recvbuf))
    { 
		Extcmd = (g_can1_rxheader.ExtId >> 24)&0xFF;
        param1 = (uint16_t)g_can1_rxheader.ExtId >>8;
		destAddr = (g_can1_rxheader.ExtId>>16)&0x7F;
        if(Extcmd == 0x01)                                            /*�ӽڵ���յ�"���ýڵ���"������*/
        {
            NodeId = param1;                                          /*�ڵ�����ȡ��������NodeId */
			slaveAddr = NodeId;                                       /*�ӻ���ַ���ڽڵ���*/
			if(slaveAddr>=1 && slaveAddr<=61)
			{
				can_send_msg_data(0x11800200|(NodeId<<16), canbuf, 0);    /*���ͽڵ��ţ����ͽڵ����� 0x02*/ 
			}
        }
        else if((Extcmd == 0x17)&&(destAddr == slaveAddr))            /*�ӽڵ���յ�"������"������*/
        {
            if(param1 == 0x01)  
            {
                printf("������\r\n");
                BAT_CHARGE_EN(1);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 1;        /* �ѳ��*/
                can_send_msg_data(0x17800100|(NodeId<<16), canbuf, 0); /* ���س��״̬*/
            }
            else
            {
                printf("��ֹ���\r\n");
                BAT_CHARGE_EN(0);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 0;        /* δ���*/
                can_send_msg_data(0x17800000|(NodeId<<16), canbuf, 0); /* ���س��״̬*/
            }
        }
		else
		{
			;
		}
    }
    //CAN_EN(1);                                                   /*CAN��ֹ */
	canbuf[0] = nodeData.NodeType;                                 /*�ӽڵ�����*/
	canbuf[1] = nodeData.PowerVolt;                                /*18V������ѹ�ɼ�ֵ*/
	canbuf[2] = nodeData.BatVolt;                                  /*��ص�ѹ�ɼ�ֵ*/
	canbuf[3] = nodeData.EmerStopKeyStatus;                        /*��ͣ��ť״̬*/
	canbuf[4] = nodeData.LockSignalStatus;                         /*lock�����ź�״̬*/
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

/*�յ�ȥ��ʱ��ָ��󣬴�����*/
uint8_t rcmDiDebounceDisposeFunc(uint16_t debounceTimeSet[])
{
	uint8_t tempValue[3] = {0};                                               /*ȥ������,�ɼ���ֵ*/
	uint8_t j;
	uint8_t rcmDiStatusFlag = 0;                                              /*RCM�ڵ��DI״̬�仯��־��0�ޱ仯��1�б仯*/
	uint8_t rcmDiValue[8] = {0};   
	
	rcmDiValue[0] = RCMDI1;
	rcmDiValue[1] = RCMDI2;
	rcmDiValue[2] = RCMDI3;
	rcmDiValue[3] = RCMDI4;
	rcmDiValue[4] = RCMDI5;
	rcmDiValue[5] = RCMDI6;
	rcmDiValue[6] = RCMDI7;
	rcmDiValue[7] = RCMDI8;
	
	if(rcmDiItFlag[0] == 1)                                                   /*��־λΪ1��ʾDI�б仯*/
	{
		if(rcmDiValue[0] != rcmDiInitStatus[0])                               /*DI�����ڳ�ʼֵ���б仯*/
		{
			delay_ms(debounceTimeSet[0]);                                     /* ȥ��*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*�ɼ�����*/
			{
				tempValue[j] = rcmDiValue[0];                                 /*����ɼ����ε�ֵ*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*�ж����βɼ���ֵ��ͬ��DI1û�б仯*/
			{
				printf("DI��ֵ�б仯,�ϱ�����\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[0] = rcmDiValue[0];
			}
			else                                                              /*�ж����βɼ���ֵ����ͬ��DI�б仯���ϱ�*/
			{
				rcmDiInitStatus[0] = tempValue[2];
				delay_ms(debounceTimeSet[0]);
			}
		}
	}
	if(rcmDiItFlag[1] == 1)                                                   /*��־λΪ1��ʾDI�б仯*/
	{
		if(rcmDiValue[1] != rcmDiInitStatus[1])                               /*DI�����ڳ�ʼֵ���б仯*/
		{
			delay_ms(debounceTimeSet[1]);                                     /* ȥ��*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*�ɼ�����*/
			{
				tempValue[j] = rcmDiValue[1];                                 /*����ɼ����ε�ֵ*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*�ж����βɼ���ֵ��ͬ��DI1û�б仯*/
			{
				printf("DI��ֵ�б仯,�ϱ�����\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[1] = rcmDiValue[1];
			}
			else                                                              /*�ж����βɼ���ֵ����ͬ��DI�б仯���ϱ�*/
			{
				delay_ms(debounceTimeSet[1]);
			}
		}
	}
	if(rcmDiItFlag[2] == 1)                                                   /*��־λΪ1��ʾDI�б仯*/
	{
		if(rcmDiValue[2] != rcmDiInitStatus[2])                               /*DI�����ڳ�ʼֵ���б仯*/
		{
			delay_ms(debounceTimeSet[2]);                                     /* ȥ��*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*�ɼ�����*/
			{
				tempValue[j] = rcmDiValue[2];                                 /*����ɼ����ε�ֵ*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*�ж����βɼ���ֵ��ͬ��DI1û�б仯*/
			{
				printf("DI��ֵ�б仯,�ϱ�����\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[2] = rcmDiValue[2];
			}
			else                                                              /*�ж����βɼ���ֵ����ͬ��DI�б仯���ϱ�*/
			{
				delay_ms(debounceTimeSet[2]);
			}
		}
	}
	if(rcmDiItFlag[3] == 1)                                                   /*��־λΪ1��ʾDI�б仯*/
	{
		if(rcmDiValue[3] != rcmDiInitStatus[3])                               /*DI�����ڳ�ʼֵ���б仯*/
		{
			delay_ms(debounceTimeSet[3]);                                     /* ȥ��*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*�ɼ�����*/
			{
				tempValue[j] = rcmDiValue[3];                                 /*����ɼ����ε�ֵ*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*�ж����βɼ���ֵ��ͬ��DI1û�б仯*/
			{
				printf("DI��ֵ�б仯,�ϱ�����\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[3] = rcmDiValue[3];
			}
			else                                                              /*�ж����βɼ���ֵ����ͬ��DI�б仯���ϱ�*/
			{
				delay_ms(debounceTimeSet[3]);
			}
		}
	}
	if(rcmDiItFlag[4] == 1)                                                   /*��־λΪ1��ʾDI�б仯*/
	{
		if(rcmDiValue[4] != rcmDiInitStatus[4])                               /*DI�����ڳ�ʼֵ���б仯*/
		{
			delay_ms(debounceTimeSet[4]);                                     /* ȥ��*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*�ɼ�����*/
			{
				tempValue[j] = rcmDiValue[4];                                 /*����ɼ����ε�ֵ*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*�ж����βɼ���ֵ��ͬ��DI1û�б仯*/
			{
				printf("DI��ֵ�б仯,�ϱ�����\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[4] = rcmDiValue[4];
			}
			else                                                              /*�ж����βɼ���ֵ����ͬ��DI�б仯���ϱ�*/
			{
				delay_ms(debounceTimeSet[4]);
			}
		}
	}
	if(rcmDiItFlag[5] == 1)                                                   /*��־λΪ1��ʾDI�б仯*/
	{
		if(rcmDiValue[5] != rcmDiInitStatus[5])                               /*DI�����ڳ�ʼֵ���б仯*/
		{
			delay_ms(debounceTimeSet[5]);                                     /* ȥ��*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*�ɼ�����*/
			{
				tempValue[j] = rcmDiValue[5];                                 /*����ɼ����ε�ֵ*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*�ж����βɼ���ֵ��ͬ��DI1û�б仯*/
			{
				printf("DI��ֵ�б仯,�ϱ�����\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[5] = rcmDiValue[5];
			}
			else                                                              /*�ж����βɼ���ֵ����ͬ��DI�б仯���ϱ�*/
			{
				delay_ms(debounceTimeSet[5]);
			}
		}
	}
	if(rcmDiItFlag[6] == 1)                                                   /*��־λΪ1��ʾDI�б仯*/
	{
		if(rcmDiValue[6] != rcmDiInitStatus[6])                               /*DI�����ڳ�ʼֵ���б仯*/
		{
			delay_ms(debounceTimeSet[6]);                                     /* ȥ��*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*�ɼ�����*/
			{
				tempValue[j] = rcmDiValue[6];                                 /*����ɼ����ε�ֵ*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*�ж����βɼ���ֵ��ͬ��DI1û�б仯*/
			{
				printf("DI��ֵ�б仯,�ϱ�����\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[1] = rcmDiValue[6];
			}
			else                                                              /*�ж����βɼ���ֵ����ͬ��DI�б仯���ϱ�*/
			{
				delay_ms(debounceTimeSet[6]);
			}
		}
	}
	if(rcmDiItFlag[7] == 1)                                                   /*��־λΪ1��ʾDI�б仯*/
	{
		if(rcmDiValue[7] != rcmDiInitStatus[7])                               /*DI�����ڳ�ʼֵ���б仯*/
		{
			delay_ms(debounceTimeSet[7]);                                     /* ȥ��*/
			for(j=0;j<DEBOUNCETIME;j++)                                       /*�ɼ�����*/
			{
				tempValue[j] = rcmDiValue[7];                                 /*����ɼ����ε�ֵ*/
			}
			if(tempValue[0] == tempValue[1] && tempValue[0] == tempValue[2])  /*�ж����βɼ���ֵ��ͬ��DI1û�б仯*/
			{
				printf("DI��ֵ�б仯,�ϱ�����\r\n");
				rcmDiStatusFlag = 1;
				rcmDiInitStatus[7] = rcmDiValue[7];
			}
			else                                                              /*�ж����βɼ���ֵ����ͬ��DI�б仯���ϱ�*/
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
	uint8_t rcmDiStatusFlag = 0;                                    /*RCM�ڵ��DI״̬�仯��־��0�ޱ仯��1�б仯*/
	uint8_t rs485ComAddr;                                           /*RS485��ͨ�ŵ�ַ*/
    uint32_t rs485Baud;                                             /*RS485��ͨ�Ų�����*/
	
	delay_us(500);
	if(nodeData.EmerStopKeyStatus == 0x01)                          /*��ͣ��ť���£��Ͽ�����DO*/ 
	{
		RCM_DO1(0);                                                 /*�͵�ƽ�Ͽ�*/
		RCM_DO2(0);
		RCM_DO3(0);
		RCM_DO4(0);
	}
	if(keyflag == 0x01)
    {
        nodeData.AudioChargeKeyStatus.Key1Key2Status = 1;          /*�а�������*/
    }
    else
    {
         nodeData.AudioChargeKeyStatus.Key1Key2Status = 0;         /*û�а�������*/
    }
    
    if(AUDIO_OUT == 1)
    {
        nodeData.AudioChargeKeyStatus.AudioStatus = 1;             /* ��⵽��Ƶ�ź�*/
    }
    else
    {
        nodeData.AudioChargeKeyStatus.AudioStatus = 0;             /* û�м�⵽��Ƶ�ź�*/
    }
	
	if(lockflag == 0x01)                                           /*�ܼ�⵽Lock�ź�*/
	{
		nodeData.LockSignalStatus = 0x01;                          /*�ܼ�⵽Lock�ź�*/
	}
	else
	{
		nodeData.LockSignalStatus = 0x00;                          /*�ܼ�ⲻ��Lock�ź�*/
	}

	canbuf[0] = nodeData.NodeType;                                 /*�ӽڵ�����*/
	canbuf[1] = nodeData.PowerVolt;                                /*18V������ѹ�ɼ�ֵ*/
	canbuf[2] = nodeData.BatVolt;                                  /*��ص�ѹ�ɼ�ֵ*/
	canbuf[3] = nodeData.EmerStopKeyStatus;                        /*��ͣ��ť״̬*/
	canbuf[4] = nodeData.LockSignalStatus;                         /*lock�����ź�״̬*/
	
    CAN_EN(0);                                                      /* canʹ��*/
    if(!can_receive_msg(g_can1_rxheader.ExtId, recvbuf))
    {
        Extcmd = (g_can1_rxheader.ExtId >> 24)&0xFF;
        param1 = (uint16_t)g_can1_rxheader.ExtId >>8;
        param2 = (uint8_t)g_can1_rxheader.ExtId;
		destAddr = (g_can1_rxheader.ExtId>>16)&0x7F;
        if(Extcmd == 0x01)                                          /*�ӽڵ���յ�"���ýڵ���"������*/
        {
            NodeId = param1;                                        /*�ڵ�����ȡ��������NodeId */
			slaveAddr = NodeId;                                     /*�ӻ���ַ���ڽڵ���*/
			if(slaveAddr>=1 && slaveAddr<=61)
			{
				can_send_msg_data(0x11800300|(NodeId<<16), canbuf, 0);  /*���ͽڵ����� 0x03*/
			}
        }
		else if((Extcmd == 0x03)&&(destAddr == slaveAddr))          /*�ӽڵ���յ�"����˿ڿ���"������*/
		{
			if(param1 == 0x01)                                      /*����1����DO���*/
			{
				if(param2 == 0x00)                                  /*����2����DO�ĵ�ƽ��0�͵�ƽ��1�ߵ�ƽ*/
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
		else if((Extcmd == 0x17)&&(destAddr == slaveAddr))               /*�ӽڵ���յ�"������"������*/
        {
            if(param1 == 0x01)  
            {
                printf("������\r\n");
                BAT_CHARGE_EN(1);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 1;          /* �ѳ��*/
                can_send_msg_data(0x17800100|(NodeId<<16), canbuf, 0);   /* ���س��״̬*/
            }
            else
            {
                printf("��ֹ���\r\n");
                BAT_CHARGE_EN(0);
                nodeData.AudioChargeKeyStatus.ChargeStatus = 0;           /* δ���*/
                can_send_msg_data(0x17800000|(NodeId<<16), canbuf, 0);    /* ���س��״̬*/
            }
			canbuf[5] = nodeData.AudioChargeKeyStatus.AudioStatus|(nodeData.AudioChargeKeyStatus.ChargeStatus<<1)|(nodeData.AudioChargeKeyStatus.Key1Key2Status<<2);
        }
		else if((Extcmd == 0x18)&&(destAddr == slaveAddr))                /*�ӽڵ���յ�"RS485ͨ�ŵ�ַ"������*/
		{
			/*����1��ͨ�ŵ�ַ������2��ͨ�Ų�����*/
			if(param2 == 0)
			{
				rs485ComAddr = 0;                                         /*�����ǰͨ�ŵ�ַ*/
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
				rs485Baud = 115200;                               /*Ĭ�ϲ�����*/
			}
			else if(param2 == 9)
			{
				rs485Baud = 460800;
			}
			can_send_msg_data(g_can1_rxheader.ExtId, canbuf, 0);
		}
		else if((Extcmd == 0x1E)&&(destAddr == slaveAddr))        /*�ӽڵ���յ�"ȥ��ʱ������"������*/
		{
			rcmDiId = param2>>4;                                  /* ���桰ȥ����DI�š�*/
			if(rcmDiId == 1)
			{
				debounceTimeSet[0] = param1|((param2&0x0F)<<8);   /* ���桰ȥ��ʱ�䡱*/
			}
			else if(rcmDiId == 2)
			{
				debounceTimeSet[1] = param1|((param2&0x0F)<<8);   /* ���桰ȥ��ʱ�䡱*/
			}
			else if(rcmDiId == 3)
			{
				debounceTimeSet[2] = param1|((param2&0x0F)<<8);   /* ���桰ȥ��ʱ�䡱*/
			}
			else if(rcmDiId == 4)
			{
				debounceTimeSet[3] = param1|((param2&0x0F)<<8);   /* ���桰ȥ��ʱ�䡱*/
			}
			else if(rcmDiId == 5)
			{
				debounceTimeSet[4] = param1|((param2&0x0F)<<8);   /* ���桰ȥ��ʱ�䡱*/
			}
			else if(rcmDiId == 6)
			{
				debounceTimeSet[5] = param1|((param2&0x0F)<<8);   /* ���桰ȥ��ʱ�䡱*/
			}
			else if(rcmDiId == 7)
			{
				debounceTimeSet[6] = param1|((param2&0x0F)<<8);   /* ���桰ȥ��ʱ�䡱*/
			}
			else if(rcmDiId == 8)
			{
				debounceTimeSet[7] = param1|((param2&0x0F)<<8);   /* ���桰ȥ��ʱ�䡱*/
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
		can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 8);         /*��DI�б仯���ϱ����� 8�ֽ�*/
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
	//CAN_EN(1);                                                       /* can��ֹ*/ 
}

uint8_t tmNodeDisposeFunc(NodeDataStruct nodeData)
{
    uint8_t canReceiBuf[8];
    uint8_t *recvbuf = canReceiBuf;
	uint8_t Extcmd;
    uint8_t param1;

	delay_us(500);
	
    CAN_EN(0); /* canʹ��*/
	canbuf[0] = nodeData.NodeType;                                 /*�ӽڵ�����*/
	canbuf[1] = nodeData.PowerVolt;  
    if(!can_receive_msg(g_can1_rxheader.ExtId,recvbuf))  
    {
		Extcmd = (g_can1_rxheader.ExtId >> 24)&0xFF;
        param1 = (uint16_t)g_can1_rxheader.ExtId >>8;
		destAddr = (g_can1_rxheader.ExtId>>16)&0x7F;
        if(Extcmd == 0x01)                                        /*�ӽڵ���յ�"���ýڵ���"������*/
        {
            NodeId = param1;                                      /*�ڵ�����ȡ��������NodeId */
			slaveAddr = NodeId;                                   /*�ӻ���ַ���ڽڵ���*/
			if(slaveAddr == 0x3E)
			{
				can_send_msg_data(0x11800400|(NodeId<<16), canbuf, 0);/*���ͽڵ����� 0x04*/
			}				
        }
		else
		{
			;
		}
    }                                                
    //CAN_EN(1);                                                   /*CAN��ֹ */
#if 0
    if(queryAflag == 0x01)
    {
		delay_ms(1);
		printf("����query�źţ��պϵ��ӿ��� QUERYAB_ON/OFF\r\n");
		MCU_CT_QUERYA(1);
		delay_ms(35);
		MCU_CT_QUERYA(0);
		queryAflag = 0x00;
		queryAflag_5ms = 0x00;
		//can_send_msg_data(0x02800000|(NodeId<<16), canbuf, 2);     /*�ϱ����� 2�ֽ�*/
    }
	TIM2->SR &= ~(0x02);                                           /*����жϱ�־*/
	HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* ����ITM2�ж� */
	HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);
	#endif
    return 0;
}

