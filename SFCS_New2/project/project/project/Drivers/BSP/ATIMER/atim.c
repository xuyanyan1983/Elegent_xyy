/**
 ****************************************************************************************************
* ������������
 ****************************************************************************************************
 */

#include "./BSP/ATIMER/atim.h"

TIM_HandleTypeDef g_tim1_ch1_npwm_chy_handle;     /* queryA�ķ������壬��ʱ��x��� */
TIM_HandleTypeDef g_tim1_ch2_npwm_chy_handle;     /* queryB�ķ������壬��ʱ��x��� */

/* g_npwm_remain��ʾ��ǰ��ʣ�¶��ٸ�����Ҫ����
 * ÿ����෢��256������
 */
static uint32_t g_npwm_remain = 0;

/**
 * @brief       �߼���ʱ��TIMX ͨ��Y ���ָ������PWM ��ʼ������
 * @note
 *              �߼���ʱ����ʱ������APB2, ��PCLK2 = 168Mhz, ��������PPRE2����Ƶ, ���
 *              �߼���ʱ��ʱ�� = 168Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ
 * @param       psc: Ԥ��Ƶϵ��
 * @retval      ��
 */
void atim_tim1_ch1_npwm_chy_init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef gpio_init_struct;
    TIM_OC_InitTypeDef timx_oc_npwm_chy = {0};  /* ��ʱ����� */
    ATIM_TIM1_CH1_NPWM_CHY_GPIO_CLK_ENABLE();       /* TIMX ͨ��IO��ʱ��ʹ�� */
    ATIM_TIM1_CH1_NPWM_CHY_CLK_ENABLE();            /* TIMX ʱ��ʹ�� */

    g_tim1_ch1_npwm_chy_handle.Instance = ATIM_TIM1_CH1_NPWM;                   /* ��ʱ��x */
    g_tim1_ch1_npwm_chy_handle.Init.Prescaler = psc;                        /* ��ʱ����Ƶ */
    g_tim1_ch1_npwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;       /* ��������ģʽ */
    g_tim1_ch1_npwm_chy_handle.Init.Period = arr;                           /* �Զ���װ��ֵ */
    g_tim1_ch1_npwm_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /* ʹ��TIMx_ARR���л��� */
    g_tim1_ch1_npwm_chy_handle.Init.RepetitionCounter = 0;                  /* �ظ���������ʼֵ */
    HAL_TIM_PWM_Init(&g_tim1_ch1_npwm_chy_handle);                          /* ��ʼ��PWM */

	#if 1
    gpio_init_struct.Pin = ATIM_TIM1_CH1_NPWM_CHY_GPIO_PIN;                 /* ͨ��y��GPIO�� */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                            /* ����������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* ���� */
    gpio_init_struct.Alternate = ATIM_TIM1_CH1_NPWM_CHY_GPIO_AF;            /* �˿ڸ��� */
    HAL_GPIO_Init(ATIM_TIM1_CH1_NPWM_CHY_GPIO_PORT, &gpio_init_struct);
	#endif

    timx_oc_npwm_chy.OCMode = TIM_OCMODE_PWM1;                          /* ģʽѡ��PWM 1 */
    timx_oc_npwm_chy.Pulse = arr / 2;                                   /* ���ñȽ�ֵ,��ֵ����ȷ��ռ�ձ� */
                                                                        /* ����Ĭ�����ñȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50% */
    timx_oc_npwm_chy.OCPolarity = TIM_OCPOLARITY_LOW;                  /* ����Ƚϼ���Ϊ�� */
    HAL_TIM_PWM_ConfigChannel(&g_tim1_ch1_npwm_chy_handle, &timx_oc_npwm_chy, ATIM_TIM1_CH1_NPWM_CHY); /* ����TIMxͨ��y */

    //HAL_NVIC_SetPriority(ATIM_TIM1_CH1_NPWM_IRQn, 1, 3);                    /* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */
    //HAL_NVIC_EnableIRQ(ATIM_TIM1_CH1_NPWM_IRQn);                            /* ����ITMx�ж� */

    __HAL_TIM_ENABLE_IT(&g_tim1_ch1_npwm_chy_handle, TIM_IT_UPDATE);        /* ��������ж� */
    HAL_TIM_PWM_Start(&g_tim1_ch1_npwm_chy_handle, ATIM_TIM1_CH1_NPWM_CHY);     /* ������ӦPWMͨ�� */
}


