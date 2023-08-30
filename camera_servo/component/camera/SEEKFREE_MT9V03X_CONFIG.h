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
 
#ifndef _zf_device_config_h_
#define _zf_device_config_h_

#include "SEEKFREE_IIC.h"

unsigned char   mt9v03x_set_config_sccb         (short int buff[10][2]);
unsigned char   mt9v03x_set_exposure_time_sccb  (unsigned short int light);
unsigned char   mt9v03x_set_reg_sccb            (unsigned char addr, unsigned short int data);


extern unsigned char mt9v03x_set_config_sccb (short int buff[10][2]);

#endif
