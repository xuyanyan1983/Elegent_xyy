/**
 ****************************************************************************************************
 * @file        atim.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-19
 * @brief       �߼���ʱ�� ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211019
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* �߼���ʱ�� ���� */

 /* TIMX ���ָ������PWM ���� 
 * ���������PWMͨ��PC6(TIM8_CH1)���, �����öŰ�������PC6��PF10, Ȼ���ڳ������潫PF10���óɸ�������
 * �Ϳ��� ����TIM8_CH1����LED1(GREEN)������, ����һ�α�ʾһ��PWM��
 * Ĭ��ʹ�õ���TIM8_CH1.
 * ע��: ͨ���޸��⼸���궨��, ����֧��TIM1/TIM8��ʱ��, ����һ��IO�����ָ��������PWM
 */
#define ATIM_TIM1_CH1_NPWM_CHY_GPIO_PORT            GPIOA
#define ATIM_TIM1_CH1_NPWM_CHY_GPIO_PIN             GPIO_PIN_8
#define ATIM_TIM1_CH1_NPWM_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PC��ʱ��ʹ�� */
#define ATIM_TIM1_CH1_NPWM_CHY_GPIO_AF              GPIO_AF1_TIM1

#if 1
#define ATIM_TIM1_CH1_NPWM                          TIM1
#define ATIM_TIM1_CH1_NPWM_IRQn                     TIM1_UP_TIM16_IRQn
//#define ATIM_TIM1_CH1_NPWM_IRQHandler               TIM1_UP_TIM16_IRQHandler
#define ATIM_TIM1_CH1_NPWM_CHY                      TIM_CHANNEL_1                               /* ͨ��Y,  1<= Y <=4 */
#define ATIM_TIM1_CH1_NPWM_CHY_CCRX                 TIM1->CCR1                                  /* ͨ��Y������ȽϼĴ��� */
#define ATIM_TIM1_CH1_NPWM_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM1 ʱ��ʹ�� */
#endif

#define ATIM_TIM1_CH2_NPWM_CHY_GPIO_PORT            GPIOA
#define ATIM_TIM1_CH2_NPWM_CHY_GPIO_PIN             GPIO_PIN_9
#define ATIM_TIM1_CH2_NPWM_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PC��ʱ��ʹ�� */
#define ATIM_TIM1_CH2_NPWM_CHY_GPIO_AF              GPIO_AF1_TIM1

#define ATIM_TIM1_CH2_NPWM                          TIM1
#define ATIM_TIM1_CH2_NPWM_IRQn                     TIM1_UP_TIM16_IRQn
//#define ATIM_TIM1_CH2_NPWM_IRQHandler               TIM1_UP_TIM16_IRQHandler
#define ATIM_TIM1_CH2_NPWM_CHY                      TIM_CHANNEL_2                              /* ͨ��Y,  1<= Y <=4 */
#define ATIM_TIM1_CH2_NPWM_CHY_CCRX                 TIM1->CCR1                                  /* ͨ��Y������ȽϼĴ��� */
#define ATIM_TIM1_CH2_NPWM_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM1 ʱ��ʹ�� */

/******************************************************************************************/

void atim_tim1_ch1_npwm_chy_init(uint16_t arr, uint16_t psc);   /* �߼���ʱ�� ���ָ������PWM��ʼ������ */
void atim_tim1_ch1_npwm_chy_set(uint32_t npwm);                 /* �߼���ʱ�� �������PWM�ĸ��� */

void atim_tim1_ch2_npwm_chy_init(uint16_t arr, uint16_t psc);   /* �߼���ʱ�� ���ָ������PWM��ʼ������ */
void atim_tim1_ch2_npwm_chy_set(uint32_t npwm);                 /* �߼���ʱ�� �������PWM�ĸ��� */

#endif
