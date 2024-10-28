#include "./BSP/QUERYTIMER/queryabcap.h"
#include "./SYSTEM/delay/delay.h"
/*********************************������ͨ�ö�ʱ�����벶��ʵ�����*************************************/
TIM_HandleTypeDef g_tim2_queryA_cap_handle;   /* ��ʱ��x��� */
TIM_HandleTypeDef g_tim2_queryB_cap_handle;   /* ��ʱ��x��� */
/**
 * @brief       ͨ�ö�ʱ��TIMX ͨ��Y ���벶�� ��ʼ������
 * @note
 *              ͨ�ö�ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ͨ�ö�ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ42M, ���Զ�ʱ��ʱ�� = 84Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ
 * @param       psc: Ԥ��Ƶϵ��
 * @retval      ��
 */
 /* ���벶��״̬(g_timxchy_cap_sta)
 * [7]  :0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
 * [6]  :0,��û���񵽸ߵ�ƽ;1,�Ѿ����񵽸ߵ�ƽ��.
 * [5:0]:����ߵ�ƽ������Ĵ���,������63��,���������ֵ = 63*65536 + 65535 = 4194303
 *       ע��:Ϊ��ͨ��,����Ĭ��ARR��CCRy����16λ�Ĵ���,����32λ�Ķ�ʱ��(��:TIM5),Ҳֻ��16λʹ��
 *       ��1us�ļ���Ƶ��,����ʱ��Ϊ:4194303 us, Լ4.19��
 *
 *      (˵��һ�£�����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
 */
uint8_t g_tim2_queryA_cap_sta = 0;    /* ���벶��״̬ */
uint16_t g_tim2_queryA_cap_val = 0;   /* ���벶��ֵ */

uint8_t g_tim2_queryB_cap_sta = 0;    /* ���벶��״̬ */
uint16_t g_tim2_queryB_cap_val = 0;   /* ���벶��ֵ */

void queryA_gtim_timx_cap_chy_init(uint32_t arr, uint16_t psc)
{
	HAL_TIM_IC_DeInit(&g_tim2_queryA_cap_handle);
    TIM_IC_InitTypeDef timx_ic_cap_chy = {0};

    g_tim2_queryA_cap_handle.Instance = QUERYA_GTIM_TIMX_CAP;         /* ��ʱ��2 */
    g_tim2_queryA_cap_handle.Init.Prescaler = psc;                    /* Ԥ��Ƶϵ�� */
    g_tim2_queryA_cap_handle.Init.CounterMode = TIM_COUNTERMODE_UP;   /* ���ϼ���ģʽ */
    g_tim2_queryA_cap_handle.Init.Period = arr;                       /* �Զ���װ��ֵ */
	g_tim2_queryA_cap_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
    HAL_TIM_IC_Init(&g_tim2_queryA_cap_handle);                       /* ��ʼ����ʱ�� */
    
    timx_ic_cap_chy.ICPolarity = TIM_ICPOLARITY_FALLING;                   /* �����ز��� */
    timx_ic_cap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI; /* ӳ�䵽TI1�� */
    timx_ic_cap_chy.ICPrescaler = TIM_ICPSC_DIV1;           /* ���������Ƶ������Ƶ */
    timx_ic_cap_chy.ICFilter = 0;                           /* ���������˲��������˲� */
    HAL_TIM_IC_ConfigChannel(&g_tim2_queryA_cap_handle, &timx_ic_cap_chy, QUERYA_GTIM_TIMX_CAP_CHY); /* ����TIM2ͨ��1 */

	__HAL_TIM_ENABLE_IT(&g_tim2_queryA_cap_handle, TIM_IT_UPDATE);     /* ʹ�ܸ����ж� */
    HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);     /* ��ʼ����TIM2��ͨ��1 */
}


