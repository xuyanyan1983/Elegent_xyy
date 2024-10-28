#include "./BSP/GPIO/gpio.h"
/**
 * @brief       初始化RCM的4个DO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void RCM_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    RCM_DO1_GPIO_CLK_ENABLE();                                 /* RCM DO1时钟使能 */
    RCM_DO2_GPIO_CLK_ENABLE();                                 /* RCM DO2时钟使能 */
    RCM_DO3_GPIO_CLK_ENABLE();                                 /* RCM DO3时钟使能 */
    RCM_DO4_GPIO_CLK_ENABLE();                                 /* RCM DO4时钟使能 */
	BAT_CHARGE_EN_GPIO_CLK_ENABLE();

    gpio_init_struct.Pin = RCM_DO1_GPIO_PIN;                   /* RCM DO1引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* 高速 */
    HAL_GPIO_Init(RCM_DO1_GPIO_PORT, &gpio_init_struct);       /* 初始化RCM DO1引脚 */
    
    gpio_init_struct.Pin = RCM_DO2_GPIO_PIN;                   /* RCM DO2引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* 高速 */
    HAL_GPIO_Init(RCM_DO2_GPIO_PORT, &gpio_init_struct);       /* 初始化RCM DO2引脚 */
    
    gpio_init_struct.Pin = RCM_DO3_GPIO_PIN;                   /* RCM DO3引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* 高速 */
    HAL_GPIO_Init(RCM_DO3_GPIO_PORT, &gpio_init_struct);       /* 初始化RCM DO3引脚 */
    
    gpio_init_struct.Pin = RCM_DO4_GPIO_PIN;                   /* RCM DO4引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* 高速 */
    HAL_GPIO_Init(RCM_DO4_GPIO_PORT, &gpio_init_struct);       /* 初始化RCM DO4引脚 */
    
    gpio_init_struct.Pin = BAT_CHARGE_EN_GPIO_PIN;             /* BAT_CHARGE_EN引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* 高速 */
    HAL_GPIO_Init(BAT_CHARGE_EN_GPIO_PORT, &gpio_init_struct); /* 初始化BAT_CHARGE_EN引脚 */
    
    RCM_DO1(0);                                                /* 关闭 RCM_DO1 高电平亮，即工作*/
    RCM_DO2(0);                                                /* 关闭 RCM_DO2 */
    RCM_DO3(0);                                                /* 关闭 RCM_DO3 */
    RCM_DO4(0);                                                /* 关闭 RCM_DO4 */
}