void atim_tim1_ch2_npwm_chy_init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef gpio_init_struct;
    TIM_OC_InitTypeDef timx_oc_npwm_chy = {0};  /* ��ʱ����� */
    ATIM_TIM1_CH2_NPWM_CHY_GPIO_CLK_ENABLE();       /* TIMX ͨ��IO��ʱ��ʹ�� */
    ATIM_TIM1_CH2_NPWM_CHY_CLK_ENABLE();            /* TIMX ʱ��ʹ�� */

    g_tim1_ch2_npwm_chy_handle.Instance = ATIM_TIM1_CH2_NPWM;                   /* ��ʱ��x */
    g_tim1_ch2_npwm_chy_handle.Init.Prescaler = psc;                        /* ��ʱ����Ƶ */
    g_tim1_ch2_npwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;       /* ��������ģʽ */
    g_tim1_ch2_npwm_chy_handle.Init.Period = arr;                           /* �Զ���װ��ֵ */
    g_tim1_ch2_npwm_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /* ʹ��TIMx_ARR���л��� */
    g_tim1_ch2_npwm_chy_handle.Init.RepetitionCounter = 0;                  /* �ظ���������ʼֵ */
    HAL_TIM_PWM_Init(&g_tim1_ch2_npwm_chy_handle);                          /* ��ʼ��PWM */

    gpio_init_struct.Pin = ATIM_TIM1_CH2_NPWM_CHY_GPIO_PIN;                 /* ͨ��y��GPIO�� */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                            /* ����������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* ���� */
    gpio_init_struct.Alternate = ATIM_TIM1_CH2_NPWM_CHY_GPIO_AF;            /* �˿ڸ��� */
    HAL_GPIO_Init(ATIM_TIM1_CH2_NPWM_CHY_GPIO_PORT, &gpio_init_struct);

    timx_oc_npwm_chy.OCMode = TIM_OCMODE_PWM1;                          /* ģʽѡ��PWM 1 */
    timx_oc_npwm_chy.Pulse = arr / 2;                                   /* ���ñȽ�ֵ,��ֵ����ȷ��ռ�ձ� */
                                                                        /* ����Ĭ�����ñȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50% */
    timx_oc_npwm_chy.OCPolarity = TIM_OCPOLARITY_LOW;                  /* ����Ƚϼ���Ϊ�� */
    HAL_TIM_PWM_ConfigChannel(&g_tim1_ch2_npwm_chy_handle, &timx_oc_npwm_chy, ATIM_TIM1_CH2_NPWM_CHY); /* ����TIMxͨ��y */

    HAL_NVIC_SetPriority(ATIM_TIM1_CH2_NPWM_IRQn, 1, 3);                    /* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */
    HAL_NVIC_EnableIRQ(ATIM_TIM1_CH2_NPWM_IRQn);                            /* ����ITMx�ж� */

    __HAL_TIM_ENABLE_IT(&g_tim1_ch2_npwm_chy_handle, TIM_IT_UPDATE);        /* ��������ж� */
    HAL_TIM_PWM_Start(&g_tim1_ch2_npwm_chy_handle, ATIM_TIM1_CH2_NPWM_CHY);     /* ������ӦPWMͨ�� */
}

/**
 * @brief       �߼���ʱ��TIMX NPWM����PWM����
 * @param       rcr: PWM�ĸ���, 1~2^32�η���
 * @retval      ��
 */
