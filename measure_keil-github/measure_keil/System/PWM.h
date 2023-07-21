#ifndef __PWM_H
#define __PWM_H
#include <stdio.h>

// CONVST(PWM)引脚 PB6 定时器PWM
#define AD_CONVST_PIN                    GPIO_Pin_6
#define AD_CONVST_GPIO_PORT              GPIOB		
#define AD_CONVST_GPIO_CLK               RCC_APB2Periph_GPIOB


void PWM_Config(void);                          // 初始化PB6引脚时钟以及TIM4时钟
void PWM_Init(uint16_t arr, uint16_t duty);		// 配置PWM
#endif
