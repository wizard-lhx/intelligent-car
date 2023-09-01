#include "hardwareInterface.h"
#include "AnoPTv8Run.h"

//AptHwSendBytes�˺�����Ҫ�����û��Լ����豸������ʵ�֣�����ʹ�ô���������λ��������Ͷ�Ӧ�ô��ڵķ��ͺ���
//ע�⣺�����������ʹ���ж�+����������DMA+�������ķ�ʽ������ʽ���ͽ����Ӱ��ϵͳ����
//ע�⣺���ڻ�������Ӧ��С���Ƽ�256�ֽڻ�����
//���������ö�Ӧ�Ĵ���h�ļ�
#include "Drv_Uart.h"
void AnoPTv8HwSendBytes(uint8_t *buf, uint16_t len)
{
	DrvUart1SendBuf(buf, len);
}

//AptHwRecvByte�˺�������hardwareInterface.h���������û���Ҫ�ڶ�Ӧ���ڵĽ����¼��е��ô˺���
//ע�⣺�˺�����������ֽ����ݣ���������¼����յ������ݴ���1�ֽڣ���ε��ô˺�������
void AnoPTv8HwRecvByte(uint8_t dat)
{
	AnoPTv8RecvOneByte(dat);
}

//AptHwTrigger1ms�˺�������hardwareInterface.h���������û���Ҫ��1ms��ʱ�жϻ���ϵͳ�δ�����Լ���Ƶĵ�������
//��1ms��ʱ�������ô˺���
void AnoPTv8HwTrigger1ms(void)
{
	AnoPTv8TxRunThread1ms();
}
