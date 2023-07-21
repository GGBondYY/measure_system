#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "pwm.h"
#include "AD7606_spi.h"
#include <stdio.h>
#include <stdarg.h>
#include "delay.h"
#include <string.h>

extern uint16_t FIFO[BUFFER_SIZE];  	// 上传到上位机的缓冲区
extern uint8_t start_index;				// 判断是否可以进行数据传输标志位
extern uint16_t freq;		    		// 采样率
extern uint16_t freq_num;				// 采样点数
extern uint16_t count;					// 记录采集

int main(void)
{
	
	// 调试用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef  GPIO_InitStructure;
        // PC13复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	// GPIO_SetBits(GPIOC, GPIO_Pin_13);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	Serial_Init();  			// 串口初始化
	InitAD7606();   			// AD7606初始化
	// main函数发送的方式
	int flag = 0;
	// GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	while(1)
	{
//		GPIO_SetBits(GPIOC, GPIO_Pin_13);
//		Delay_ms(500);
//		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//		Delay_ms(500);
		if(Serial_GetRxFlag() == 1)
		{
			
			if(Serial_RxPacket[0] == 0xFA && Serial_RxPacket[6] == 0xFB){
				if(Serial_RxPacket[1] == 0xAA){
					// 获取采样频率和采样点数
					uint16_t orign_freq = (Serial_RxPacket[2] << 8) | Serial_RxPacket[3];
					freq = 1000000 / orign_freq;
					freq_num = 2 * ((Serial_RxPacket[4] << 8) | Serial_RxPacket[5]);
					flag = 1;
					start_index = 0;
					count = 0;
					memset(FIFO, 0, BUFFER_SIZE);
					ad7606_StartRecord(freq);   // 开始转换 1KHz采样率
					// GPIO_ResetBits(GPIOC, GPIO_Pin_13);
					GPIO_SetBits(GPIOC, GPIO_Pin_13);
				}
				else if(Serial_RxPacket[1] == 0xBB){
					flag = 0;					
					ad7606_StopRecord();					
					GPIO_SetBits(GPIOC, GPIO_Pin_13);
					GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				}
			}
		}
		if(flag)
		{
			if(start_index)
			{
				for(int i = 0; i < freq_num; ++i)
				{
					Serial_SendPacket(FIFO[i]);
				}
				memset(FIFO, 0, freq_num);
				start_index = 0;
				ad7606_StartRecord(freq);
			}
		}
			
	}	
		
		
}
