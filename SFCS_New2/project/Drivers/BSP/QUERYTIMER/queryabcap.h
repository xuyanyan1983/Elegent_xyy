#ifndef __QUERYABCAP_H
#define __QUERYABCAP_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* 通用定时器 定义 */
 /* TIMX 输入捕获定义 
 * 这里的输入捕获使用定时器TIM5_CH1,捕获WK_UP按键的输入
 * 默认是针对TIM2~TIM5. 
 * 注意: 通过修改这几个宏定义,可以支持TIM1~TIM8任意一个定时器,任意一个IO口做输入捕获
 *       特别要注意:默认用的PA0,设置的是下拉输入!如果改其他IO,对应的上下拉方式也得改!
 */
 /* 查询信号是两个方向都能捕获 */
 /* 查询信号PA15输入捕获*/
 /*query_INA :PA15->PA8  query_INB :PB3->PA0*/
#define QUERYA_GTIM_TIMX_CAP_CHY_GPIO_PORT         GPIOA
#define QUERYA_GTIM_TIMX_CAP_CHY_GPIO_PIN          GPIO_PIN_8
#define QUERYA_GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */
#define QUERYA_GTIM_TIMX_CAP_CHY_GPIO_AF           GPIO_AF1_TIM1                                 /* AF功能选择 */

#define QUERYA_GTIM_TIMX_CAP                       TIM1                       
#define QUERYA_GTIM_TIMX_CAP_IRQn                  TIM1_CC_IRQn
//#define GTIM_TIMX_CAP_IRQHandler                   TIM1_IRQHandler
#define QUERYA_GTIM_TIMX_CAP_CHY                   TIM_CHANNEL_1                                 /* 通道Y,  1<= Y <=4 */
#define QUERYA_GTIM_TIMX_CAP_CHY_CCRX              TIM1->CCR1                                    /* 通道Y的输出比较寄存器 */
#define QUERYA_GTIM_TIMX_CAP_CHY_CLK_ENABLE()      do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)    /* TIM2 时钟使能 */

#define QUERYB_GTIM_TIMX_CAP_CHY_GPIO_PORT         GPIOA
#define QUERYB_GTIM_TIMX_CAP_CHY_GPIO_PIN          GPIO_PIN_0
#define QUERYB_GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */
#define QUERYB_GTIM_TIMX_CAP_CHY_GPIO_AF           GPIO_AF1_TIM2                                 /* AF功能选择 */

#define QUERYB_GTIM_TIMX_CAP                       TIM2                       
#define QUERYB_GTIM_TIMX_CAP_IRQn                  TIM2_IRQn
//#define GTIM_TIMX_CAP_IRQHandler                   TIM2_IRQHandler
#define QUERYB_GTIM_TIMX_CAP_CHY                   TIM_CHANNEL_1                                 /* 通道Y,  1<= Y <=4 */
#define QUERYB_GTIM_TIMX_CAP_CHY_CCRX              TIM2->CCR1                                    /* 通道Y的输出比较寄存器 */
#define QUERYB_GTIM_TIMX_CAP_CHY_CLK_ENABLE()      do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)    /* TIM2 时钟使能 */
/******************************************************************************************/
void queryA_gtim_timx_cap_chy_init(uint32_t arr, uint16_t psc);                   /* 通用定时器 输入捕获初始化函数 */
void queryB_gtim_timx_cap_chy_init(uint32_t arr, uint16_t psc);                   /* 通用定时器 输入捕获初始化函数 */
				  
				  		  
#endif

















