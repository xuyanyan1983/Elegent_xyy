/**
  ******************************************************************************
  * @file    ADC/ADC_RegularConversion_Polling/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor ADCx instance used and associated
   resources */
   
/* Definition for ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL1_PIN                GPIO_PIN_0
#define ADCx_CHANNEL1_GPIO_PORT          GPIOB

#define ADCx_CHANNEL2_PIN                GPIO_PIN_1
#define ADCx_CHANNEL2_GPIO_PORT          GPIOB

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL1                    ADC_CHANNEL_15
#define ADCx_CHANNEL2                    ADC_CHANNEL_16
#define SAMPLINGTIME                    ADC_SAMPLETIME_6CYCLES_5
/* Exported macro ------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */

uint16_t adc_sigChann_get_value(ADC_HandleTypeDef AdcHandle,ADC_ChannelConfTypeDef sConfig,uint8_t ch);
#endif /* __MAIN_H */

