#ifndef __HARDWAREINTERFACE_H
#define __HARDWAREINTERFACE_H
#include <stdint.h>

//��ȡ����һ�ֽ����ݵ�ÿ�ֽ����ݣ�����int16��int32��float��
#define BYTE0(dwTemp) (*((char *)(&dwTemp)))
#define BYTE1(dwTemp) (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp) (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp) (*((char *)(&dwTemp) + 3))
	
#define LIMIT( x,min,max ) ( ((x) <= (min)) ? (min) : ( ((x) > (max))? (max) : (x) ) )



void AnoPTv8HwSendBytes(uint8_t *buf, uint16_t len);
void AnoPTv8HwRecvByte(uint8_t dat);
void AnoPTv8HwTrigger1ms(void);

#endif
