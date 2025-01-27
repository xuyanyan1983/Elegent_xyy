#include "./BSP/QUERYTIMER/queryabcap.h"
#include "./SYSTEM/delay/delay.h"
/*********************************以下是通用定时器输入捕获实验程序*************************************/
TIM_HandleTypeDef g_tim2_queryA_cap_handle;   /* 定时器x句柄 */
TIM_HandleTypeDef g_tim2_queryB_cap_handle;   /* 定时器x句柄 */
TIM_HandleTypeDef g_tim2_queryA_cap_tm_handle;   /* 定时器x句柄 */
/**
 * @brief       通用定时器TIMX 通道Y 输入捕获 初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为42M, 所以定时器时钟 = 84Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值
 * @param       psc: 预分频系数
 * @retval      无
 */
 /* 输入捕获状态(g_timxchy_cap_sta)
 * [7]  :0,没有成功的捕获;1,成功捕获到一次.
 * [6]  :0,还没捕获到高电平;1,已经捕获到高电平了.
 * [5:0]:捕获高电平后溢出的次数,最多溢出63次,所以最长捕获值 = 63*65536 + 65535 = 4194303
 *       注意:为了通用,我们默认ARR和CCRy都是16位寄存器,对于32位的定时器(如:TIM5),也只按16位使用
 *       按1us的计数频率,最长溢出时间为:4194303 us, 约4.19秒
 *
 *      (说明一下：正常32位定时器来说,1us计数器加1,溢出时间:4294秒)
 */
uint8_t g_tim2_queryA_cap_sta = 0;    /* 输入捕获状态 */
uint16_t g_tim2_queryA_cap_val = 0;   /* 输入捕获值 */

uint8_t g_tim2_queryB_cap_sta = 0;    /* 输入捕获状态 */
uint16_t g_tim2_queryB_cap_val = 0;   /* 输入捕获值 */

uint8_t g_tim2_queryA_cap_tm_sta = 0;       /* 输入捕获状态 */
uint16_t g_tim2_queryA_cap_tm_val = 0;      /* 输入捕获值 */

/*query_inA捕获query的初始化函数*/
void queryA_gtim_timx_cap_chy_init(uint32_t arr, uint16_t psc)
{
	HAL_TIM_IC_DeInit(&g_tim2_queryA_cap_handle);
    TIM_IC_InitTypeDef timx_ic_cap_chy = {0};

    g_tim2_queryA_cap_handle.Instance = QUERYA_GTIM_TIMX_CAP;         /* 定时器2 */
    g_tim2_queryA_cap_handle.Init.Prescaler = psc;                    /* 预分频系数 */
    g_tim2_queryA_cap_handle.Init.CounterMode = TIM_COUNTERMODE_UP;   /* 向上计数模式 */
    g_tim2_queryA_cap_handle.Init.Period = arr;                       /* 自动重装载值 */
	g_tim2_queryA_cap_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
    HAL_TIM_IC_Init(&g_tim2_queryA_cap_handle);                       /* 初始化定时器 */
    
    timx_ic_cap_chy.ICPolarity = TIM_ICPOLARITY_FALLING;                   /* 上升沿捕获 */
    timx_ic_cap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI; /* 映射到TI1上 */
    timx_ic_cap_chy.ICPrescaler = TIM_ICPSC_DIV1;           /* 配置输入分频，不分频 */
    timx_ic_cap_chy.ICFilter = 0;                           /* 配置输入滤波器，不滤波 */
    HAL_TIM_IC_ConfigChannel(&g_tim2_queryA_cap_handle, &timx_ic_cap_chy, QUERYA_GTIM_TIMX_CAP_CHY); /* 配置TIM2通道1 */

	__HAL_TIM_ENABLE_IT(&g_tim2_queryA_cap_handle, TIM_IT_UPDATE);     /* 使能更新中断 */
    HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);     /* 开始捕获TIM2的通道1 */
}