/******queryB���@query��̖*******/
void queryB_gtim_timx_cap_chy_init(uint32_t arr, uint16_t psc)
{
	HAL_TIM_IC_DeInit(&g_tim2_queryB_cap_handle);
    TIM_IC_InitTypeDef timx_ic_cap_chy = {0};
	
    g_tim2_queryB_cap_handle.Instance = QUERYB_GTIM_TIMX_CAP;         /* ��ʱ��2 */
    g_tim2_queryB_cap_handle.Init.Prescaler = psc;                    /* Ԥ��Ƶϵ�� */
    g_tim2_queryB_cap_handle.Init.CounterMode = TIM_COUNTERMODE_UP;   /* ���ϼ���ģʽ */
    g_tim2_queryB_cap_handle.Init.Period = arr;                       /* �Զ���װ��ֵ */
	g_tim2_queryB_cap_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_2;
    HAL_TIM_IC_Init(&g_tim2_queryB_cap_handle);                       /* ��ʼ����ʱ�� */
    
    timx_ic_cap_chy.ICPolarity = TIM_ICPOLARITY_FALLING;     /* �����ز��� */
    timx_ic_cap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI; /* ӳ�䵽TI1�� */
    timx_ic_cap_chy.ICPrescaler = TIM_ICPSC_DIV1;           /* ���������Ƶ������Ƶ */
    timx_ic_cap_chy.ICFilter = 0;                           /* ���������˲��������˲� */
    HAL_TIM_IC_ConfigChannel(&g_tim2_queryB_cap_handle, &timx_ic_cap_chy, QUERYB_GTIM_TIMX_CAP_CHY); /* ����TIM2ͨ��1 */

    __HAL_TIM_ENABLE_IT(&g_tim2_queryB_cap_handle, TIM_IT_UPDATE);         /* ʹ�ܸ����ж� */
    HAL_TIM_IC_Start_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);     /* ��ʼ����TIM2��ͨ��1 */
}

/**
 * @brief       ͨ�ö�ʱ�����벶���ʼ���ӿ�
 *              HAL����õĽӿڣ��������ò�ͬ�����벶��   
 * @param       htim:��ʱ�����
 * @note        �˺����ᱻHAL_TIM_IC_Init()����
 * @retval      ��
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if ((htim->Instance == QUERYA_GTIM_TIMX_CAP) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1))                        /* ����ͨ������ */
    {
        GPIO_InitTypeDef gpio_init_struct;
        QUERYA_GTIM_TIMX_CAP_CHY_CLK_ENABLE();                         /* ʹ��TIMxʱ�� */
        QUERYA_GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE();                    /* ��������IO��ʱ�� */

        gpio_init_struct.Pin = QUERYA_GTIM_TIMX_CAP_CHY_GPIO_PIN;      /* ���벶���GPIO�� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                       /* ����������� */
        gpio_init_struct.Pull = GPIO_NOPULL;                           /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;            /* ���� */
        gpio_init_struct.Alternate = QUERYA_GTIM_TIMX_CAP_CHY_GPIO_AF; /* ����Ϊ����TIM5��ͨ��1 */
        HAL_GPIO_Init(QUERYA_GTIM_TIMX_CAP_CHY_GPIO_PORT, &gpio_init_struct);

        HAL_NVIC_SetPriority(QUERYA_GTIM_TIMX_CAP_IRQn, 1, 3);         /* ��ռ1�������ȼ�3 */
        HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* ����ITM2�ж� */
    }
	
	if((htim->Instance == QUERYB_GTIM_TIMX_CAP) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2))                       /* ����ͨ������ */
	{
		GPIO_InitTypeDef gpio_init_struct;
		QUERYB_GTIM_TIMX_CAP_CHY_CLK_ENABLE();                         /* ʹ��TIMxʱ�� */
        QUERYB_GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE();                    /* ��������IO��ʱ�� */
		
		gpio_init_struct.Pin = QUERYB_GTIM_TIMX_CAP_CHY_GPIO_PIN;      /* ���벶���GPIO�� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                       /* ����������� */
        gpio_init_struct.Pull = GPIO_NOPULL;                           /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;            /* ���� */
        gpio_init_struct.Alternate = QUERYB_GTIM_TIMX_CAP_CHY_GPIO_AF; /* ����Ϊ����TIM5��ͨ��1 */
        HAL_GPIO_Init(QUERYB_GTIM_TIMX_CAP_CHY_GPIO_PORT, &gpio_init_struct);

        HAL_NVIC_SetPriority(QUERYB_GTIM_TIMX_CAP_IRQn, 1, 3);         /* ��ռ1�������ȼ�3 */
        HAL_NVIC_EnableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);                 /* ����ITM2�ж� */
	}
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&g_tim2_queryA_cap_handle);        /* ��ʱ�����ô����� */
	__HAL_TIM_CLEAR_FLAG(&g_tim2_queryA_cap_handle, TIM_FLAG_UPDATE|TIM_FLAG_CC1);
	HAL_TIM_IRQHandler(&g_tim2_queryB_cap_handle);        /* ��ʱ�����ô����� */
	__HAL_TIM_CLEAR_FLAG(&g_tim2_queryB_cap_handle, TIM_FLAG_UPDATE|TIM_FLAG_CC2);
}

