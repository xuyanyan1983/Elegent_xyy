#ifndef __QUERYABCAP_H
#define __QUERYABCAP_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* ͨ�ö�ʱ�� ���� */
 /* TIMX ���벶���� 
 * ��������벶��ʹ�ö�ʱ��TIM5_CH1,����WK_UP����������
 * Ĭ�������TIM2~TIM5. 
 * ע��: ͨ���޸��⼸���궨��,����֧��TIM1~TIM8����һ����ʱ��,����һ��IO�������벶��
 *       �ر�Ҫע��:Ĭ���õ�PA0,���õ�����������!���������IO,��Ӧ����������ʽҲ�ø�!
 */
 /* ��ѯ�ź������������ܲ��� */
 /* ��ѯ�ź�PA15���벶��*/
 /*query_INA :PA15->PA8  query_INB :PB3->PA0*/
#define QUERYA_GTIM_TIMX_CAP_CHY_GPIO_PORT         GPIOA
#define QUERYA_GTIM_TIMX_CAP_CHY_GPIO_PIN          GPIO_PIN_8
#define QUERYA_GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */
#define QUERYA_GTIM_TIMX_CAP_CHY_GPIO_AF           GPIO_AF1_TIM1                                 /* AF����ѡ�� */

#define QUERYA_GTIM_TIMX_CAP                       TIM1                       
#define QUERYA_GTIM_TIMX_CAP_IRQn                  TIM1_CC_IRQn
//#define GTIM_TIMX_CAP_IRQHandler                   TIM1_IRQHandler
#define QUERYA_GTIM_TIMX_CAP_CHY                   TIM_CHANNEL_1                                 /* ͨ��Y,  1<= Y <=4 */
#define QUERYA_GTIM_TIMX_CAP_CHY_CCRX              TIM1->CCR1                                    /* ͨ��Y������ȽϼĴ��� */
#define QUERYA_GTIM_TIMX_CAP_CHY_CLK_ENABLE()      do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)    /* TIM2 ʱ��ʹ�� */

#define QUERYB_GTIM_TIMX_CAP_CHY_GPIO_PORT         GPIOA
#define QUERYB_GTIM_TIMX_CAP_CHY_GPIO_PIN          GPIO_PIN_0
#define QUERYB_GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */
#define QUERYB_GTIM_TIMX_CAP_CHY_GPIO_AF           GPIO_AF1_TIM2                                 /* AF����ѡ�� */

#define QUERYB_GTIM_TIMX_CAP                       TIM2                       
#define QUERYB_GTIM_TIMX_CAP_IRQn                  TIM2_IRQn
//#define GTIM_TIMX_CAP_IRQHandler                   TIM2_IRQHandler
#define QUERYB_GTIM_TIMX_CAP_CHY                   TIM_CHANNEL_1                                 /* ͨ��Y,  1<= Y <=4 */
#define QUERYB_GTIM_TIMX_CAP_CHY_CCRX              TIM2->CCR1                                    /* ͨ��Y������ȽϼĴ��� */
#define QUERYB_GTIM_TIMX_CAP_CHY_CLK_ENABLE()      do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)    /* TIM2 ʱ��ʹ�� */
/******************************************************************************************/
void queryA_gtim_timx_cap_chy_init(uint32_t arr, uint16_t psc);                   /* ͨ�ö�ʱ�� ���벶���ʼ������ */
void queryB_gtim_timx_cap_chy_init(uint32_t arr, uint16_t psc);                   /* ͨ�ö�ʱ�� ���벶���ʼ������ */
				  
				  		  
#endif

















