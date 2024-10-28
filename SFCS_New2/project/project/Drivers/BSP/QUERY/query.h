#ifndef __QUERY_H
#define __QUERY_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* ���� ���� */
/* ��queryB����query�źţ�queryA����˿�*/
#define QUERYA_GPIO_PORT                     GPIOA
#define QUERYA_GPIO_PIN                      GPIO_PIN_15                    /* queryA����˿�*/
#define QUERYA_GPIO_CLK_ENABLE()             do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PA��ʱ��ʹ�� */

/* ��queryA����query�źţ�queryB����˿�*/
#define QUERYB_GPIO_PORT                     GPIOB
#define QUERYB_GPIO_PIN                      GPIO_PIN_3                     /* queryB����˿�*/
#define QUERYB_GPIO_CLK_ENABLE()             do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB��ʱ��ʹ�� */

//��ѯ���ӿ���
#define QUERYAB_ON_OFF_GPIO_PORT             GPIOB
#define QUERYAB_ON_OFF_GPIO_PIN              GPIO_PIN_2                    /*QUERYAB_ON_OFF ���ӿ�������˿�*/
#define QUERYAB_ON_OFF_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB��ʱ��ʹ�� */
/* MCU_CT_QUERYA��MCU_CT_QUERYB�Ķ���*/
#define MCU_CT_QUERYA_PORT                   GPIOA
#define MCU_CT_QUERYA_PIN                    GPIO_PIN_8                    /*QUERYAB_ON_OFF ���ӿ�������˿�*/
#define MCU_CT_QUERYA_CLK_ENABLE()           do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PB��ʱ��ʹ�� */

#define MCU_CT_QUERYB_PORT                   GPIOA
#define MCU_CT_QUERYB_PIN                    GPIO_PIN_9                    /*QUERYAB_ON_OFF ���ӿ�������˿�*/
#define MCU_CT_QUERYB_CLK_ENABLE()           do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PB��ʱ��ʹ�� */
/******************************************************************************************/
#define QUERYAB_ON_OFF(x)   do{ x ? \
                      HAL_GPIO_WritePin(QUERYAB_ON_OFF_GPIO_PORT, QUERYAB_ON_OFF_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(QUERYAB_ON_OFF_GPIO_PORT, QUERYAB_ON_OFF_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)       /*QUERYB= QUERY_INB */

#define MCU_CT_QUERYA(x)   do{ x ? \
                      HAL_GPIO_WritePin(MCU_CT_QUERYA_PORT, MCU_CT_QUERYA_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(MCU_CT_QUERYA_PORT, MCU_CT_QUERYA_PIN, GPIO_PIN_RESET); \
                  }while(0)       /* MCU_CT_QUERYA */

#define MCU_CT_QUERYB(x)   do{ x ? \
                      HAL_GPIO_WritePin(MCU_CT_QUERYB_PORT, MCU_CT_QUERYB_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(MCU_CT_QUERYB_PORT, MCU_CT_QUERYB_PIN, GPIO_PIN_RESET); \
                  }while(0)       /* MCU_CT_QUERYB */
/******************************************************************************************/
/* �ⲿ�ӿں���*/
void QUERY_init(void);                                                                            /* ��ʼ�� */

#endif