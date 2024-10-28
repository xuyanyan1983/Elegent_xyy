#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* ���� ���� */
/*IN_K1: PB13->PC7  
  IN_K2: PB14->PC8*/
#define KEY1_GPIO_PORT                  GPIOC
#define KEY1_GPIO_PIN                   GPIO_PIN_7
#define KEY1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

#define KEY2_GPIO_PORT                  GPIOC
#define KEY2_GPIO_PIN                   GPIO_PIN_8
#define KEY2_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */
/******************************************************************************************/

#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)     /* ��ȡKEY1���� */
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN)     /* ��ȡKEY2���� */

#define KEY1_PRES    1              /* KEY1���� */
#define KEY2_PRES    2              /* KEY2���� */

void key_init(void);                /* ������ʼ������ */
uint8_t key_scan(uint8_t mode);     /* ����ɨ�躯�� */

#endif


















