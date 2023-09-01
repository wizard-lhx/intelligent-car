#ifndef __ANOPTV8DATADEF_H
#define __ANOPTV8DATADEF_H
#include "hardwareInterface.h"
#include "AnoPTv8Run.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
�����ṹ�嶨�壬������ö��
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	int32_t		min;
	int32_t		max;
	int32_t * 	pval;
	char		name[20];
	char		info[70];
}__attribute__ ((__packed__)) _st_par_info;

//ע�⣬���ﶨ���������������û����ӻ�ɾ��������һ��Ҫ�޸�ANOPTV8_PARNUM�궨�壬����AnoPTv8DataDef.c���޸Ķ�Ӧ��AnoParInfoList����
#define ANOPTV8_PARNUM	30

extern const _st_par_info AnoParInfoList[ANOPTV8_PARNUM];

uint16_t AnoPTv8GetParCount(void);









/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
����ṹ��
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	uint8_t cmdid0;
	uint8_t cmdid1;
	uint8_t cmdid2;
	uint8_t cmdval0;
	uint8_t cmdval1;
	uint8_t cmdval2;
	uint8_t cmdval3;
	uint8_t cmdval4;
	uint8_t cmdval5;
	uint8_t cmdval6;
	uint8_t cmdval7;
}__attribute__ ((__packed__)) _st_cmd;

typedef struct
{
	_st_cmd		cmd;
	char		name[20];
	char		info[50];
	void 		(*pFun)(const _un_frame_v8 *);
}__attribute__ ((__packed__)) _st_cmd_info;

#define ANOPTV8_CMDNUM	5

extern const _st_cmd_info AnoCmdInfoList[ANOPTV8_CMDNUM];

uint16_t AnoPTv8GetCmdCount(void);










/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
��Ҫ�������õ��ⲿ����������V8Э��֡���͵���λ����ʾ
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//0x01
#define DD_DAT_ACC_RAW_X      (0)
#define DD_DAT_ACC_RAW_Y      (0)
#define DD_DAT_ACC_RAW_Z      (0)
#define DD_DAT_GYR_RAW_X      (0)
#define DD_DAT_GYR_RAW_Y      (0)
#define DD_DAT_GYR_RAW_Z      (0)
#define DD_DAT_SHOCK_STA      (0)
//0x02
#define DD_DAT_ECP_RAW_X      (0)
#define DD_DAT_ECP_RAW_Y      (0)
#define DD_DAT_ECP_RAW_Z      (0)
#define DD_DAT_BARO_ALT       (0)
#define DD_DAT_TEMPERATURE    (0)
#define DD_DAT_BARO_STA       (0)
#define DD_DAT_ECP_STA        (0)
//0x03
#define DD_DAT_ANGLE_ROL      (0)
#define DD_DAT_ANGLE_PIT      (0)
#define DD_DAT_ANGLE_YAW      (0)
#define DD_DAT_ATT_FUSION_STA (0)
//0x04 
#define DD_DAT_QUA0_10K       (0)
#define DD_DAT_QUA1_10K       (0)
#define DD_DAT_QUA2_10K       (0)
#define DD_DAT_QUA3_10K       (0)
//0X05
#define DD_DAT_ALT_FU	       (0)
#define DD_DAT_ALT_ADD	       (0)
#define DD_DAT_ALT_STA        (0)
//0X06
#define DD_DAT_FC_MODE	       (0)
#define DD_DAT_FC_LOCKED	   (0)
#define DD_DAT_FC_CMD_ID	   (0)
#define DD_DAT_FC_CMD_0       (0)
#define DD_DAT_FC_CMD_1       (0)
//0X07
#define DD_DAT_HCA_VEL_X      (0)
#define DD_DAT_HCA_VEL_Y      (0)
#define DD_DAT_HCA_VEL_Z      (0)
//0x08
#define DD_DAT_ULHCA_POS_X      (0)
#define DD_DAT_ULHCA_POS_Y      (0)
//0X09
#define DD_DAT_HCA_WIND_X      (0)
#define DD_DAT_HCA_WIND_Y      (0)
//0X0A
#define DD_DAT_TAR_ROL         (0)
#define DD_DAT_TAR_PIT         (0)
#define DD_DAT_TAR_YAW         (0)
//0X0B
#define DD_DAT_HCA_TAR_VEL_X   (0)
#define DD_DAT_HCA_TAR_VEL_Y   (0)
#define DD_DAT_HCA_TAR_VEL_Z   (0)
//0X0D
#define DD_DAT_PWR_VOTAGE		(0)
#define DD_DAT_PWR_CURRENT		(0)
//0X0E
#define DD_DAT_SENSTA_VEL		(0)
#define DD_DAT_SENSTA_POS		(0)
#define DD_DAT_SENSTA_GPS		(0)
#define DD_DAT_SENSTA_ALT		(0)
//0x20
#define DD_DAT_PWM_1          (0)
#define DD_DAT_PWM_2          (0)
#define DD_DAT_PWM_3          (0)
#define DD_DAT_PWM_4          (0)
#define DD_DAT_PWM_5          (0)
#define DD_DAT_PWM_6          (0)
#define DD_DAT_PWM_7          (0)
#define DD_DAT_PWM_8          (0)
//0x21
#define DD_DAT_CTRL_ROL	(0)
#define DD_DAT_CTRL_PIT	(0)
#define DD_DAT_CTRL_THR	(0)
#define DD_DAT_CTRL_YAW	(0)




#endif
