//-------------------------------------------------------------------------------------------------------------------
//  简介:八邻域图像处理

//------------------------------------------------------------------------------------------------------------------
#include "image.h"
#include "SEEKFREE_MT9V03X.h"
#include "math.h"

/*
函数名称：int my_abs(int value)
功能说明：求绝对值
参数说明：
函数返回：绝对值
修改时间：2022年9月8日
备    注：
example：  my_abs( x)；
 */
int my_abs(int value)
{
  if (value >= 0)
    return value;
  else
    return -value;
}

static int16_t limit_a_b(int16_t x, int a, int b)
{
  if (x < a)
    x = a;
  if (x > b)
    x = b;
  return x;
}

/*
函数名称：int16 limit(int16_t x, int16_t y)
功能说明：求x,y中的最小值
参数说明：
函数返回：返回两值中的最小值
修改时间：2022年9月8日
备    注：
example：  limit( x,  y)
 */
int16_t limit1(int16_t x, int16_t y)
{
  if (x > y)
    return y;
  else if (x < -y)
    return -y;
  else
    return x;
}

/*变量声明*/
uint8_t original_image[IMAGE_HEIGHT][IMAGE_WIDTH];
uint8_t image_thereshold; // 图像分割阈值
//------------------------------------------------------------------------------------------------------------------
//  @brief      获得一副灰度图像
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
void get_image(uint8_t (*mt9v03x_image)[IMAGE_WIDTH])
{
#define COMPRESS 1 // 1就是不压缩，2就是压缩一倍
  uint8_t i = 0, j = 0, row = 0, line = 0;
  for (i = 0; i < IMAGE_HEIGHT; i += COMPRESS) //
  {
    for (j = 0; j < IMAGE_WIDTH; j += COMPRESS) //
    {
      original_image[row][line] = mt9v03x_image[i][j]; // 这里的参数填写你的摄像头采集到的图像
      line++;
    }
    line = 0;
    row++;
  }
}
//------------------------------------------------------------------------------------------------------------------
//  @brief     动态阈值
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
uint8_t OSTU_threshold(uint8_t *data, uint16_t col, uint16_t row)
{
#define GRAY_SCALE 256
  uint16_t histogram[GRAY_SCALE] = {0};

  uint16_t sum_pixel = 0;         //像素总数
  uint16_t sum_back_pixel = 0;    //前景和背景像素总数
  uint16_t sum_front_pixel = 0;

  uint32_t sum_gray = 0;          //像素灰度总和
  uint32_t sum_back_gray = 0;     //前景和背景灰度总和
  uint32_t sum_front_gray = 0;

  float omega_back = 0.0f, omega_front = 0.0f;  //前景背景像素数量占比
  float micro_back = 0.0f, micro_front = 0.0f;  //前景背景平均灰度
  float max_sigma = 0.0f, sigma = 0.0f;         //类间方差;
  uint8_t min_gray = 0, max_gray = 0;
  uint8_t threshold = 0;

  sum_pixel = row * col;
  for (uint8_t y = 0; y < row; y++) // y<Image_Height改为uint8 y =Image_Height；以便进行 行二值化
  {
    // y=Image_Height;
    for (uint8_t x = 0; x < col; x++)
    {
      histogram[(uint8_t)data[y * col + x]]++; // 统计每个灰度值的个数信息
    }
  }

  for (min_gray = 0; min_gray < 256 && histogram[min_gray] == 0; min_gray++)
    ; // 获取最小灰度的值
  for (max_gray = 255; max_gray > min_gray && histogram[max_gray] == 0; max_gray--)
    ; // 获取最大灰度的值

  if (max_gray == min_gray)
  {
    return max_gray; // 图像中只有一个颜色
  }
  if (min_gray + 1 == max_gray)
  {
    return min_gray; // 图像中只有二个颜色
  }

  for (uint8_t i = min_gray; i < max_gray; i++)
  {
    sum_gray += histogram[i] * i; // 灰度值总数
  }

  for (uint8_t i = min_gray; i < max_gray; i++)
  {
    sum_back_pixel = sum_back_pixel + histogram[i];   // 前景像素点数
    sum_front_pixel = sum_pixel - sum_back_pixel;     // 背景像素点数
    omega_back = (float)sum_back_pixel / (float)sum_pixel;   // 前景像素百分比
    omega_front = (float)sum_front_pixel / (float)sum_pixel; // 背景像素百分比
    
    sum_back_gray += histogram[i] * i;                            // 前景灰度值
    sum_front_gray = sum_gray - sum_back_gray;                    // 背景灰度值
    micro_back = (float)sum_back_gray / (float)sum_back_pixel;    // 前景灰度百分比
    micro_front = (float)sum_front_gray / (float)sum_front_pixel; // 背景灰度百分比
    
    sigma = omega_back * omega_front * (micro_back - micro_front) * (micro_back - micro_front);
    if (sigma > max_sigma)          // 遍历最大的类间方差
    {
      max_sigma = sigma;
      threshold = (uint8_t)i;
    }
  }
  return threshold;
}
//------------------------------------------------------------------------------------------------------------------
//  @brief      图像二值化，这里用的是大津法二值化。
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
uint8_t bin_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // 图像数组
void turn_to_bin(void)
{
  uint8_t i, j;
  image_thereshold = OSTU_threshold(original_image[0], IMAGE_WIDTH, IMAGE_HEIGHT);
  //image_thereshold = limit_a_b(image_thereshold, 0, 85);
  for (i = 0; i < IMAGE_HEIGHT; i++)
  {
    for (j = 0; j < IMAGE_WIDTH; j++)
    {
      if (original_image[i][j] > image_thereshold)
        bin_image[i][j] = WHITE_PIXEL;
      else
        bin_image[i][j] = BLACK_PIXEL;
    }
  }
}

