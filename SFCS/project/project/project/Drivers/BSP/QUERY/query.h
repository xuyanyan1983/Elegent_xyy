#ifndef __QUERY_H
#define __QUERY_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* 引脚 定义 */
/* 从queryB捕获query信号，queryA输出端口*/
#define QUERYA_GPIO_PORT                     GPIOA
#define QUERYA_GPIO_PIN                      GPIO_PIN_15                    /* queryA输入端口*/
#define QUERYA_GPIO_CLK_ENABLE()             do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PA口时钟使能 */

/* 从queryA捕获query信号，queryB输出端口*/
#define QUERYB_GPIO_PORT                     GPIOB
#define QUERYB_GPIO_PIN                      GPIO_PIN_3                     /* queryB输出端口*/
#define QUERYB_GPIO_CLK_ENABLE()             do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */

//查询电子开关
#define QUERYAB_ON_OFF_GPIO_PORT             GPIOB
#define QUERYAB_ON_OFF_GPIO_PIN              GPIO_PIN_2                    /*QUERYAB_ON_OFF 电子开关输出端口*/
#define QUERYAB_ON_OFF_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */
/* MCU_CT_QUERYA和MCU_CT_QUERYB的定义*/
#define MCU_CT_QUERYA_PORT                   GPIOA
#define MCU_CT_QUERYA_PIN                    GPIO_PIN_8                    /*QUERYAB_ON_OFF 电子开关输出端口*/
#define MCU_CT_QUERYA_CLK_ENABLE()           do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */

#define MCU_CT_QUERYB_PORT                   GPIOA
#define MCU_CT_QUERYB_PIN                    GPIO_PIN_9                    /*QUERYAB_ON_OFF 电子开关输出端口*/
#define MCU_CT_QUERYB_CLK_ENABLE()           do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */
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
/* 外部接口函数*/
void QUERY_init(void);                                                                            /* 初始化 */

#endif
