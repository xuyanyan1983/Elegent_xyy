/* RCM DO����������  */
#ifndef __GPIO_H
#define __GPIO_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* ���� ���� */
/*RCM_DO1:PB7->PB15
  RCM_DO2:PB6->PB14
  RCM_DO3:PB5->PB13
  RCM_DO4:PB4->PB12 */
#define RCM_DO1_GPIO_PORT                  GPIOB
#define RCM_DO1_GPIO_PIN                   GPIO_PIN_15
#define RCM_DO1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB��ʱ��ʹ�� */

#define RCM_DO2_GPIO_PORT                  GPIOB
#define RCM_DO2_GPIO_PIN                   GPIO_PIN_14
#define RCM_DO2_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB��ʱ��ʹ�� */

#define RCM_DO3_GPIO_PORT                  GPIOB
#define RCM_DO3_GPIO_PIN                   GPIO_PIN_13
#define RCM_DO3_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB��ʱ��ʹ�� */

#define RCM_DO4_GPIO_PORT                  GPIOB
#define RCM_DO4_GPIO_PIN                   GPIO_PIN_12
#define RCM_DO4_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB��ʱ��ʹ�� */

/*��س��ʹ��  1:���ʹ�ܣ�0����ֹ��� */
#define BAT_CHARGE_EN_GPIO_PORT            GPIOC
#define BAT_CHARGE_EN_GPIO_PIN             GPIO_PIN_13
#define BAT_CHARGE_EN_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)             /* PC��ʱ��ʹ�� */
/******************************************************************************************/
/* LED�˿ڶ��� */
#define RCM_DO1(x)   do{ x ? \
                      HAL_GPIO_WritePin(RCM_DO1_GPIO_PORT, RCM_DO1_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(RCM_DO1_GPIO_PORT, RCM_DO1_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)

#define RCM_DO2(x)   do{ x ? \
                      HAL_GPIO_WritePin(RCM_DO2_GPIO_PORT, RCM_DO2_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(RCM_DO2_GPIO_PORT, RCM_DO2_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)

#define RCM_DO3(x)   do{ x ? \
                      HAL_GPIO_WritePin(RCM_DO3_GPIO_PORT, RCM_DO3_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(RCM_DO3_GPIO_PORT, RCM_DO3_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)

#define RCM_DO4(x)   do{ x ? \
                      HAL_GPIO_WritePin(RCM_DO4_GPIO_PORT, RCM_DO4_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(RCM_DO4_GPIO_PORT, RCM_DO4_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)

#define BAT_CHARGE_EN(x)   do{ x ? \
                           HAL_GPIO_WritePin(BAT_CHARGE_EN_GPIO_PORT, BAT_CHARGE_EN_GPIO_PIN, GPIO_PIN_SET) : \
                           HAL_GPIO_WritePin(BAT_CHARGE_EN_GPIO_PORT, BAT_CHARGE_EN_GPIO_PIN, GPIO_PIN_RESET); \
                       }while(0)
/******************************************************************************************/
/* �ⲿ�ӿں���*/
void RCM_init(void);       /* ��ʼ�� */
#endif
