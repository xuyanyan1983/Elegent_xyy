#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* ���� ���� */

#define KEY1_GPIO_PORT                  GPIOB
#define KEY1_GPIO_PIN                   GPIO_PIN_13
#define KEY1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

#define KEY2_GPIO_PORT                  GPIOB
#define KEY2_GPIO_PIN                   GPIO_PIN_14
#define KEY2_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */
/******************************************************************************************/

#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)     /* ��ȡKEY1���� */
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN)     /* ��ȡKEY2���� */

#define KEY1_PRES    1              /* KEY1���� */
#define KEY2_PRES    2              /* KEY2���� */

void key_init(void);                /* ������ʼ������ */
uint8_t key_scan(uint8_t mode);     /* ����ɨ�躯�� */

#endif


















