#ifndef __LOCKPWM_H
#define __LOCKPWM_H

#include "stm32l4xx_hal.h"

/* Definition for TIMx clock resources */
#define TIMx                           TIM15
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM15_CLK_ENABLE()


/* Definition for TIMx Channel Pins */
#define TIMx_CHANNEL_GPIO_PORT()       __HAL_RCC_GPIOA_CLK_ENABLE();
#define TIMx_GPIO_PORT_CHANNEL1        GPIOA
#define TIMx_GPIO_PIN_CHANNEL1         GPIO_PIN_2
#define TIMx_GPIO_AF_CHANNEL1          GPIO_AF14_TIM15
void gtim_pwm_lock_init(void);

#endif /* __MAIN_H */

