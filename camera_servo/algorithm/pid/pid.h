#ifndef __PID_H__
#define __PID_H__

#include "main.h"

typedef struct
{
	float kp;
	float ki;
	float kd;
	
	float error;
	float last_error;
	
	float pout;
	float iout;
	float dout;
	float out;
	
	float max_out;
	float max_iout;
} pid_t;

extern pid_t angle_pid;

void PID_Init(pid_t *pid, float kp, float ki, float kd, float max_iout, float max_out);
float PID_Cal(pid_t *pid, float set, float ref);

#endif
