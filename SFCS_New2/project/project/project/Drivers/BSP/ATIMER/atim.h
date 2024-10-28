/**
 ****************************************************************************************************
 * @file        atim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-19
 * @brief       高级定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211019
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 高级定时器 定义 */

 /* TIMX 输出指定个数PWM 定义 
 * 这里输出的PWM通过PC6(TIM8_CH1)输出, 我们用杜邦线连接PC6和PF10, 然后在程序里面将PF10设置成浮空输入
 * 就可以 看到TIM8_CH1控制LED1(GREEN)的亮灭, 亮灭一次表示一个PWM波
 * 默认使用的是TIM8_CH1.
 * 注意: 通过修改这几个宏定义, 可以支持TIM1/TIM8定时器, 任意一个IO口输出指定个数的PWM
 */
#define ATIM_TIM1_CH1_NPWM_CHY_GPIO_PORT            GPIOA
#define ATIM_TIM1_CH1_NPWM_CHY_GPIO_PIN             GPIO_PIN_8
#define ATIM_TIM1_CH1_NPWM_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PC口时钟使能 */
#define ATIM_TIM1_CH1_NPWM_CHY_GPIO_AF              GPIO_AF1_TIM1

#if 1
#define ATIM_TIM1_CH1_NPWM                          TIM1
#define ATIM_TIM1_CH1_NPWM_IRQn                     TIM1_UP_TIM16_IRQn
//#define ATIM_TIM1_CH1_NPWM_IRQHandler               TIM1_UP_TIM16_IRQHandler
#define ATIM_TIM1_CH1_NPWM_CHY                      TIM_CHANNEL_1                               /* 通道Y,  1<= Y <=4 */
#define ATIM_TIM1_CH1_NPWM_CHY_CCRX                 TIM1->CCR1                                  /* 通道Y的输出比较寄存器 */
#define ATIM_TIM1_CH1_NPWM_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM1 时钟使能 */
#endif

#define ATIM_TIM1_CH2_NPWM_CHY_GPIO_PORT            GPIOA
#define ATIM_TIM1_CH2_NPWM_CHY_GPIO_PIN             GPIO_PIN_9
#define ATIM_TIM1_CH2_NPWM_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PC口时钟使能 */
#define ATIM_TIM1_CH2_NPWM_CHY_GPIO_AF              GPIO_AF1_TIM1

#define ATIM_TIM1_CH2_NPWM                          TIM1
#define ATIM_TIM1_CH2_NPWM_IRQn                     TIM1_UP_TIM16_IRQn
//#define ATIM_TIM1_CH2_NPWM_IRQHandler               TIM1_UP_TIM16_IRQHandler
#define ATIM_TIM1_CH2_NPWM_CHY                      TIM_CHANNEL_2                              /* 通道Y,  1<= Y <=4 */
#define ATIM_TIM1_CH2_NPWM_CHY_CCRX                 TIM1->CCR1                                  /* 通道Y的输出比较寄存器 */
#define ATIM_TIM1_CH2_NPWM_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM1 时钟使能 */

/******************************************************************************************/

void atim_tim1_ch1_npwm_chy_init(uint16_t arr, uint16_t psc);   /* 高级定时器 输出指定个数PWM初始化函数 */
void atim_tim1_ch1_npwm_chy_set(uint32_t npwm);                 /* 高级定时器 设置输出PWM的个数 */

void atim_tim1_ch2_npwm_chy_init(uint16_t arr, uint16_t psc);   /* 高级定时器 输出指定个数PWM初始化函数 */
void atim_tim1_ch2_npwm_chy_set(uint32_t npwm);                 /* 高级定时器 设置输出PWM的个数 */

#endif