/*query_inA捕获tm返回的35ms的初始化函数*/
void queryA_gtim_timx_cap_tm_init(uint32_t arr, uint16_t psc)
{
	HAL_TIM_IC_DeInit(&g_tim2_queryA_cap_tm_handle);
    TIM_IC_InitTypeDef timx_ic_cap_chy = {0};

    g_tim2_queryA_cap_tm_handle.Instance = QUERYA_GTIM_TIMX_CAP;         /* 定时器2 */
    g_tim2_queryA_cap_tm_handle.Init.Prescaler = psc;                    /* 预分频系数 */
    g_tim2_queryA_cap_tm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;   /* 向上计数模式 */
    g_tim2_queryA_cap_tm_handle.Init.Period = arr;                       /* 自动重装载值 */
	g_tim2_queryA_cap_tm_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
    HAL_TIM_IC_Init(&g_tim2_queryA_cap_tm_handle);                       /* 初始化定时器 */
    
    timx_ic_cap_chy.ICPolarity = TIM_ICPOLARITY_FALLING;                   /* 上升沿捕获 */
    timx_ic_cap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI; /* 映射到TI1上 */
    timx_ic_cap_chy.ICPrescaler = TIM_ICPSC_DIV1;           /* 配置输入分频，不分频 */
    timx_ic_cap_chy.ICFilter = 0;                           /* 配置输入滤波器，不滤波 */
    HAL_TIM_IC_ConfigChannel(&g_tim2_queryA_cap_tm_handle, &timx_ic_cap_chy, QUERYA_GTIM_TIMX_CAP_CHY); /* 配置TIM2通道1 */

	//__HAL_TIM_ENABLE_IT(&g_tim2_queryA_cap_tm_handle, TIM_IT_UPDATE);     /* 使能更新中断 */
    //HAL_TIM_IC_Start_IT(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);     /* 开始捕获TIM2的通道1 */
}

/*query_inB捕获query的初始化函数*/
void queryB_gtim_timx_cap_chy_init(uint32_t arr, uint16_t psc)
{
	HAL_TIM_IC_DeInit(&g_tim2_queryB_cap_handle);
    TIM_IC_InitTypeDef timx_ic_cap_chy = {0};
	
    g_tim2_queryB_cap_handle.Instance = QUERYB_GTIM_TIMX_CAP;         /* 定时器2 */
    g_tim2_queryB_cap_handle.Init.Prescaler = psc;                    /* 预分频系数 */
    g_tim2_queryB_cap_handle.Init.CounterMode = TIM_COUNTERMODE_UP;   /* 向上计数模式 */
    g_tim2_queryB_cap_handle.Init.Period = arr;                       /* 自动重装载值 */
	g_tim2_queryB_cap_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_2;
    HAL_TIM_IC_Init(&g_tim2_queryB_cap_handle);                       /* 初始化定时器 */
    
    timx_ic_cap_chy.ICPolarity = TIM_ICPOLARITY_FALLING;     /* 上升沿捕获 */
    timx_ic_cap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI; /* 映射到TI1上 */
    timx_ic_cap_chy.ICPrescaler = TIM_ICPSC_DIV1;           /* 配置输入分频，不分频 */
    timx_ic_cap_chy.ICFilter = 0;                           /* 配置输入滤波器，不滤波 */
    HAL_TIM_IC_ConfigChannel(&g_tim2_queryB_cap_handle, &timx_ic_cap_chy, QUERYB_GTIM_TIMX_CAP_CHY); /* 配置TIM2通道1 */

    __HAL_TIM_ENABLE_IT(&g_tim2_queryB_cap_handle, TIM_IT_UPDATE);         /* 使能更新中断 */
    HAL_TIM_IC_Start_IT(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);     /* 开始捕获TIM2的通道1 */
	
}