/*
函数名称：void get_start_point(uint8_t start_row)
功能说明：寻找两个边界的边界点作为八邻域循环的起始点
参数说明：输入任意行数
函数返回：无
修改时间：2022年9月8日
备    注：
example：  get_start_point(IMAGE_HEIGHT-2)
 */
uint8_t start_point_l[2] = {0}; // 左边起点的x，y值
uint8_t start_point_r[2] = {0}; // 右边起点的x，y值
uint8_t get_start_point(uint8_t start_row)
{
  uint8_t i = 0, l_found = 0, r_found = 0;
  // 清零
  start_point_l[0] = 0; // x
  start_point_l[1] = 0; // y

  start_point_r[0] = 0; // x
  start_point_r[1] = 0; // y

  // 从中间往左边，先找起点
  for (i = IMAGE_WIDTH / 2; i > BORDER_MIN; i--)
  {
    start_point_l[0] = i;         // x
    start_point_l[1] = start_row; // y
    if (bin_image[start_row][i] == 255 && bin_image[start_row][i - 1] == 0)
    {
      // printf("找到左边起点image[%d][%d]\n", start_row,i);
      l_found = 1;
      break;
    }
  }

  for (i = IMAGE_WIDTH / 2; i < BORDER_MAX; i++)
  {
    start_point_r[0] = i;         // x
    start_point_r[1] = start_row; // y
    if (bin_image[start_row][i] == 255 && bin_image[start_row][i + 1] == 0)
    {
      // printf("找到右边起点image[%d][%d]\n",start_row, i);
      r_found = 1;
      break;
    }
  }

  if (l_found && r_found)
    return 1;
  else
  {
    // printf("未找到起点\n");
    return 0;
  }
}

