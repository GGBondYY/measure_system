#ifndef __SERIAL_H
#define __SERIAL_H
#include <stdio.h>

extern uint8_t Serial_TxPacket[];		// 发送数据包
extern uint8_t Serial_RxPacket[];		// 接收数据包


// USART引脚 PB10 PB11 定时器PWM
#define USART_TX_PIN                    GPIO_Pin_10
#define USART_RX_PIN                    GPIO_Pin_11
#define USART_GPIO_PORT                 GPIOB		
#define USART_GPIO_CLK                  RCC_APB2Periph_GPIOB
#define USART3_CLK                      RCC_APB1Periph_USART3

void Serial_Init(void);                                     				// 串口初始化
void Serial_SendByte(uint8_t Byte);											// 发送一个字节
void Serial_SendArray(uint8_t *Array, uint16_t Length);						// 发送一个数组函数
void Serial_SendPacket(uint16_t two_byte);									// 发送数据包
uint8_t Serial_GetRxFlag(void);												// 获取接收数据RXEN标志位


// USART + DMA 测试
// 串口工作参数定义
#define DEBUG_USARTx						USART3
#define DEBUG_USART_CLK						RCC_APB1Periph_USART3
#define DEBUG_USART_APBxClkCmd				RCC_APB1PeriphClockCmd
#define DEBUG_USART_BUADRATE				115200
// USART GPIO 引脚宏定义
#define DEBUG_USART_GPIO_CLK				RCC_APB2Periph_GPIOB
#define DEBUG_USART_GPIO_APBxClkCmd			RCC_APB2PeriphClockCmd

#define DEBUG_USART_TX_GPIO_PORT			GPIOB
#define DEBUG_USART_TX_GPIO_PIN				GPIO_Pin_10
#define DEBUG_USART_RX_GPIO_PORT			GPIOB
#define DEBUG_USART_RX_GPIO_PIN				GPIO_Pin_11

#define USART_TX_DMA_CLK					RCC_AHBPeriph_DMA1
#define USART_TX_DMA_CHANNEL				DMA1_Channel2
#define USART_TX_DMA_FLAG_TC 				DMA1_FLAG_TC4
// 外设寄存器地址
#define USART_DR_ADDRESS					(USART3_BASE+0x04)
#define SENDBUFF_SIZE						5000

void DEBUG_USART_Config(void);													// 串口初始化
void USARTx_DMA_Config(void);													// 配置中断通道
void DMA_Start_Channle_Transfer(DMA_Channel_TypeDef* channel, uint16_t len);	// DMA初始化
#endif
