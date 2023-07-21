#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "PWM.h"


/*
*	@brief:  PWM_Config
*	@param:  void
*	@return: void
*	@note:   初始化PB6引脚时钟以及TIM4时钟
*/
void PWM_Config(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(AD_CONVST_GPIO_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = AD_CONVST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(AD_CONVST_GPIO_PORT, &GPIO_InitStructure);
}

/*
*	@brief:  TIM_Init
*	@param:  uint16_t arr(ARR自动重装载寄存器的值), uint16_t duty(设置占空比)
*	@return: void
*	@note:   配置PWM 周期和分频比决定了定时长短，修改这两个值可修改定时时长
*/
void PWM_Init(uint16_t arr, uint16_t duty){
	
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    TIM_DeInit(TIM4);  // 复位定时器4
    // 决定定时时间 定时器的时钟是内部时钟，系统主频72MHz
    // 定时频率 = 72M / (PSC + 1) / (ARR + 1)  
    // 注意PSC和ARR的取值都在0-65535之间，不能超范围
    // 设置采样频率为1KHz时，arr = 1000  向上计数1000次为一个PWM周期
    TIM_TimeBaseStructure.TIM_Period = arr - 1;  					// ARR自动重装载寄存器的值
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;     				// PSC预分频器的值

    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  		// 采样分频 不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	// 计数器模式：向上计数
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // 当采样频率设置为1000Hz时，为保证CONVST低电平持续时间大于25ns，可以设置占空比为999，刚刚配置了arr = 1000，
    // 即一个周期被分成了1000个小段，最后一个小段反转成低电平。低电平的时间为：1/1000/1000 = 1us > 25ns
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				// TIM的PWM1模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = duty - 1;  						// 设置占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  		// TIM输出比较极性：高
    // PWM波形由TIM_OCMode和TIM_OCPolarity决定

    TIM_OC1Init(TIM4, &TIM_OCInitStructure);  						// 选用的是TIM4的CH1通道

    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  				// 使能定时器TIM4的CCR3上的预装载寄存器
    TIM_ARRPreloadConfig(TIM4, ENABLE);  							// 使能定时器TIM4在ARR上的预装载寄存器
    TIM_Cmd(TIM4, ENABLE);  										// 使能定时器4
 
}