/*
函数名称：void search_l_r(uint16_t break_flag, uint8_t(*image)[IMAGE_WIDTH],uint16_t *l_stastic, uint16_t *r_stastic,
              uint8_t l_start_x, uint8_t l_start_y, uint8_t r_start_x, uint8_t r_start_y,uint8_t*hightest)

功能说明：八邻域正式开始找右边点的函数，输入参数有点多，调用的时候不要漏了，这个是左右线一次性找完。
参数说明：
break_flag_r			：最多需要循环的次数
(*image)[IMAGE_WIDTH]		：需要进行找点的图像数组，必须是二值图,填入数组名称即可
             特别注意，不要拿宏定义名字作为输入参数，否则数据可能无法传递过来
*l_stastic				：统计左边数据，用来输入初始数组成员的序号和取出循环次数
*r_stastic				：统计右边数据，用来输入初始数组成员的序号和取出循环次数
l_start_x				：左边起点横坐标
l_start_y				：左边起点纵坐标
r_start_x				：右边起点横坐标
r_start_y				：右边起点纵坐标
hightest				：循环结束所得到的最高高度
函数返回：无
修改时间：2022年9月25日
备    注：
example：
  search_l_r((uint16_t)BORDER_LENGTH,image,&data_stastics_l, &data_stastics_r,start_point_l[0],
        start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
 */
#define BORDER_LENGTH IMAGE_HEIGHT * 3 // 定义找点的数组成员个数按理说300个点能放下，但是有些特殊情况确实难顶，多定义了一点