void atim_tim1_ch1_npwm_chy_set(uint32_t npwm)
{
    if (npwm == 0) return;

    g_npwm_remain = npwm;                                                   /* ����������� */
    HAL_TIM_GenerateEvent(&g_tim1_ch1_npwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* ����һ�θ����¼�,���ж����洦��������� */
    __HAL_TIM_ENABLE(&g_tim1_ch1_npwm_chy_handle);                              /* ʹ�ܶ�ʱ��TIMX */
}

void atim_tim1_ch2_npwm_chy_set(uint32_t npwm)
{
    if (npwm == 0) return;

    g_npwm_remain = npwm;                                                   /* ����������� */
    HAL_TIM_GenerateEvent(&g_tim1_ch2_npwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* ����һ�θ����¼�,���ж����洦��������� */
    __HAL_TIM_ENABLE(&g_tim1_ch2_npwm_chy_handle);                              /* ʹ�ܶ�ʱ��TIMX */
}

/**
 * @brief       �߼���ʱ��TIMX NPWM�жϷ�����
 * @param       ��
 * @retval      ��
 */
void TIM1_UP_TIM16_IRQHandler(void)
{
    uint16_t npwm = 0;
    
    /* ���´���û��ʹ�ö�ʱ��HAL�⹲�ô���������������ֱ��ͨ���ж��жϱ�־λ�ķ�ʽ */
    if (__HAL_TIM_GET_FLAG(&g_tim1_ch1_npwm_chy_handle, TIM_FLAG_UPDATE) != RESET)
    {
        if (g_npwm_remain >= 256)            /* ���д���256��������Ҫ���� */
        {
            g_npwm_remain = g_npwm_remain - 256;
            npwm = 256;
        }
        else if (g_npwm_remain % 256)       /* ����λ��������256��������Ҫ���� */
        {
            npwm = g_npwm_remain % 256;
            g_npwm_remain = 0;              /* û�������� */
        }

        if (npwm) /* ������Ҫ���� */
        {
            ATIM_TIM1_CH1_NPWM->RCR = npwm - 1;                                         /* �����ظ������Ĵ���ֵΪnpwm-1, ��npwm������ */
            HAL_TIM_GenerateEvent(&g_tim1_ch1_npwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* ����һ�θ����¼�,���ж����洦��������� */
            __HAL_TIM_ENABLE(&g_tim1_ch1_npwm_chy_handle);                              /* ʹ�ܶ�ʱ��TIMX */
        }
        else
        {
            ATIM_TIM1_CH1_NPWM->CR1 &= ~(1 << 0); /* �رն�ʱ��TIMX��ʹ��HAL Disable�����PWMͨ����Ϣ���˴����� */
        }

        __HAL_TIM_CLEAR_IT(&g_tim1_ch1_npwm_chy_handle, TIM_IT_UPDATE);  /* �����ʱ������жϱ�־λ */
    }
	
	   /* ���´���û��ʹ�ö�ʱ��HAL�⹲�ô���������������ֱ��ͨ���ж��жϱ�־λ�ķ�ʽ */
    if (__HAL_TIM_GET_FLAG(&g_tim1_ch2_npwm_chy_handle, TIM_FLAG_UPDATE) != RESET)
    {
        if (g_npwm_remain >= 256)            /* ���д���256��������Ҫ���� */
        {
            g_npwm_remain = g_npwm_remain - 256;
            npwm = 256;
        }
        else if (g_npwm_remain % 256)       /* ����λ��������256��������Ҫ���� */
        {
            npwm = g_npwm_remain % 256;
            g_npwm_remain = 0;              /* û�������� */
        }

        if (npwm) /* ������Ҫ���� */
        {
            ATIM_TIM1_CH2_NPWM->RCR = npwm - 1;                                         /* �����ظ������Ĵ���ֵΪnpwm-1, ��npwm������ */
            HAL_TIM_GenerateEvent(&g_tim1_ch2_npwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* ����һ�θ����¼�,���ж����洦��������� */
            __HAL_TIM_ENABLE(&g_tim1_ch2_npwm_chy_handle);                              /* ʹ�ܶ�ʱ��TIMX */
        }
        else
        {
            ATIM_TIM1_CH2_NPWM->CR1 &= ~(1 << 0); /* �رն�ʱ��TIMX��ʹ��HAL Disable�����PWMͨ����Ϣ���˴����� */
        }

        __HAL_TIM_CLEAR_IT(&g_tim1_ch2_npwm_chy_handle, TIM_IT_UPDATE);  /* �����ʱ������жϱ�־λ */
    }
}