/**
 * @brief       通用定时器输入捕获初始化接口
 *              HAL库调用的接口，用于配置不同的输入捕获   
 * @param       htim:定时器句柄
 * @note        此函数会被HAL_TIM_IC_Init()调用
 * @retval      无
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if ((htim->Instance == QUERYA_GTIM_TIMX_CAP) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1))                        /* 输入通道捕获 */
    {
        GPIO_InitTypeDef gpio_init_struct;
        QUERYA_GTIM_TIMX_CAP_CHY_CLK_ENABLE();                         /* 使能TIMx时钟 */
        QUERYA_GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE();                    /* 开启捕获IO的时钟 */

        gpio_init_struct.Pin = QUERYA_GTIM_TIMX_CAP_CHY_GPIO_PIN;      /* 输入捕获的GPIO口 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                       /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_NOPULL;                           /* 下拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;            /* 高速 */
        gpio_init_struct.Alternate = QUERYA_GTIM_TIMX_CAP_CHY_GPIO_AF; /* 复用为捕获TIM5的通道1 */
        HAL_GPIO_Init(QUERYA_GTIM_TIMX_CAP_CHY_GPIO_PORT, &gpio_init_struct);

        HAL_NVIC_SetPriority(QUERYA_GTIM_TIMX_CAP_IRQn, 1, 3);         /* 抢占1，子优先级3 */
        HAL_NVIC_EnableIRQ(QUERYA_GTIM_TIMX_CAP_IRQn);                 /* 开启ITM2中断 */
    }
	
	if((htim->Instance == QUERYB_GTIM_TIMX_CAP) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2))                       /* 输入通道捕获 */
	{
		GPIO_InitTypeDef gpio_init_struct;
		QUERYB_GTIM_TIMX_CAP_CHY_CLK_ENABLE();                         /* 使能TIMx时钟 */
        QUERYB_GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE();                    /* 开启捕获IO的时钟 */
		
		gpio_init_struct.Pin = QUERYB_GTIM_TIMX_CAP_CHY_GPIO_PIN;      /* 输入捕获的GPIO口 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                       /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_NOPULL;                           /* 下拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;            /* 高速 */
        gpio_init_struct.Alternate = QUERYB_GTIM_TIMX_CAP_CHY_GPIO_AF; /* 复用为捕获TIM5的通道1 */
        HAL_GPIO_Init(QUERYB_GTIM_TIMX_CAP_CHY_GPIO_PORT, &gpio_init_struct);

        HAL_NVIC_SetPriority(QUERYB_GTIM_TIMX_CAP_IRQn, 1, 3);         /* 抢占1，子优先级3 */
        HAL_NVIC_EnableIRQ(QUERYB_GTIM_TIMX_CAP_IRQn);                 /* 开启ITM2中断 */
		
	}
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&g_tim2_queryA_cap_handle);        /* 定时器共用处理函数 */
	__HAL_TIM_CLEAR_FLAG(&g_tim2_queryA_cap_handle, TIM_FLAG_UPDATE|TIM_FLAG_CC1);
	HAL_TIM_IRQHandler(&g_tim2_queryA_cap_tm_handle);        /* 定时器共用处理函数 */
	__HAL_TIM_CLEAR_FLAG(&g_tim2_queryA_cap_tm_handle, TIM_FLAG_UPDATE|TIM_FLAG_CC1);
	HAL_TIM_IRQHandler(&g_tim2_queryB_cap_handle);        /* 定时器共用处理函数 */
	__HAL_TIM_CLEAR_FLAG(&g_tim2_queryB_cap_handle, TIM_FLAG_UPDATE|TIM_FLAG_CC2);
}

/**
 * @brief       定时器输入捕获中断处理回调函数
 * @param       htim:定时器句柄指针
 * @note        该函数在HAL_TIM_IRQHandler中会被调用
 * @retval      无
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if ((g_tim2_queryA_cap_sta & 0x80) == 0)    /* 还未成功捕获 */
    {
        if (g_tim2_queryA_cap_sta & 0x40)       /* 捕获到一个下降沿 */
        {
            g_tim2_queryA_cap_sta |= 0x80;      /* 标记成功捕获到一次高电平脉宽 */
            g_tim2_queryA_cap_val = HAL_TIM_ReadCapturedValue(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);  /* 获取当前的捕获值 */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);                      /* 一定要先清除原来的设置 */
			TIM_SET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING); /* 定时器5通道1设置为下降沿捕获 */
        }
        else                                            /* 还未开始,第一次捕获上升沿 */
        {
            g_tim2_queryA_cap_sta = 0;                      /* 清空 */
            g_tim2_queryA_cap_val = 0;
            g_tim2_queryA_cap_sta |= 0x40;                  /* 标记捕获到了上升沿 */
            __HAL_TIM_SET_COUNTER(&g_tim2_queryA_cap_handle, 0);  /* 定时器5计数器清零 */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);                       /* 一定要先清除原来的设置！！ */
			TIM_SET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING); /* 配置TIM5通道1上升沿捕获 */
        }
    }

    if ((g_tim2_queryA_cap_tm_sta & 0x80) == 0)    /* 还未成功捕获 */
    {
        if (g_tim2_queryA_cap_tm_sta & 0x40)       /* 捕获到一个下降沿 */
        {
            g_tim2_queryA_cap_tm_sta |= 0x80;      /* 标记成功捕获到一次高电平脉宽 */
            g_tim2_queryA_cap_tm_val = HAL_TIM_ReadCapturedValue(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);  /* 获取当前的捕获值 */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);                      /* 一定要先清除原来的设置 */
			TIM_SET_CAPTUREPOLARITY(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING); /* 定时器5通道1设置为下降沿捕获 */
        }
        else                                            /* 还未开始,第一次捕获上升沿 */
        {
            g_tim2_queryA_cap_tm_sta = 0;                      /* 清空 */
            g_tim2_queryA_cap_tm_val = 0;
            g_tim2_queryA_cap_tm_sta |= 0x40;                  /* 标记捕获到了上升沿 */
            __HAL_TIM_SET_COUNTER(&g_tim2_queryA_cap_tm_handle, 0);  /* 定时器5计数器清零 */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);                       /* 一定要先清除原来的设置！！ */
			TIM_SET_CAPTUREPOLARITY(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING); /* 配置TIM5通道1上升沿捕获 */
        }
    }	
	
	if ((g_tim2_queryB_cap_sta & 0x80) == 0)    /* 还未成功捕获 */
    {
        if (g_tim2_queryB_cap_sta & 0x40)       /* 捕获到一个下降沿 */
        {
            g_tim2_queryB_cap_sta |= 0x80;      /* 标记成功捕获到一次高电平脉宽 */
            g_tim2_queryB_cap_val = HAL_TIM_ReadCapturedValue(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);  /* 获取当前的捕获值 */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);                      /* 一定要先清除原来的设置 */
            TIM_SET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING); /* 配置TIM5通道1上升沿捕获 */
        }
        else                                            /* 还未开始,第一次捕获上升沿 */
        {
            g_tim2_queryB_cap_sta = 0;                      /* 清空 */
            g_tim2_queryB_cap_val = 0;
            g_tim2_queryB_cap_sta |= 0x40;                  /* 标记捕获到了上升沿 */
            __HAL_TIM_SET_COUNTER(&g_tim2_queryB_cap_handle, 0);  /* 定时器5计数器清零 */
            TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);                       /* 一定要先清除原来的设置！！ */
            TIM_SET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING); /* 定时器5通道1设置为下降沿捕获 */
        }
    }
}