// 存放点的x，y坐标
uint8_t points_l[(uint16_t)BORDER_LENGTH][2] = {{0}}; // 左线
uint8_t points_r[(uint16_t)BORDER_LENGTH][2] = {{0}}; // 右线
uint8_t dir_r[(uint16_t)BORDER_LENGTH] = {0};         // 用来存储右边生长方向
uint8_t dir_l[(uint16_t)BORDER_LENGTH] = {0};         // 用来存储左边生长方向
uint16_t data_stastics_l = 0;                  // 统计左边找到点的个数
uint16_t data_stastics_r = 0;                  // 统计右边找到点的个数
uint8_t hightest = 0;                          // 最高点
void search_l_r(uint16_t break_flag, uint8_t (*image)[IMAGE_WIDTH], uint16_t *l_stastic, uint16_t *r_stastic, uint8_t l_start_x, uint8_t l_start_y, uint8_t r_start_x, uint8_t r_start_y, uint8_t *hightest)
{

  uint8_t i = 0, j = 0;

  // 左边变量
  uint8_t search_filds_l[8][2] = {{0}};
  uint8_t index_l = 0;
  uint8_t temp_l[8][2] = {{0}};
  uint8_t center_point_l[2] = {0};
  uint16_t l_data_statics; // 统计左边
  // 定义八个邻域
  static int8_t seeds_l[8][2] = {
      {0, 1},
      {-1, 1},
      {-1, 0},
      {-1, -1},
      {0, -1},
      {1, -1},
      {1, 0},
      {1, 1},
  };
  //{-1,-1},{0,-1},{+1,-1},
  //{-1, 0},	     {+1, 0},
  //{-1,+1},{0,+1},{+1,+1},
  // 这个是顺时针

  // 右边变量
  uint8_t search_filds_r[8][2] = {{0}};
  uint8_t center_point_r[2] = {0}; // 中心坐标点
  uint8_t index_r = 0;             // 索引下标
  uint8_t temp_r[8][2] = {{0}};
  uint16_t r_data_statics; // 统计右边
  // 定义八个邻域
  static int8_t seeds_r[8][2] = {
      {0, 1},
      {1, 1},
      {1, 0},
      {1, -1},
      {0, -1},
      {-1, -1},
      {-1, 0},
      {-1, 1},
  };
  //{-1,-1},{0,-1},{+1,-1},
  //{-1, 0},	     {+1, 0},
  //{-1,+1},{0,+1},{+1,+1},
  // 这个是逆时针

  l_data_statics = *l_stastic; // 统计找到了多少个点，方便后续把点全部画出来
  r_data_statics = *r_stastic; // 统计找到了多少个点，方便后续把点全部画出来

  // 第一次更新坐标点  将找到的起点值传进来
  center_point_l[0] = l_start_x; // x
  center_point_l[1] = l_start_y; // y
  center_point_r[0] = r_start_x; // x
  center_point_r[1] = r_start_y; // y

  // 开启邻域循环
  while (break_flag--)
  {

    // 左边
    for (i = 0; i < 8; i++) // 传递8F坐标
    {
      search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0]; // x
      search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1]; // y
    }
    // 中心坐标点填充到已经找到的点内
    points_l[l_data_statics][0] = center_point_l[0]; // x
    points_l[l_data_statics][1] = center_point_l[1]; // y
    l_data_statics++;                                // 索引加一

    // 右边
    for (i = 0; i < 8; i++) // 传递8F坐标
    {
      search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0]; // x
      search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1]; // y
    }
    // 中心坐标点填充到已经找到的点内
    points_r[r_data_statics][0] = center_point_r[0]; // x
    points_r[r_data_statics][1] = center_point_r[1]; // y

    index_l = 0; // 先清零，后使用
    for (i = 0; i < 8; i++)
    {
      temp_l[i][0] = 0; // 先清零，后使用
      temp_l[i][1] = 0; // 先清零，后使用
    }

    // 左边判断
    for (i = 0; i < 8; i++)
    {
      if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0 && 
      image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)
      {
        temp_l[index_l][0] = search_filds_l[(i)][0];
        temp_l[index_l][1] = search_filds_l[(i)][1];
        index_l++;
        dir_l[l_data_statics - 1] = (i); // 记录生长方向
      }

      if (index_l)
      {
        // 更新坐标点
        center_point_l[0] = temp_l[0][0]; // x
        center_point_l[1] = temp_l[0][1]; // y
        for (j = 0; j < index_l; j++)
        {
          if (center_point_l[1] > temp_l[j][1])
          {
            center_point_l[0] = temp_l[j][0]; // x
            center_point_l[1] = temp_l[j][1]; // y
          }
        }
      }
    }
    if ((points_r[r_data_statics][0] == points_r[r_data_statics - 1][0] && points_r[r_data_statics][0] == points_r[r_data_statics - 2][0] && 
    points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1]) || 
    (points_l[l_data_statics - 1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics - 1][0] == points_l[l_data_statics - 3][0] && 
    points_l[l_data_statics - 1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics - 1][1] == points_l[l_data_statics - 3][1]))
    {
      // printf("三次进入同一个点，退出\n");
      break;
    }
    if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2 && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2))
    {
      // printf("\n左右相遇退出\n");
      *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1; // 取出最高点
      // printf("\n在y=%d处退出\n",*hightest);
      break;
    }
    if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
    {
      //printf("\n如果左边比右边高了，左边等待右边\n");
      continue; // 如果右边比左边高了，右边等待左边
    }
    if (dir_l[l_data_statics - 1] == 7 && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1])) // 左边比右边高且已经向下生长了
    {
      // printf("\n左边开始向下了，等待右边，等待中... \n");
      center_point_l[0] = points_l[l_data_statics - 1][0]; // x
      center_point_l[1] = points_l[l_data_statics - 1][1]; // y
      l_data_statics--;
    }
    r_data_statics++; // 索引加一

    index_r = 0; // 先清零，后使用
    for (i = 0; i < 8; i++)
    {
      temp_r[i][0] = 0; // 先清零，后使用
      temp_r[i][1] = 0; // 先清零，后使用
    }

    // 右边判断
    for (i = 0; i < 8; i++)
    {
      if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0 && 
      image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
      {
        temp_r[index_r][0] = search_filds_r[(i)][0];
        temp_r[index_r][1] = search_filds_r[(i)][1];
        index_r++;                       // 索引加一
        dir_r[r_data_statics - 1] = (i); // 记录生长方向
        // printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
      }
      if (index_r)
      {

        // 更新坐标点
        center_point_r[0] = temp_r[0][0]; // x
        center_point_r[1] = temp_r[0][1]; // y
        for (j = 0; j < index_r; j++)
        {
          if (center_point_r[1] > temp_r[j][1])
          {
            center_point_r[0] = temp_r[j][0]; // x
            center_point_r[1] = temp_r[j][1]; // y
          }
        }
      }
    }
  }

  // 取出循环次数
  *l_stastic = l_data_statics;
  *r_stastic = r_data_statics;
}
/*
函数名称：void get_left(uint16_t total_L)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_L	：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example： get_left(data_stastics_l );
 */
