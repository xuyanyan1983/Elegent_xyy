#include "./BSP/QUERY/query.h"
/**
 * @brief       ��ʼ��LED���IO��, ��ʹ��ʱ��
 * @param       ��
 * @retval      ��
 */
void QUERY_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    QUERYA_GPIO_CLK_ENABLE();                                   /* QUERYAʱ��ʹ�� */
    QUERYB_GPIO_CLK_ENABLE();                                   /* QUERYBʱ��ʹ�� */   
	QUERYAB_ON_OFF_GPIO_CLK_ENABLE();	
	MCU_CT_QUERYA_CLK_ENABLE();
	MCU_CT_QUERYB_CLK_ENABLE();

    gpio_init_struct.Pin = QUERYA_GPIO_PIN;                     /* QUERYA���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;//GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(QUERYA_GPIO_PORT, &gpio_init_struct);         /* ��ʼ��QUERYA���� */
    
    gpio_init_struct.Pin = QUERYB_GPIO_PIN;                     /* QUERYB���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;//GPIO_MODE_OUTPUT_PP;                /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(QUERYB_GPIO_PORT, &gpio_init_struct);         /* ��ʼ��QUERYB���� */

    gpio_init_struct.Pin = QUERYAB_ON_OFF_GPIO_PIN;             /* QUERYAB_ON_OFF���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(QUERYAB_ON_OFF_GPIO_PORT, &gpio_init_struct); /* ��ʼ��QUERYAB_ON_OFF���� */
	
	gpio_init_struct.Pin = MCU_CT_QUERYA_PIN;                   /* MCU_CT_QUERYA���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                /* ������� */
    gpio_init_struct.Pull = GPIO_NOPULL;//GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(MCU_CT_QUERYA_PORT, &gpio_init_struct);       /* ��ʼ��MCU_CT_QUERYA���� */
	
	gpio_init_struct.Pin = MCU_CT_QUERYB_PIN;                   /*MCU_CT_QUERYB���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                /* ������� */
    gpio_init_struct.Pull = GPIO_NOPULL;//GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(MCU_CT_QUERYB_PORT, &gpio_init_struct);       /* ��ʼ��MCU_CT_QUERYB���� */
    
    QUERYAB_ON_OFF(0);                                        /* �Ͽ� QUERYAB_ON_OFF */
	MCU_CT_QUERYA(0);
	MCU_CT_QUERYB(0);

}
