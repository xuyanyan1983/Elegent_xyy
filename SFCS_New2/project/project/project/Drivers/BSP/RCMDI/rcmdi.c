#include "./BSP/RCMDI/rcmdi.h"
#include "./SYSTEM/delay/delay.h"
extern uint8_t rcmDiInitStatus[8];                    /* RCM8个DI的初始状态*/   
extern uint8_t rcmDiItFlag[8];

void rcmdi_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;                            /* GPIO配置参数存储变量 */
    RCMDI1_GPIO_CLK_ENABLE();                                     /* RCMDI1时钟使能 */
	IN_BTN2_GPIO_CLK_ENABLE();                                    /*lock button 时钟使能*/

    gpio_init_struct.Pin = RCMDI1_GPIO_PIN;                       /* RCMDI1引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;          /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                          /* 上拉 */
    HAL_GPIO_Init(RCMDI1_GPIO_PORT, &gpio_init_struct);           /* RCMDI1引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = RCMDI2_GPIO_PIN;                       /* RCMDI2引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;          /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                          /* 上拉 */
    HAL_GPIO_Init(RCMDI2_GPIO_PORT, &gpio_init_struct);           /* RCMDI2引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = RCMDI3_GPIO_PIN;                       /* RCMDI3引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;          /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                          /* 上拉 */
    HAL_GPIO_Init(RCMDI3_GPIO_PORT, &gpio_init_struct);           /* RCMDI3引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = RCMDI4_GPIO_PIN;                       /* RCMDI4引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;          /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                          /* 下拉 */
    HAL_GPIO_Init(RCMDI4_GPIO_PORT, &gpio_init_struct);           /* RCMDI4引脚模式设置,上拉输入 */
    
    gpio_init_struct.Pin = RCMDI5_GPIO_PIN;                       /* RCMDI5引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;          /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                          /* 上拉 */
    HAL_GPIO_Init(RCMDI5_GPIO_PORT, &gpio_init_struct);           /* RCMDI5引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = RCMDI6_GPIO_PIN;                       /* RCMDI6引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;          /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                          /* 上拉 */
    HAL_GPIO_Init(RCMDI6_GPIO_PORT, &gpio_init_struct);           /* RCMDI6引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = RCMDI7_GPIO_PIN;                       /* RCMDI7引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;          /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                          /* 上拉 */
    HAL_GPIO_Init(RCMDI7_GPIO_PORT, &gpio_init_struct);           /* RCMDI7引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = RCMDI8_GPIO_PIN;                       /* RCMDI8引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;          /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                          /* 上拉 */
    HAL_GPIO_Init(RCMDI8_GPIO_PORT, &gpio_init_struct);           /* RCMDI8引脚模式设置,上拉输入 */
    
    /*lock按钮*/
    gpio_init_struct.Pin = IN_BTN2_GPIO_PIN;                      /* lock按钮引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                      /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                          /* 上拉 */
    HAL_GPIO_Init(IN_BTN2_GPIO_PORT, &gpio_init_struct);          /* lock按钮引脚模式设置,上拉输入 */
	
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 1);
	HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 2);
	HAL_NVIC_SetPriority(EXTI3_IRQn, 1, 3);
	HAL_NVIC_SetPriority(EXTI4_IRQn, 1, 4);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 5);
	
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	HAL_GPIO_ReadPin(RCMDI1_GPIO_PORT, RCMDI1_GPIO_PIN);     /* 读取RCMDI1引脚 */
	HAL_GPIO_ReadPin(RCMDI2_GPIO_PORT, RCMDI2_GPIO_PIN);     /* 读取RCMDI2引脚 */
	HAL_GPIO_ReadPin(RCMDI3_GPIO_PORT, RCMDI3_GPIO_PIN);     /* 读取RCMDI3引脚 */
	HAL_GPIO_ReadPin(RCMDI4_GPIO_PORT, RCMDI4_GPIO_PIN);     /* 读取RCMDI4引脚 */
	HAL_GPIO_ReadPin(RCMDI5_GPIO_PORT, RCMDI5_GPIO_PIN);     /* 读取RCMDI5引脚 */
	HAL_GPIO_ReadPin(RCMDI6_GPIO_PORT, RCMDI6_GPIO_PIN);     /* 读取RCMDI6引脚 */
	HAL_GPIO_ReadPin(RCMDI7_GPIO_PORT, RCMDI7_GPIO_PIN);     /* 读取RCMDI7引脚 */
	HAL_GPIO_ReadPin(RCMDI8_GPIO_PORT, RCMDI8_GPIO_PIN);     /* 读取RCMDI8引脚 */
}

