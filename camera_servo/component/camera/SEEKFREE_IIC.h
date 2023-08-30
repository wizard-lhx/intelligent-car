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



#ifndef _SEEKFREE_IIC_h
#define _SEEKFREE_IIC_h

#include "main.h"

#include "usart.h"

typedef enum IIC       
{
    SIMIIC,
    SCCB
} IIC_type;



void  simiic_start(void);
void  simiic_stop(void);
void  simiic_ack_main(uint8_t ack_main);
void  send_ch(uint8_t c);
uint8_t read_ch(uint8_t ack);
void  simiic_write_reg(uint8_t dev_add, uint8_t reg, uint8_t dat);
uint8_t simiic_read_reg(uint8_t dev_add, uint8_t reg, IIC_type type);
void  simiic_read_regs(uint8_t dev_add, uint8_t reg, uint8_t *dat_add, uint8_t num, IIC_type type);
void  simiic_init(void);

void soft_iic_sccb_write_register(unsigned short int dev_add, unsigned short int reg, unsigned short int dat);
unsigned short int soft_iic_sccb_read_register(unsigned short int dev_add, unsigned short int reg);









#endif

