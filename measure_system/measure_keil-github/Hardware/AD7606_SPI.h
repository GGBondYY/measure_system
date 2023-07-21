#include "stdint.h"

#ifndef __AD7606_SPI_H
#define __AD7606_SPI_H

/* 定义AD7606的SPI2 GPIO */
#define AD_SPI							 SPI2
#define AD_SPI_CLK						 RCC_APB1Periph_SPI2
// SPI2时钟SCLK PB13
#define AD_SPI_SCK_PIN                   GPIO_Pin_13
#define AD_SPI_SCK_GPIO_PORT             GPIOB
#define AD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB  // 36MHz
// SPI 输出MISO PB14
#define AD_SPI_MISO_PIN                  GPIO_Pin_14
#define AD_SPI_MISO_GPIO_PORT            GPIOB 
#define AD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB
// SPI片选 PB3  
#define AD_CS_PIN                        GPIO_Pin_3
#define AD_CS_GPIO_PORT                  GPIOB
#define AD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOB


/* 定义AD7606其他的GPIO */

// RESET引脚 PB5
#define AD_RESET_PIN                     GPIO_Pin_5
#define AD_RESET_GPIO_PORT               GPIOB
#define AD_RESET_GPIO_CLK                RCC_APB2Periph_GPIOB

// CONVST引脚 PB6 定时器PWM -> TIM4-CH1
#define AD_CONVST_PIN                    GPIO_Pin_6
#define AD_CONVST_GPIO_PORT              GPIOB		
#define AD_CONVST_GPIO_CLK               RCC_APB2Periph_GPIOB

// RANGE引脚 PB8
#define AD_RANGE_PIN                     GPIO_Pin_8
#define AD_RANGE_GPIO_PORT               GPIOB		
#define AD_RANGE_GPIO_CLK                RCC_APB2Periph_GPIOB

// OS0-2过采样设置 OS0:PA0 OS1:PA1 OS2:PA2
#define AD_OS0_PIN                     GPIO_Pin_0
#define AD_OS0_GPIO_PORT               GPIOA		
#define AD_OS0_GPIO_CLK                RCC_APB2Periph_GPIOA

#define AD_OS1_PIN                     GPIO_Pin_1
#define AD_OS1_GPIO_PORT               GPIOA		
#define AD_OS1_GPIO_CLK                RCC_APB2Periph_GPIOA

#define AD_OS2_PIN                     GPIO_Pin_2
#define AD_OS2_GPIO_PORT               GPIOA		
#define AD_OS2_GPIO_CLK                RCC_APB2Periph_GPIOA

// BUSY引脚  PA15 EXTI外部中断
#define AD_BUSY_PIN                     GPIO_Pin_15
#define AD_BUSY_GPIO_PORT               GPIOA		
#define AD_BUSY_GPIO_CLK                RCC_APB2Periph_GPIOA

/* 引脚控制 */

// 控制片选CS PB3 
#define AD_CS_LOW()     				AD_CS_GPIO_PORT->BRR = AD_CS_PIN
#define AD_CS_HIGH()     				AD_CS_GPIO_PORT->BSRR = AD_CS_PIN

// 控制复位RESET PB5
#define AD_RESET_LOW()					AD_RESET_GPIO_PORT->BRR = AD_RESET_PIN
#define AD_RESET_HIGH()					AD_RESET_GPIO_PORT->BSRR = AD_RESET_PIN

// 控制CONVST PB6
#define AD_CONVST_LOW()					AD_CONVST_GPIO_PORT->BRR = AD_CONVST_PIN
#define AD_CONVST_HIGH()				AD_CONVST_GPIO_PORT->BSRR = AD_CONVST_PIN

// 控制RANGE    PB8  
#define AD_RANGE_5V()					AD_RANGE_GPIO_PORT->BRR = AD_RANGE_PIN
#define AD_RANGE_10V()					AD_RANGE_GPIO_PORT->BSRR = AD_RANGE_PIN

// 控制OS[0:2] PA0 PA1 PA2 过采样
#define AD_OS0_0()						AD_OS0_GPIO_PORT->BRR = AD_OS0_PIN
#define AD_OS0_1()						AD_OS0_GPIO_PORT->BSRR = AD_OS0_PIN

#define AD_OS1_0()						AD_OS1_GPIO_PORT->BRR = AD_OS1_PIN
#define AD_OS1_1()						AD_OS1_GPIO_PORT->BSRR = AD_OS1_PIN

#define AD_OS2_0()						AD_OS2_GPIO_PORT->BRR = AD_OS2_PIN
#define AD_OS2_1()						AD_OS2_GPIO_PORT->BSRR = AD_OS2_PIN

/* FIFO缓冲区容量 */
#define BUFFER_SIZE		8192
/* 采集通道个数 */
#define CH_NUM			2				/* 采集2通道 */

/* 供外部调用的函数声明 */
void InitAD7606(void);						// 初始化AD7606各接口

void ad7606_SetInputRange(uint8_t range);   // 配置AD7606模拟信号量输入量程

void ad7606_Reset(void);					// 硬件复位AD7606

void ad7606_SetOS(uint8_t _ucMode);			// 设置过采样模式（硬件求平均值)

void ad7606_StartConv(void);				// 启动AD7606的ADC转换  PWM

uint16_t spi_ReadBytes(void);				// 读取AD7606的采样结果

void ad7606_ReadBytes(void);				// PWM定时调用本函数，用于读取AD转换器数据  存储在FIFO缓冲区中

void ad7606_StartRecord(uint32_t _ulFreq);  // 开始采集

void ad7606_StopRecord(void);               // 停止采集

void BUSY_EXTI_Init(void);					// BUSY中断  CONVST -> EXTI外部中断初始化

#endif