void AudioCheck_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    AUDIO_OUT_GPIO_CLK_ENABLE();                             /* IN_SD_AU时钟使能 */
	
    gpio_init_struct.Pin = AUDIO_OUT_GPIO_PIN;               /* IN_SD_AU引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(AUDIO_OUT_GPIO_PORT, &gpio_init_struct);   /* 初始化IN_SD_AU引脚 */
}

void lock_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    //LOCK_STOP_GPIO_CLK_ENABLE();                            /* lock stop时钟使能 */
	DC_LOCK_GPIO_CLK_ENABLE();                              /* DC_lock时钟使能 */

    //gpio_init_struct.Pin = LOCK_STOP_GPIO_PIN;              /*lock stop引脚 */
    //gpio_init_struct.Mode = GPIO_MODE_INPUT;                /* 推挽输出 */
    //gpio_init_struct.Pull = GPIO_NOPULL;                    /* 不用上下拉 */
    //gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    //HAL_GPIO_Init(LOCK_STOP_GPIO_PORT, &gpio_init_struct);  /* 初始化lock stop引脚 */
	
	gpio_init_struct.Pin = DC_LOCK_GPIO_PIN;                /*DC lock引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_NOPULL;                    /* 不用上下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(DC_LOCK_GPIO_PORT, &gpio_init_struct);  /* 初始化lock stop引脚 */
}

void EXTI0_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(RCMDI1_GPIO_PIN);
}

void EXTI1_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(RCMDI2_GPIO_PIN);
}

void EXTI2_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(RCMDI3_GPIO_PIN);
}

void EXTI3_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(RCMDI4_GPIO_PIN);
}

void EXTI4_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(RCMDI5_GPIO_PIN);
}

void EXTI9_5_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(RCMDI6_GPIO_PIN);
	HAL_GPIO_EXTI_IRQHandler(RCMDI7_GPIO_PIN);
	HAL_GPIO_EXTI_IRQHandler(RCMDI8_GPIO_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == RCMDI1_GPIO_PIN)
	{
		rcmDiItFlag[0] = 1;
		HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	}
	else if(GPIO_Pin == RCMDI2_GPIO_PIN)
	{
		rcmDiItFlag[1] = 1;
		HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	}
	else if(GPIO_Pin == RCMDI3_GPIO_PIN)
	{
		rcmDiItFlag[2] = 1;
		HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	}
	else if(GPIO_Pin == RCMDI4_GPIO_PIN)
	{
		rcmDiItFlag[3] = 1;
		HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	}
	else if(GPIO_Pin == RCMDI5_GPIO_PIN)
	{
		rcmDiItFlag[4] = 1;
		HAL_NVIC_DisableIRQ(EXTI4_IRQn);
	}
	else if(GPIO_Pin == RCMDI6_GPIO_PIN)
	{
		rcmDiItFlag[5] = 1;
		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	}
	else if(GPIO_Pin == RCMDI7_GPIO_PIN)
	{
		rcmDiItFlag[6] = 1;
		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	}
	else if(GPIO_Pin == RCMDI8_GPIO_PIN)
	{
		rcmDiItFlag[7] = 1;
		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	}
	else
	{
		;
	}
}










