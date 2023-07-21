#include "stdint.h"

#ifndef __AD7606_SPI_H
#define __AD7606_SPI_H
// /* 开关全局中断的宏 */
#define ENABLE_EXTI15()		EXTI->IMR |= EXTI_Line15      // 打开15的外部中断
#define DISABLE_EXTI15()	EXTI->IMR &= ~(EXTI_Line15)	  // 关闭15的外部中断

/* 定义AD7606的SPI GPIO */
#define AD_SPI							 SPI2
#define AD_SPI_CLK						 RCC_APB1Periph_SPI2
// SPI时钟SCLK PB13
#define AD_SPI_SCK_PIN                   GPIO_Pin_13
#define AD_SPI_SCK_GPIO_PORT             GPIOB
#define AD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB
// SPI 输出MISO PB14
#define AD_SPI_MISO_PIN                  GPIO_Pin_14
#define AD_SPI_MISO_GPIO_PORT            GPIOB 
#define AD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB
// SPI片选 PB3  需重定义
#define AD_CS_PIN                        GPIO_Pin_3
#define AD_CS_GPIO_PORT                  GPIOB
#define AD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOB

/* 定义AD7606其他的GPIO */

// RESET引脚 PB5
#define AD_RESET_PIN                     GPIO_Pin_5
#define AD_RESET_GPIO_PORT               GPIOB
#define AD_RESET_GPIO_CLK                RCC_APB2Periph_GPIOB

// CONVST引脚 PB6 定时器PWM
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

#define BUFFER_SIZE		8192
#define CH_NUM			2				/* 采集2通道 */

/* AD数据采集环形缓冲区 */
/* 每个样本两个字节，采集通道2 */

//#define FIFO_SIZE		1*1024*2		/* 大小不要超过48K，CPU内部RAM只有64K */
//#define FIFOBUFF_OK     1
//#define FIFOBUFF_ERR    0
//typedef struct
//{
//	uint16_t usRead;			// 当前需要读取的位置
//	uint16_t usWrite;			// 当前需要写入的位置
//	uint16_t usCount;			// 记录当前FIFO中现有的数据个数
//	uint16_t usBuf[FIFO_SIZE];  // FIFO缓冲区
//}FIFO_T;

//void FifoBuff_Init(void);  					// 初始化环形缓冲区
//uint16_t Write_FifoBuff(uint16_t data);  	// 向缓冲区写入数据
//uint16_t Read_FifoBuff(uint16_t *r_data);   // 从环形缓冲区读出一个数据


/* 供外部调用的函数声明 */
void InitAD7606(void);						// 初始化AD7606各接口
void ad7606_SetInputRange(uint8_t range);   // 配置AD7606模拟信号量输入量程
void ad7606_Reset(void);					// 硬件复位AD7606
void ad7606_SetOS(uint8_t _ucMode);			// 设置过采样模式（数字滤波，硬件求平均值)
void ad7606_StartConv(void);				// 启动AD7606的ADC转换
uint16_t spi_ReadBytes(void);				// 读取AD7606的采样结果
void ad7606_ReadBytes(void);				// 定时调用本函数，用于读取AD转换器数据  存储在FIFO缓冲区中
void ad7606_StartRecord(uint32_t _ulFreq);  // 开始采集
void ad7606_StopRecord(void);               // 停止采集
void BUSY_EXTI_Init(void);					// BUSY中断

#endif