uint8_t l_border[IMAGE_HEIGHT];    // 左线数组
uint8_t r_border[IMAGE_HEIGHT];    // 右线数组
uint8_t center_line[IMAGE_HEIGHT]; // 中线数组
void get_left(uint16_t total_L)
{
  uint8_t h = 0;

  h = IMAGE_HEIGHT - 2;
  for(uint16_t i = 0; i < IMAGE_HEIGHT; i++)
  {
    center_line[i] = IMAGE_WIDTH / 2;
  }
  for(uint16_t i = 0; i < total_L; i++)
  {
    l_border[i] = BORDER_MIN;
  }
  // 左边
  for (uint16_t j = 0; j < total_L; j++)
  {
    // printf("%d\n", j);
    if (points_l[j][1] == h)
    {
      l_border[h] = points_l[j][0] + 1;
    }
    else
      continue; // 每行只取一个点，没到下一行就不记录
    h--;
    if (h == 0)
    {
      break; // 到最后一行退出
    }
  }
}
/*
函数名称：void get_right(uint16_t total_R)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_R  ：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example：get_right(data_stastics_r);
 */
void get_right(uint16_t total_R)
{
  uint8_t h = 0;
  for (uint16_t i = 0; i < IMAGE_HEIGHT; i++)
  {
    r_border[i] = BORDER_MAX; // 右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
  }
  h = IMAGE_HEIGHT - 2;
  // 右边
  for (uint16_t j = 0; j < total_R; j++)
  {
    if (points_r[j][1] == h)
    {
      r_border[h] = points_r[j][0] - 1;
    }
    else
      continue; // 每行只取一个点，没到下一行就不记录
    h--;
    if (h == 0)
      break; // 到最后一行退出
  }
}

// 定义膨胀和腐蚀的阈值区间
#define THRESHOLD_MAX 255 * 5                  // 此参数可根据自己的需求调节
#define THRESHOLD_MIN 255 * 2                  // 此参数可根据自己的需求调节
void image_filter(uint8_t (*bin_image)[IMAGE_WIDTH]) // 形态学滤波，简单来说就是膨胀和腐蚀的思想
{
  uint16_t i, j;
  uint32_t num = 0;

  for (i = 1; i < IMAGE_HEIGHT - 1; i++)
  {
    for (j = 1; j < (IMAGE_WIDTH - 1); j++)
    {
      // 统计八个方向的像素值
      num =
          bin_image[i - 1][j - 1] + bin_image[i - 1][j] + bin_image[i - 1][j + 1] + bin_image[i][j - 1] + 
          bin_image[i][j + 1] + bin_image[i + 1][j - 1] + bin_image[i + 1][j] + bin_image[i + 1][j + 1];

      if (num >= THRESHOLD_MAX && bin_image[i][j] == 0)
      {

        bin_image[i][j] = WHITE_PIXEL; // 白  可以搞成宏定义，方便更改
      }
      if (num <= THRESHOLD_MIN && bin_image[i][j] == 255)
      {

        bin_image[i][j] = BLACK_PIXEL; // 黑
      }
    }
  }
}

/*
函数名称：void image_draw_rectan(uint8_t(*image)[IMAGE_WIDTH])
功能说明：给图像画一个黑框
参数说明：uint8(*image)[IMAGE_WIDTH]	图像首地址
函数返回：无
修改时间：2022年9月8日
备    注：
example： image_draw_rectan(bin_image);
 */
