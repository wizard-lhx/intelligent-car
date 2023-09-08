/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897(已满)  三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		总钻风
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		MDK 5.24
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-01-03
 * @note
					摄像头接线定义：
					------------------------------------
					模块管脚            单片机管脚
					TX                 UART2_RX_A3
					RX                 UART2_TX_A2
					VSY(场中断)         A0
					PCLK(像素中断)      E11
					HREF(行中断)		不需要
					D0-D7(数据口)       B0-B7
					------------------------------------
					默认分辨率       	160*120
					默认FPS             50帧
					------------------------------------
 ********************************************************************************************************************/

#ifndef _SEEKFREE_MT9V03X_H
#define _SEEKFREE_MT9V03X_H


#include "main.h"


//--------------------------------------------------------------------------------------------------
// 不带MCU的摄像头
//--------------------------------------------------------------------------------------------------
#define SOFT_IIC_SCL_PIN		(SIM_I2C1_SCL_Pin)
#define SOFT_IIC_SCL_PORT		(SIM_I2C1_SCL_GPIO_Port)
#define SOFT_IIC_SDA_PIN		(SIM_I2C1_SDA_Pin)
#define SOFT_IIC_SDA_PORT		(SIM_I2C1_SDA_GPIO_Port)
#define SOFT_IIC_DELAY			(100)											// IIC通信失败，可以尝试增加延时



//--------------------------------------------------------------------------------------------------
//        	摄像头数据接口
//--------------------------------------------------------------------------------------------------

//#define MT9V032_DATAPORT 		(uint32_t)&GPIOB->IDR	                		// DMA数据口，禁止随意更改

//#define MT9V032_DMA_CH          &hdma_tim1_ch4_trig_com                     	// 定义摄像头的DMA采集通道，禁止随意更改

//--------------------------------------------------------------------------------------------------
// 摄像头默认参数配置 在此修改摄像头配置
//--------------------------------------------------------------------------------------------------
#define MT9V03X_W               ( 188 )                                         // 图像宽度     范围 [1-752]
#define MT9V03X_H               ( 60 )                                         // 图像高度     范围 [1-480]
#define MT9V03X_IMAGE_SIZE      ( MT9V03X_W * MT9V03X_H )                       // 整体图像大小不能超过 65535

#define MT9V03X_AUTO_EXP_DEF    ( 30   )                                         // 自动曝光设置     默认不开启自动曝光设置  范围 [0-63] 0为关闭
                                                                                //                  如果自动曝光开启  EXP_TIME命令设置自动曝光时间的上限
                                                                                //                  一般情况是不需要开启自动曝光设置 如果遇到光线非常不均匀的情况可以尝试设置自动曝光，增加图像稳定性
#define MT9V03X_EXP_TIME_DEF    ( 512 )                                         // 曝光时间         摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
#define MT9V03X_FPS_DEF         ( 50  )                                         // 图像帧率         摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
#define MT9V03X_LR_OFFSET_DEF   ( 0   )                                         // 图像左右偏移量   正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移
                                                                                //                  摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
#define MT9V03X_UD_OFFSET_DEF   ( -50   ) // 图像上下偏移量   正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移
                                                                                //                  摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
#define MT9V03X_GAIN_DEF        ( 32  )                                         // 图像增益         范围 [16-64]  增益可以在曝光时间固定的情况下改变图像亮暗程度
#define MT9V03X_PCLK_MODE_DEF   ( 1   )                                         // 像素时钟模式     范围 [0-1]    默认：0 可选参数为：[0：不输出消隐信号,1：输出消隐信号]
                                                                                //                  通常都设置为0，如果使用CH32V307的DVP接口或STM32的DCMI接口采集需要设置为1
            
#define MT9V03X_INIT_TIMEOUT    ( 0x0080 )         								// 默认的摄像头初始化超时时间 毫秒为单位
			

// 摄像头命令枚举
typedef enum
{
    MT9V03X_INIT = 0,                                                           // 摄像头初始化命令
    MT9V03X_AUTO_EXP,                                                           // 自动曝光命令
    MT9V03X_EXP_TIME,                                                           // 曝光时间命令
    MT9V03X_FPS,                                                                // 摄像头帧率命令
    MT9V03X_SET_COL,                                                            // 图像列命令
    MT9V03X_SET_ROW,                                                            // 图像行命令
    MT9V03X_LR_OFFSET,                                                          // 图像左右偏移命令
    MT9V03X_UD_OFFSET,                                                          // 图像上下偏移命令
    MT9V03X_GAIN,                                                               // 图像偏移命令
    MT9V03X_PCLK_MODE,                                                          // 像素时钟模式命令(仅总钻风MT9V034 V1.5以及以上版本支持该命令)
    MT9V03X_CONFIG_FINISH,                                                      // 非命令位，主要用来占位计数

    MT9V03X_COLOR_GET_WHO_AM_I = 0xEF,
    MT9V03X_SET_EXP_TIME = 0XF0,                                                // 单独设置曝光时间命令
    MT9V03X_GET_STATUS,                                                         // 获取摄像头配置命令
    MT9V03X_GET_VERSION,                                                        // 固件版本号命令

    MT9V03X_SET_ADDR = 0XFE,                                                    // 寄存器地址命令
    MT9V03X_SET_DATA                                                            // 寄存器数据命令
}m9v03x_cmd_enum;

extern volatile uint8_t   mt9v03x_finish_flag;                                            // 一场图像采集完成标志位
extern uint8_t    mt9v03x_image[MT9V03X_H][MT9V03X_W];
extern uint8_t tx_image_buf[65536];
extern uint16_t tx_buf_count;
extern uint16_t tx_count;


uint8_t       mt9v03x_init                (void);

void seekfree_sendimg_mt9v03x(UART_HandleTypeDef* huart, void *imgaddr, uint32_t imgsize);

#endif
