#ifndef __RCMDI_H
#define __RCMDI_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* ���� ���� */
/*RCM_DI1:PA0->PC3
  RCM_DI2:PA1->PC1->PA2
  RCM_DI3:PA2->PC0->PA3
  RCM_DI4:PA3->PB5->PA4
  RCM_DI5:PA4->PC12->PA5
  RCM_DI6:PA5->PA1->PA6
  RCM_DI7:PA6->PA2->PA7
  RCM_DI8:PA7->PC4->PB2
  */
#define RCMDI1_GPIO_PORT                  GPIOC
#define RCMDI1_GPIO_PIN                   GPIO_PIN_3
#define RCMDI1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define RCMDI2_GPIO_PORT                  GPIOA
#define RCMDI2_GPIO_PIN                   GPIO_PIN_2
#define RCMDI2_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define RCMDI3_GPIO_PORT                  GPIOA
#define RCMDI3_GPIO_PIN                   GPIO_PIN_3
#define RCMDI3_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define RCMDI4_GPIO_PORT                  GPIOA
#define RCMDI4_GPIO_PIN                   GPIO_PIN_4
#define RCMDI4_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define RCMDI5_GPIO_PORT                  GPIOA
#define RCMDI5_GPIO_PIN                   GPIO_PIN_5
#define RCMDI5_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define RCMDI6_GPIO_PORT                  GPIOA
#define RCMDI6_GPIO_PIN                   GPIO_PIN_6
#define RCMDI6_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define RCMDI7_GPIO_PORT                  GPIOA
#define RCMDI7_GPIO_PIN                   GPIO_PIN_7
#define RCMDI7_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define RCMDI8_GPIO_PORT                  GPIOB
#define RCMDI8_GPIO_PIN                   GPIO_PIN_2
#define RCMDI8_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

/*lock���button*/
/*IN_BTN2: PA10->PA5->PB5  */
#define IN_BTN2_GPIO_PORT                 GPIOB
#define IN_BTN2_GPIO_PIN                  GPIO_PIN_5
#define IN_BTN2_GPIO_CLK_ENABLE()         do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

/* IN_SD_AU��⹦������ģ�����������ˣ����ܳ�磬���Ͽ�*/
/*IN_SD_AU: PB15->PC9  */
#define IN_SD_AU_GPIO_PORT                GPIOC
#define IN_SD_AU_GPIO_PIN                 GPIO_PIN_9
#define IN_SD_AU_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)          /* PB��ʱ��ʹ�� */

/* AUDIO_OUT��Ƶ������˿�*/
/*����ӵ�*/
#define AUDIO_OUT_GPIO_PORT                GPIOB
#define AUDIO_OUT_GPIO_PIN                 GPIO_PIN_11
#define AUDIO_OUT_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)          /* PB��ʱ��ʹ�� */

/*lock_stop */
/*LOCK_STOP: PB12->PC6  */
#define LOCK_STOP_GPIO_PORT               GPIOC
#define LOCK_STOP_GPIO_PIN                GPIO_PIN_6
#define LOCK_STOP_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)       /* PB��ʱ��ʹ�� */
/*DC_lock*/
/*DC_lock: PA11->PD2  */
#define DC_LOCK_GPIO_PORT                 GPIOD
#define DC_LOCK_GPIO_PIN                  GPIO_PIN_2
#define DC_LOCK_GPIO_CLK_ENABLE()         do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)         /* PB��ʱ��ʹ�� */

/*AC_lock: PB3  */
#define AC_LOCK_GPIO_PORT                 GPIOB
#define AC_LOCK_GPIO_PIN                  GPIO_PIN_3
#define AC_LOCK_GPIO_CLK_ENABLE()         do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)         /* PB��ʱ��ʹ�� */
/******************************************************************************************/

#define RCMDI1        HAL_GPIO_ReadPin(RCMDI1_GPIO_PORT, RCMDI1_GPIO_PIN)     /* ��ȡRCMDI1���� */
#define RCMDI2        HAL_GPIO_ReadPin(RCMDI2_GPIO_PORT, RCMDI2_GPIO_PIN)     /* ��ȡRCMDI2���� */
#define RCMDI3        HAL_GPIO_ReadPin(RCMDI3_GPIO_PORT, RCMDI3_GPIO_PIN)     /* ��ȡRCMDI3���� */
#define RCMDI4        HAL_GPIO_ReadPin(RCMDI4_GPIO_PORT, RCMDI4_GPIO_PIN)     /* ��ȡRCMDI4���� */
#define RCMDI5        HAL_GPIO_ReadPin(RCMDI5_GPIO_PORT, RCMDI5_GPIO_PIN)     /* ��ȡRCMDI5���� */
#define RCMDI6        HAL_GPIO_ReadPin(RCMDI6_GPIO_PORT, RCMDI6_GPIO_PIN)     /* ��ȡRCMDI6���� */
#define RCMDI7        HAL_GPIO_ReadPin(RCMDI7_GPIO_PORT, RCMDI7_GPIO_PIN)     /* ��ȡRCMDI7���� */
#define RCMDI8        HAL_GPIO_ReadPin(RCMDI8_GPIO_PORT, RCMDI8_GPIO_PIN)     /* ��ȡRCMDI8���� */

#define IN_BTN2       HAL_GPIO_ReadPin(IN_BTN2_GPIO_PORT, IN_BTN2_GPIO_PIN)     /* ��ȡ��ͣ��ť���� */
#define IN_SD_AU      HAL_GPIO_ReadPin(IN_SD_AU_GPIO_PORT, IN_SD_AU_GPIO_PIN)           /* ��ȡ��Ƶ�źż������ */
#define LOCK_STOP     HAL_GPIO_ReadPin(LOCK_STOP_GPIO_PORT, LOCK_STOP_GPIO_PIN)         /* ��ȡlock�źż������ */
#define DC_LOCK       HAL_GPIO_ReadPin(DC_LOCK_GPIO_PORT, DC_LOCK_GPIO_PIN)             /* ��ȡlock�źż������ */
#define AC_LOCK       HAL_GPIO_ReadPin(AC_LOCK_GPIO_PORT, AC_LOCK_GPIO_PIN)             /* ��ȡlock�źż������ */
#define AUDIO_OUT     HAL_GPIO_ReadPin(AUDIO_OUT_GPIO_PORT, AUDIO_OUT_GPIO_PIN)         /* ��ȡ��Ƶ�źż������ */

void rcmdi_init(void);                                                                  /* RCMDI�˿ڳ�ʼ������ */
void AudioCheck_init(void);
void lock_init(void);                                                                   /* ��ʼ�� */  
#endif


