void image_draw_rectan(uint8_t (*image)[IMAGE_WIDTH])
{

  uint8_t i = 0;
  for (i = 0; i < IMAGE_HEIGHT; i++)
  {
    image[i][0] = 0;
    image[i][1] = 0;
    image[i][IMAGE_WIDTH - 1] = 0;
    image[i][IMAGE_WIDTH - 2] = 0;
  }
  for (i = 0; i < IMAGE_WIDTH; i++)
  {
    image[0][i] = 0;
    image[1][i] = 0;
    // image[IMAGE_HEIGHT-1][i] = 0;
  }
}
static void slope_intercept_calc(int16_t *line, uint8_t begin, uint8_t end, float *slope, float *intercept)
{
  float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;

  static float last_slope;
  uint8_t point_cnt = 0;

  for(uint8_t i = begin; i < end; i++)
  {
    if(line[i] != -94)
    {
      point_cnt++;
    }
  }
  if(point_cnt < 10)
  {
    return;
  }

  for (uint8_t i = begin; i < end; i++)
  {
    if(line[i] != -94)
    {
      xsum += line[i];
      ysum += i;
      xysum += i * (line[i]);
      x2sum += (line[i] * line[i]);
    }
    else
    {
      continue;
    }
  }
  if (point_cnt * x2sum - xsum * xsum) // 判断除数是否为零
  {
    *slope = (point_cnt * xysum - xsum * ysum) / (point_cnt * x2sum - xsum * xsum);
    last_slope = *slope;
  }
  else
  {
    *slope = last_slope;
  }
  *intercept = ysum / point_cnt - (*slope) * xsum / point_cnt;
}

uint8_t black_line[IMAGE_HEIGHT];
float error = 0.0f;
float slope, intercept;
float calc_center_line_error(void)
{
  int16_t line[IMAGE_HEIGHT];
  error = 0.0f;
  for(uint8_t i = 0; i < IMAGE_HEIGHT; i++)
  {
    line[i] = (int16_t)black_line[IMAGE_HEIGHT - 1 - i] - (IMAGE_WIDTH / 2);
  }
  slope_intercept_calc(line, 5, 25, &slope, &intercept);
  // error = (float)atan((double)slope) * 180.0f / 3.1415926f;
  // if(error < 0)
  // {
  //   error = -90.0f - error;
  // }
  // else if(error > 0)
  // {
  //   error = 90.0f - error;
  // }
  if(slope != 0)
  {
    error = (35 - intercept) / slope;
  }
  else
  {
    error = 0;
  }
  return error;
}

void get_black_line(uint8_t (*bin_image)[IMAGE_WIDTH])
{
  uint8_t white_pixel_cnt = 0;
  uint8_t black_line_l = 0, black_line_r = 0;
  for(uint8_t i = 0; i < IMAGE_HEIGHT; i++)
  {
    black_line[i] = IMAGE_WIDTH / 2;
  }
  for(uint8_t i = IMAGE_HEIGHT - 1; i > (uint8_t)(IMAGE_HEIGHT * 1.0 / 4.0); i--)
  {
    for(uint8_t j = 0; j < IMAGE_WIDTH; j++)
    {
      if(bin_image[i][j] == 255)
      {
        white_pixel_cnt++;
      }
      if(bin_image[i][j] == 0 && white_pixel_cnt >= 5)
      {
        white_pixel_cnt = 0;
        black_line_l = j;
        break;
      }
    }
    white_pixel_cnt = 0;
    for(uint8_t j = IMAGE_WIDTH - 1; j > 0; j--)
    {
      if(bin_image[i][j] == 255)
      {
        white_pixel_cnt++;
      }
      if(bin_image[i][j] == 0 && white_pixel_cnt >= 5)
      {
        white_pixel_cnt = 0;
        black_line_r = j;
        break;
      }
    }
    if(black_line_l >= IMAGE_WIDTH - 2 || black_line_r <= 1)
    {
      black_line[i] = 0;
    }
    else
    {
      black_line[i] = (black_line_l + black_line_r) / 2;
    }
  }
  for(uint8_t i = IMAGE_HEIGHT - 1; i > (uint8_t)(IMAGE_HEIGHT * 1.0 / 4.0); i--)
  {
    if((black_line[i] - black_line[i-1] > 5 || black_line[i] - black_line[i-1] < -5) && black_line[i] != 0)
    {
      black_line[i-1] = black_line[i];
    }
  }
}