/**
 * @brief       ��ʱ�����벶���жϴ���ص�����
 * @param       htim:��ʱ�����ָ��
 * @note        �ú�����HAL_TIM_IRQHandler�лᱻ����
 * @retval      ��
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if ((g_tim2_queryA_cap_sta & 0x80) == 0)    /* ��δ�ɹ����� */
    {
        if (g_tim2_queryA_cap_sta & 0x40)       /* ����һ���½��� */
        {
            g_tim2_queryA_cap_sta |= 0x80;      /* ��ǳɹ�����һ�θߵ�ƽ���� */
            g_tim2_queryA_cap_val = HAL_TIM_ReadCapturedValue(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);  /* ��ȡ��ǰ�Ĳ���ֵ */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);                      /* һ��Ҫ�����ԭ�������� */
			TIM_SET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING); /* ��ʱ��5ͨ��1����Ϊ�½��ز��� */
        }
        else                                            /* ��δ��ʼ,��һ�β��������� */
        {
            g_tim2_queryA_cap_sta = 0;                      /* ��� */
            g_tim2_queryA_cap_val = 0;
            g_tim2_queryA_cap_sta |= 0x40;                  /* ��ǲ����������� */
            __HAL_TIM_SET_COUNTER(&g_tim2_queryA_cap_handle, 0);  /* ��ʱ��5���������� */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);                       /* һ��Ҫ�����ԭ�������ã��� */
			TIM_SET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING); /* ����TIM5ͨ��1�����ز��� */
        }
    }

	if ((g_tim2_queryB_cap_sta & 0x80) == 0)    /* ��δ�ɹ����� */
    {
        if (g_tim2_queryB_cap_sta & 0x40)       /* ����һ���½��� */
        {
            g_tim2_queryB_cap_sta |= 0x80;      /* ��ǳɹ�����һ�θߵ�ƽ���� */
            g_tim2_queryB_cap_val = HAL_TIM_ReadCapturedValue(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);  /* ��ȡ��ǰ�Ĳ���ֵ */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);                      /* һ��Ҫ�����ԭ�������� */
            TIM_SET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING); /* ����TIM5ͨ��1�����ز��� */
        }
        else                                            /* ��δ��ʼ,��һ�β��������� */
        {
            g_tim2_queryB_cap_sta = 0;                      /* ��� */
            g_tim2_queryB_cap_val = 0;
            g_tim2_queryB_cap_sta |= 0x40;                  /* ��ǲ����������� */
            __HAL_TIM_SET_COUNTER(&g_tim2_queryB_cap_handle, 0);  /* ��ʱ��5���������� */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);                       /* һ��Ҫ�����ԭ�������ã��� */
            TIM_SET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING); /* ��ʱ��5ͨ��1����Ϊ�½��ز��� */
        }
    }
}

/**
 * @brief       ��ʱ�������жϻص�����
 * @param       htim : ��ʱ�����ָ��
 * @note        �˺����ᱻ��ʱ���жϺ�����ͬ���õ�
 * @retval      ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if((htim->Instance == QUERYA_GTIM_TIMX_CAP) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1))
    {
        if ((g_tim2_queryA_cap_sta & 0x80) == 0)                /* ��û�ɹ����� */
        {
            if (g_tim2_queryA_cap_sta & 0x40)                   /* �Ѿ����񵽸ߵ�ƽ�� */
            {
                if ((g_tim2_queryA_cap_sta & 0x3F) == 0x3F)     /* �ߵ�ƽ̫���� */
                {
                    TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);                     /* һ��Ҫ�����ԭ�������� */
                    TIM_SET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);/* ����TIM5ͨ��1�����ز��� */
                    g_tim2_queryA_cap_sta |= 0x80;              /* ��ǳɹ�������һ�� */
                    g_tim2_queryA_cap_val = 0xFFFF;
                }
                else  /* �ۼƶ�ʱ��������� */
                {
                    g_tim2_queryA_cap_sta++;
                }
            }
        }
    }
	if((htim->Instance == QUERYB_GTIM_TIMX_CAP) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2))
    {
        if ((g_tim2_queryB_cap_sta & 0x80) == 0)                /* ��û�ɹ����� */
        {
            if (g_tim2_queryB_cap_sta & 0x40)                   /* �Ѿ����񵽸ߵ�ƽ�� */
            {
                if ((g_tim2_queryB_cap_sta & 0x3F) == 0x3F)     /* �ߵ�ƽ̫���� */
                {
                    TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);                     /* һ��Ҫ�����ԭ�������� */
                    TIM_SET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);/* ����TIM5ͨ��1�����ز��� */
					//TIM_SET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING);/* ����TIM5ͨ��1�����ز��� */
                    g_tim2_queryB_cap_sta |= 0x80;              /* ��ǳɹ�������һ�� */
                    g_tim2_queryB_cap_val = 0xFFFF;
                }
                else  /* �ۼƶ�ʱ��������� */
                {
                    g_tim2_queryB_cap_sta++;
                }
            }
        }
    }
}



