#ifndef __SERIAL_H
#define __SERIAL_H
#include <stdio.h>

extern uint8_t Serial_TxPacket[];		// 发送数据包
extern uint8_t Serial_RxPacket[];		// 接收数据包


// USART3引脚 PB10 PB11 定时器PWM
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

#endif