/*
函数名称：void image_process(void)
功能说明：最终处理函数
参数说明：无
函数返回：无
修改时间：2022年9月8日
备    注：
example： image_process();
 */
void image_process(void)
{
  uint16_t i;
  uint8_t hightest = 0; // 定义一个最高行，tip：这里的最高指的是y值的最小
  /*这是离线调试用的*/
  get_image(mt9v03x_image);
  turn_to_bin();
  /*提取赛道边界*/
  image_filter(bin_image);      // 滤波
  image_draw_rectan(bin_image); // 预处理
  // 清零
  // data_stastics_l = 0;
  // data_stastics_r = 0;
  // if (get_start_point(IMAGE_HEIGHT - 2)) // 找到起点了，再执行八领域，没找到就一直找
  // {
  //   //printf("正在开始八领域\n");
  //   search_l_r((uint16_t)BORDER_LENGTH, bin_image, &data_stastics_l, &data_stastics_r, start_point_l[0], start_point_l[1], start_point_r[0], start_point_r[1], &hightest);
  //   //printf("八邻域已结束\n");
  //   // 从爬取的边界线内提取边线 ， 这个才是最终有用的边线
  //   get_left(data_stastics_l);
  //   get_right(data_stastics_r);
  //   // 处理函数放这里，不要放到if外面去了，不要放到if外面去了，不要放到if外面去了，重要的事说三遍
  // }
  get_black_line(bin_image);

  // // 显示图像   改成你自己的就行 等后期足够自信了，显示关掉，显示屏挺占资源的
  // ips154_displayimage032_zoom(bin_image[0], IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT, 0, 0);

  // // 根据最终循环次数画出边界点
  // for (i = 0; i < data_stastics_l; i++)
  // {
  //   ips154_drawpoint(points_l[i][0] + 2, points_l[i][1], uesr_BLUE); // 显示起点
  // }
  // for (i = 0; i < data_stastics_r; i++)
  // {
  //   ips154_drawpoint(points_r[i][0] - 2, points_r[i][1], uesr_RED); // 显示起点
  // }

  for (i = hightest; i < IMAGE_HEIGHT; i++)
  {
    //center_line[i] = (l_border[i] + r_border[i]) >> 1; // 求中线
    bin_image[i][black_line[i]] = WHITE_PIXEL;
    if(i == IMAGE_HEIGHT - 25 || i == IMAGE_HEIGHT - 15)
    {
      for(uint8_t j = 0; j < IMAGE_WIDTH; j++)
      {
        bin_image[i][j] = BLACK_PIXEL;
      }
    }
    // // 求中线最好最后求，不管是补线还是做状态机，全程最好使用一组边线，中线最后求出，不能干扰最后的输出
    // // 当然也有多组边线的找法，但是个人感觉很繁琐，不建议
    // ips154_drawpoint(center_line[i], i, uesr_GREEN); // 显示起点 显示中线
    // ips154_drawpoint(l_border[i], i, uesr_GREEN);    // 显示起点 显示左边线
    // ips154_drawpoint(r_border[i], i, uesr_GREEN);    // 显示起点 显示右边线
  }
}

/*

这里是起点（0.0）***************——>*************x值最大
************************************************************
************************************************************
************************************************************
************************************************************
******************假如这是一副图像*************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
y值最大*******************************************(188.120)

*/
