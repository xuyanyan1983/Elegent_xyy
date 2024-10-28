#include "./BSP/GPIO/gpio.h"
/**
 * @brief       ��ʼ��RCM��4��DO��, ��ʹ��ʱ��
 * @param       ��
 * @retval      ��
 */
void RCM_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    RCM_DO1_GPIO_CLK_ENABLE();                                 /* RCM DO1ʱ��ʹ�� */
    RCM_DO2_GPIO_CLK_ENABLE();                                 /* RCM DO2ʱ��ʹ�� */
    RCM_DO3_GPIO_CLK_ENABLE();                                 /* RCM DO3ʱ��ʹ�� */
    RCM_DO4_GPIO_CLK_ENABLE();                                 /* RCM DO4ʱ��ʹ�� */
	BAT_CHARGE_EN_GPIO_CLK_ENABLE();

    gpio_init_struct.Pin = RCM_DO1_GPIO_PIN;                   /* RCM DO1���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* ���� */
    HAL_GPIO_Init(RCM_DO1_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��RCM DO1���� */
    
    gpio_init_struct.Pin = RCM_DO2_GPIO_PIN;                   /* RCM DO2���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* ���� */
    HAL_GPIO_Init(RCM_DO2_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��RCM DO2���� */
    
    gpio_init_struct.Pin = RCM_DO3_GPIO_PIN;                   /* RCM DO3���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* ���� */
    HAL_GPIO_Init(RCM_DO3_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��RCM DO3���� */
    
    gpio_init_struct.Pin = RCM_DO4_GPIO_PIN;                   /* RCM DO4���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* ���� */
    HAL_GPIO_Init(RCM_DO4_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��RCM DO4���� */
    
    gpio_init_struct.Pin = BAT_CHARGE_EN_GPIO_PIN;             /* BAT_CHARGE_EN���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;             /* ���� */
    HAL_GPIO_Init(BAT_CHARGE_EN_GPIO_PORT, &gpio_init_struct); /* ��ʼ��BAT_CHARGE_EN���� */
    
    RCM_DO1(0);                                                /* �ر� RCM_DO1 �ߵ�ƽ����������*/
    RCM_DO2(0);                                                /* �ر� RCM_DO2 */
    RCM_DO3(0);                                                /* �ر� RCM_DO3 */
    RCM_DO4(0);                                                /* �ر� RCM_DO4 */
}



