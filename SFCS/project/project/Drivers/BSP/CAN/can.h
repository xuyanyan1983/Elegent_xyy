#ifndef __CAN_H
#define __CAN_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* CAN ���� ���� */
#define CAN_RX_GPIO_PORT                GPIOB
#define CAN_RX_GPIO_PIN                 GPIO_PIN_8
#define CAN_RX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* PB��ʱ��ʹ�� */

#define CAN_TX_GPIO_PORT                GPIOB
#define CAN_TX_GPIO_PIN                 GPIO_PIN_9
#define CAN_TX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* PB��ʱ��ʹ�� */

/* canʹ������*/
#define CAN_EN_GPIO_PORT                GPIOA
#define CAN_EN_GPIO_PIN                 GPIO_PIN_12
#define CAN_EN_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)  
/******************************************************************************************/

/* CANʹ�ܶ˿ڶ��� */
#define CAN_EN(x)   do{ x ? \
                      HAL_GPIO_WritePin(CAN_EN_GPIO_PORT, CAN_EN_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(CAN_EN_GPIO_PORT, CAN_EN_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)  

/* CAN����RX0�ж�ʹ�� */
#define CAN_RX0_INT_ENABLE              CAN1_RX0_IRQHandler

/* �������� */
uint8_t can_receive_msg(uint32_t id, uint8_t *buf);                  
uint8_t can_send_msg_data(uint32_t id, uint8_t *msg, uint8_t len);                      /* CAN�������� */
uint8_t can_init(uint32_t tsjw,uint32_t tbs2,uint32_t tbs1,uint16_t brp,uint32_t mode); /* CAN��ʼ�� */

#endif








