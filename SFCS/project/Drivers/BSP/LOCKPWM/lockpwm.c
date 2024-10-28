#include "lockpwm.h"
/*功能：TM产生lock信号*/
#define  PERIOD_VALUE       (uint32_t)(205 - 1)               /* ARR=Period Value  */
#define  PERIOD_VALUE       (uint32_t)(28000 - 1)               /* ARR=Period Value  */
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        /* Capture Compare 1 Value  */
/* Timer handler declaration */
TIM_HandleTypeDef    TimHandle;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;

/* Counter Prescaler value */
uint32_t uhPrescalerValue = 0;
static void Error_Handler(void);

void gtim_pwm_lock_init()
{ 
  /* Compute the prescaler value to have TIM2 counter clock equal to 8000000 Hz */
  uhPrescalerValue = (uint32_t)(SystemCoreClock / 8000000) - 1;       /*PSC*/
  //uhPrescalerValue = (uint32_t)(SystemCoreClock / 2000000) - 1;       /*PSC*/
  /* Initialize TIMx peripheral as follows:
       + Prescaler = (SystemCoreClock / 8000000) - 1
       + Period = (205 - 1)
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Instance = TIMx;

  TimHandle.Init.Prescaler         = uhPrescalerValue;
  TimHandle.Init.Period            = PERIOD_VALUE;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for all channels */
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  //sConfig.OCNPolarity  = 
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  /* Set the pulse value for channel 1 */
  sConfig.Pulse = PULSE1_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
  /*##-3- Start PWM signals generation #######################################*/
  /* Start channel 1 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM15)
	{
		GPIO_InitTypeDef   GPIO_InitStruct;
		/* TIMx Peripheral clock enable */
		TIMx_CLK_ENABLE();

		/* Enable all GPIO Channels Clock requested */
		TIMx_CHANNEL_GPIO_PORT();
		/* Common configuration for all channels */
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

		GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL1;
		GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL1;
		HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL1, &GPIO_InitStruct);
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  while (1)
  {
  }
}




