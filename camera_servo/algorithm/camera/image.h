#ifndef _IMAGE_H
#define _IMAGE_H

#include "main.h"

//颜色定义  因为有先例，连颜色都改不来，我直接放这了
#define uesr_RED     0xF800    //红色
#define uesr_GREEN   0x07E0    //绿色
#define uesr_BLUE    0x001F    //蓝色


//宏定义
#define IMAGE_HEIGHT	120//图像高度
#define IMAGE_WIDTH	188//图像宽度
#define IMAGE_SIZE ((IMAGE_WIDTH) * (IMAGE_HEIGHT))

#define WHITE_PIXEL	255
#define BLACK_PIXEL	0

#define bin_jump_num	1//跳过的点数
#define BORDER_MAX	IMAGE_WIDTH-2 //边界最大值
#define BORDER_MIN	1	//边界最小值	
extern uint8_t original_image[IMAGE_HEIGHT][IMAGE_WIDTH];
extern uint8_t bin_image[IMAGE_HEIGHT][IMAGE_WIDTH];//图像数组
extern uint8_t center_line[IMAGE_HEIGHT];

extern void image_process(void); //直接在中断或循环里调用此程序就可以循环执行了

#endif /*_IMAGE_H*/

