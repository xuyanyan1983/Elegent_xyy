#include "./BSP/QUERY/query.h"
/**
 * @brief       初始化LED相关IO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void QUERY_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    QUERYA_GPIO_CLK_ENABLE();                                   /* QUERYA时钟使能 */
    QUERYB_GPIO_CLK_ENABLE();                                   /* QUERYB时钟使能 */   
	QUERYAB_ON_OFF_GPIO_CLK_ENABLE();	
	MCU_CT_QUERYA_CLK_ENABLE();
	MCU_CT_QUERYB_CLK_ENABLE();

    gpio_init_struct.Pin = QUERYA_GPIO_PIN;                     /* QUERYA引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;//GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(QUERYA_GPIO_PORT, &gpio_init_struct);         /* 初始化QUERYA引脚 */
    
    gpio_init_struct.Pin = QUERYB_GPIO_PIN;                     /* QUERYB引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;//GPIO_MODE_OUTPUT_PP;                /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(QUERYB_GPIO_PORT, &gpio_init_struct);         /* 初始化QUERYB引脚 */

    gpio_init_struct.Pin = QUERYAB_ON_OFF_GPIO_PIN;             /* QUERYAB_ON_OFF引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(QUERYAB_ON_OFF_GPIO_PORT, &gpio_init_struct); /* 初始化QUERYAB_ON_OFF引脚 */
	
	gpio_init_struct.Pin = MCU_CT_QUERYA_PIN;                   /* MCU_CT_QUERYA引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_NOPULL;//GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(MCU_CT_QUERYA_PORT, &gpio_init_struct);       /* 初始化MCU_CT_QUERYA引脚 */
	
	gpio_init_struct.Pin = MCU_CT_QUERYB_PIN;                   /*MCU_CT_QUERYB引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_NOPULL;//GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(MCU_CT_QUERYB_PORT, &gpio_init_struct);       /* 初始化MCU_CT_QUERYB引脚 */
    
    QUERYAB_ON_OFF(0);                                        /* 断开 QUERYAB_ON_OFF */
	MCU_CT_QUERYA(0);
	MCU_CT_QUERYB(0);

}
