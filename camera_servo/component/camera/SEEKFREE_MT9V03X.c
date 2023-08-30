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
void uart_write_buffer(UART_HandleTypeDef *huart, uint8_t *Data, uint32_t len)
{
  //    uart_putbuff(USARTx, Data, len);
  HAL_UART_Transmit(huart, Data, len, 0xFFFF);
}

static uint8_t rx_buff;
static uint8_t mt9v03x_uart_receive[3];
static uint8_t mt9v03x_receive_num = 0;
static volatile uint8_t mt9v03x_uart_receive_flag = 0;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      （总钻风摄像头）串口2中断事件回调函数
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//  @note       //用于配置MT9V032(总钻风摄像头)和接收总钻风摄像头数据
//-------------------------------------------------------------------------------------------------------------------
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if(huart->Instance == USART2)
//     {
//         mt9v03x_uart_receive[mt9v03x_receive_num] = rx_buff;
//         mt9v03x_receive_num++;

//         if(1==mt9v03x_receive_num && 0XA5!=mt9v03x_uart_receive[0])
// 			mt9v03x_receive_num = 0;
//         if(3 == mt9v03x_receive_num)
//         {
// 			mt9v03x_receive_num = 0;
// 			mt9v03x_uart_receive_flag = 1;
//         }
//         HAL_UART_Receive_IT(&huart2,&rx_buff,1);
//     }
// }

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取摄像头固件版本
// 参数说明     void
// 返回参数     uint16_t          0-获取错误 N-版本号
// 使用示例     mt9v03x_get_version();                          // 调用该函数前请先初始化串口
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
// uint16_t mt9v03x_get_version(void)
// {
//     uint16_t temp = 0;
//     uint8_t  uart_buffer[4];
//     uint16_t timeout_count = 0;
//     uint16_t return_value = 0;
// //    uint32_t uart_buffer_index = 0;

//     mt9v03x_uart_receive_flag = 0;

//     uart_buffer[0] = 0xA5;
//     uart_buffer[1] = MT9V03X_GET_STATUS;
//     temp = MT9V03X_GET_VERSION;
//     uart_buffer[2] = temp >> 8;
//     uart_buffer[3] = (uint8_t)temp;
//     uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

//     do
//     {
//         if(mt9v03x_uart_receive_flag)
//         {
//             return_value = mt9v03x_uart_receive[1] << 8 | mt9v03x_uart_receive[2];
//             break;
//         }
//         system_delay_ms(1);
//     }
//     while((MT9V03X_INIT_TIMEOUT > timeout_count ++));

//     return return_value;
// }

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     配置摄像头内部配置信息
// 参数说明     buff            发送配置信息的地址
// 返回参数     uint8_t           1-失败 0-成功
// 使用示例     mt9v03x_set_config(mt9v03x_set_confing_buffer);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
// static uint8_t mt9v03x_set_config(int16_t buff[MT9V03X_CONFIG_FINISH][2])
// {
//     uint8_t return_state = 1;
//     uint8_t  uart_buffer[4];
//     uint16_t temp = 0;
//     uint16_t timeout_count = 0;
//     uint32_t loop_count = 0;
// //    uint32_t uart_buffer_index = 0;

//     switch(mt9v03x_version)
//     {
// 		case 0x0230:
// 			loop_count = MT9V03X_PCLK_MODE;
// 			break;
// 		default:
// 			loop_count = MT9V03X_GAIN;
// 			break;
//     }
// 	mt9v03x_uart_receive_flag = 0;
//     // 设置参数  具体请参看问题锦集手册
//     // 开始配置摄像头并重新初始化
//     for(; MT9V03X_SET_DATA > loop_count; loop_count --)
//     {
//         uart_buffer[0] = 0xA5;
//         uart_buffer[1] = buff[loop_count][0];
//         temp = buff[loop_count][1];
//         uart_buffer[2] = temp >> 8;
//         uart_buffer[3] = (uint8_t)temp;
//         uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

