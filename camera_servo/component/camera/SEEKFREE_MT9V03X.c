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

#include "SEEKFREE_MT9V03X_CONFIG.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_MT9V03X.h"
#include "dcmi.h"

volatile uint8_t mt9v03x_finish_flag = 0; // 一场图像采集完成标志位
__attribute__((aligned(4))) uint8_t mt9v03x_image[MT9V03X_H][MT9V03X_W];

static uint16_t mt9v03x_version = 0x00;

// 需要配置到摄像头的数据 不允许在这修改参数
static int16_t mt9v03x_set_confing_buffer[MT9V03X_CONFIG_FINISH][2] =
    {
        {MT9V03X_INIT, 0}, // 摄像头开始初始化

        {MT9V03X_AUTO_EXP, MT9V03X_AUTO_EXP_DEF},   // 自动曝光设置
        {MT9V03X_EXP_TIME, MT9V03X_EXP_TIME_DEF},   // 曝光时间
        {MT9V03X_FPS, MT9V03X_FPS_DEF},             // 图像帧率
        {MT9V03X_SET_COL, MT9V03X_W},               // 图像列数量
        {MT9V03X_SET_ROW, MT9V03X_H},               // 图像行数量
        {MT9V03X_LR_OFFSET, MT9V03X_LR_OFFSET_DEF}, // 图像左右偏移量
        {MT9V03X_UD_OFFSET, MT9V03X_UD_OFFSET_DEF}, // 图像上下偏移量
        {MT9V03X_GAIN, MT9V03X_GAIN_DEF},           // 图像增益
        {MT9V03X_PCLK_MODE, MT9V03X_PCLK_MODE_DEF}, // 像素时钟模式
};

// 从摄像头内部获取到的配置数据 不允许在这修改参数
static int16_t mt9v03x_get_confing_buffer[MT9V03X_CONFIG_FINISH - 1][2] =
    {
        {MT9V03X_AUTO_EXP, 0},  // 自动曝光设置
        {MT9V03X_EXP_TIME, 0},  // 曝光时间
        {MT9V03X_FPS, 0},       // 图像帧率
        {MT9V03X_SET_COL, 0},   // 图像列数量
        {MT9V03X_SET_ROW, 0},   // 图像行数量
        {MT9V03X_LR_OFFSET, 0}, // 图像左右偏移量
        {MT9V03X_UD_OFFSET, 0}, // 图像上下偏移量
        {MT9V03X_GAIN, 0},      // 图像增益
        {MT9V03X_PCLK_MODE, 0}, // 像素时钟模式命令 PCLK模式 < 仅总钻风 MT9V034 V1.5 以及以上版本支持该命令 >
};

//-------------------------------------------------------------------------------------------------------------------
//  @brief      MS级延时
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//  @note
//-------------------------------------------------------------------------------------------------------------------
void system_delay_ms(uint16_t ms)
{

  extern void HAL_Delay(uint32_t Delay);
  HAL_Delay(ms);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      串口发送数据
//  @param      *Data		BUFF地址
//  @param      len			数据长度
//  @return     void
//  @since      v1.0
//  Sample usage:
//  @note
//-------------------------------------------------------------------------------------------------------------------
uint8_t tx_image_buf[65536];
uint16_t tx_buf_count;
uint16_t tx_count;
void uart_write_buffer(UART_HandleTypeDef *huart, uint8_t *Data, uint32_t len)
{
  // uint8_t i;
  // for (i = 0; i < len; i++)
  // {
  //   tx_image_buf[tx_buf_count++] = Data[i];
  // }

  // if (!(USART1->CR1 & USART_CR1_TXEIE))
  // {
  //   __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);
  // }
  //HAL_UART_Transmit_IT(&huart1, Data, len);
  HAL_UART_Transmit(&huart1, Data, len, 0xfffff);
  //HAL_UART_Transmit_DMA(&huart1, Data, len);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      总钻风摄像头图像发送至上位机查看图像
//  @param      *imgaddr            压缩图像数据地址
//  @param      *imgsize            图像大小
//  @return     void
//  @since      v1.0
//  Sample usage:                   调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
void seekfree_sendimg_mt9v03x(UART_HandleTypeDef *huart, void *imgaddr, uint32_t imgsize)
{
  uint8_t dat[] = {0x00, 0xff, 0x01, 0x01};

  uart_write_buffer(huart, dat, 4);
  uart_write_buffer(huart, imgaddr, imgsize);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     MT9V03X 摄像头初始化
// 参数说明     void
// 返回参数     uint8_t           1-失败 0-成功
// 使用示例     zf_log(mt9v03x_init(), "mt9v03x init error");
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8_t mt9v03x_init(void)
{
  uint8_t return_state = 0;

  do
  {
    // 等待摄像头上电初始化成功
    system_delay_ms(1000); // 延时1秒

    // 尝试使用IIC进行初始化，如果初始化不成功，就会执行串口初始化
    simiic_init();
    // mt9v03x_set_config_sccb 返回值1代表失败，返回0代表成功
    // mt9v03x_set_config_sccb 这个函数已经被封装成lib，不对外提供。
    if (mt9v03x_set_config_sccb(mt9v03x_set_confing_buffer) == 0)
    {
      // 不带MCU的单片机初始化失败
      break;
    }
    else
    {
      
    }
  } while (0);

  // 使用DCMI中断
  __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);                                                          // 开启帧中断，一帧中断一次
  HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)mt9v03x_image, MT9V03X_H * MT9V03X_W / 4); // 使能DCMI_DMA，连续采集图像。

  return return_state;
}