/**
 * @brief       定时器更新中断回调函数
 * @param       htim : 定时器句柄指针
 * @note        此函数会被定时器中断函数共同调用的
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if((htim->Instance == QUERYA_GTIM_TIMX_CAP) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1))
    {
        if ((g_tim2_queryA_cap_sta & 0x80) == 0)                /* 还没成功捕获 */
        {
            if (g_tim2_queryA_cap_sta & 0x40)                   /* 已经捕获到高电平了 */
            {
                if ((g_tim2_queryA_cap_sta & 0x3F) == 0x3F)     /* 高电平太长了 */
                {
                    TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY);                     /* 一定要先清除原来的设置 */
                    TIM_SET_CAPTUREPOLARITY(&g_tim2_queryA_cap_handle, QUERYA_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);/* 配置TIM5通道1上升沿捕获 */
                    g_tim2_queryA_cap_sta |= 0x80;              /* 标记成功捕获了一次 */
                    g_tim2_queryA_cap_val = 0xFFFF;
                }
                else  /* 累计定时器溢出次数 */
                {
                    g_tim2_queryA_cap_sta++;
                }
            }
        }
		
		if ((g_tim2_queryA_cap_tm_sta & 0x80) == 0)                /* 还没成功捕获 */
        {
            if (g_tim2_queryA_cap_tm_sta & 0x40)                   /* 已经捕获到高电平了 */
            {
                if ((g_tim2_queryA_cap_tm_sta & 0x3F) == 0x3F)     /* 高电平太长了 */
                {
                    TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY);                     /* 一定要先清除原来的设置 */
                    TIM_SET_CAPTUREPOLARITY(&g_tim2_queryA_cap_tm_handle, QUERYA_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);/* 配置TIM5通道1上升沿捕获 */
                    g_tim2_queryA_cap_tm_sta |= 0x80;              /* 标记成功捕获了一次 */
                    g_tim2_queryA_cap_tm_val = 0xFFFF;
                }
                else  /* 累计定时器溢出次数 */
                {
                    g_tim2_queryA_cap_tm_sta++;
                }
            }
        }
    }
	
	if((htim->Instance == QUERYB_GTIM_TIMX_CAP) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2))
    {
        if ((g_tim2_queryB_cap_sta & 0x80) == 0)                /* 还没成功捕获 */
        {
            if (g_tim2_queryB_cap_sta & 0x40)                   /* 已经捕获到高电平了 */
            {
                if ((g_tim2_queryB_cap_sta & 0x3F) == 0x3F)     /* 高电平太长了 */
                {
                    TIM_RESET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY);                     /* 一定要先清除原来的设置 */
                    TIM_SET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);/* 配置TIM5通道1上升沿捕获 */
					//TIM_SET_CAPTUREPOLARITY(&g_tim2_queryB_cap_handle, QUERYB_GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING);/* 配置TIM5通道1上升沿捕获 */
                    g_tim2_queryB_cap_sta |= 0x80;              /* 标记成功捕获了一次 */
                    g_tim2_queryB_cap_val = 0xFFFF;
                }
                else  /* 累计定时器溢出次数 */
                {
                    g_tim2_queryB_cap_sta++;
                }
            }
        }
    }
}