//         system_delay_ms(2);
//     }

//     do
//     {
//         if(mt9v03x_uart_receive_flag)
//         {
//             return_state = 0;
//             break;
//         }
//         system_delay_ms(1);
//     }
//     while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
//     // 以上部分对摄像头配置的数据全部都会保存在摄像头上51单片机的eeprom中
//     // 利用set_exposure_time函数单独配置的曝光数据不存储在eeprom中
//     return return_state;
// }

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取摄像头内部配置信息
// 参数说明     buff            接收配置信息的地址
// 返回参数     uint8_t           1-失败 0-成功
// 使用示例     mt9v03x_get_config(mt9v03x_get_confing_buffer);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
// static uint8_t mt9v03x_get_config(int16_t buff[MT9V03X_CONFIG_FINISH - 1][2])
// {
//     uint8_t return_state = 0;
//     uint8_t  uart_buffer[4];
//     uint16_t temp = 0;
//     uint16_t timeout_count = 0;
//     uint32_t loop_count = 0;
// //    uint32_t uart_buffer_index = 0;

//     switch(mt9v03x_version)
//     {
//     case 0x0230:
//         loop_count = MT9V03X_PCLK_MODE;
//         break;
//     default:
//         loop_count = MT9V03X_GAIN;
//         break;
//     }

//     for(loop_count = loop_count - 1; 1 <= loop_count; loop_count --)
//     {
// 		mt9v03x_uart_receive_flag = 0;
//         if((0x0230 > mt9v03x_version) && (MT9V03X_PCLK_MODE == buff[loop_count][0]))
//         {
//             continue;
//         }
//         uart_buffer[0] = 0xA5;
//         uart_buffer[1] = MT9V03X_GET_STATUS;
//         temp = buff[loop_count][0];
//         uart_buffer[2] = temp >> 8;
//         uart_buffer[3] = (uint8_t)temp;
//         uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

//         timeout_count = 0;
//         do
//         {
//             if(mt9v03x_uart_receive_flag)
//             {
//                 return_state = 0;
// 				buff[loop_count][1] = mt9v03x_uart_receive[1] << 8 | mt9v03x_uart_receive[2];
//                 break;
//             }
//             system_delay_ms(1);
//         }
//         while(MT9V03X_INIT_TIMEOUT > timeout_count ++);

//         if(MT9V03X_INIT_TIMEOUT < timeout_count)                                // 超时
//         {
//             return_state = 1;
//             break;
//         }
//     }
//     return return_state;
// }

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
      // system_delay_ms(200);
      // // 初始化串口
      //       MX_USART2_UART_Init();
      // HAL_UART_MspInit(MT9V03X_COF_UART);
      // // 设置接收中断
      // HAL_UART_Receive_IT(MT9V03X_COF_UART,&rx_buff,1);

      // // 获取版本号
      //       mt9v03x_version = mt9v03x_get_version();

      //       if(mt9v03x_set_config(mt9v03x_set_confing_buffer))
      //       {
      //           // 如果程序在输出了断言信息 并且提示出错位置在这里
      //           // 那么就是串口通信出错并超时退出了
      //           // 检查一下接线有没有问题 如果没问题可能就是坏了

      //           return_state = 1;
      //           break;
      //       }

      //       // 获取配置便于查看配置是否正确
      //       if(mt9v03x_get_config(mt9v03x_get_confing_buffer))
      //       {
      //           // 如果程序在输出了断言信息 并且提示出错位置在这里
      //           // 那么就是串口通信出错并超时退出了
      //           // 检查一下接线有没有问题 如果没问题可能就是坏了

      //           return_state = 1;
      //           break;
      //       }
    }
  } while (0);

  // 使用DCMI中断
  __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);                                                          // 开启帧中断，一帧中断一次
  HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)mt9v03x_image, MT9V03X_H * MT9V03X_W / 4); // 使能DCMI_DMA，连续采集图像。

  return return_state;
}
