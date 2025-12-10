/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  
**  功能描述 :  
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

/****************************************************************************\
                               Includes
\****************************************************************************/
#include "mk_filter.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/

/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/**
* \brief 中值滤波
* \param[in] Array：需要排序的数组 / ArraySize:数组成员个数
* \return 滤波值
*/
uint16_t mk_filter_average(uint16_t *Array, uint16_t ArraySize)
{
    uint16_t j, k;

    if (Array == 0)
    {
        return 0;
    }

    for (j = 0; j < ArraySize - 1; j++)
    {
        for (k = 0; k < ArraySize - j - 1; k++)
        {
            if (Array[k] > Array[k + 1])
            {
                uint16_t temp = Array[k];
                Array[k]      = Array[k + 1];
                Array[k + 1]  = temp;
            }
        }
    }

    return (Array[(ArraySize - 1) >> 1]);
}

/**
* \brief 选择排序算法的滤波函数
* \param[in] Array：需要排序的数组 / ArraySize:数组成员个数, 需大于4
* \return NONE
*/
uint32_t mk_filter_selectionSort(int *Array, uint8_t ArraySize)
{
    uint8_t  i, j;
    uint8_t  LostTimes = 2;
    uint32_t Tmp       = 0;

    if ((Array == 0) && (ArraySize <= 4))
    {
        return 0;
    }

    for (i = 0; i < ArraySize - 1; i++) //升序排序
    {
        for (j = i + 1; j < ArraySize; j++)
        {
            if (Array[i] > Array[j])
            {
                Tmp      = Array[i];
                Array[i] = Array[j];
                Array[j] = Tmp;
            }
        }
    }

    Tmp = 0;
    for (i = LostTimes; i < (ArraySize - LostTimes); i++) //去掉头尾各LostTimes，中间取平均
    {
        Tmp += Array[i];
    }
    Tmp = (Tmp / (ArraySize - 2 * LostTimes));

    return Tmp;
}

/**
* \brief 卡尔曼滤波
* \param[in] 卡尔曼滤波参数结构体指针 TS_Kalman_Param*
* \return 滤波后的值
*/
double mk_filter_kalman(TS_Kalman_Param *pParam)
{
    //Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
    //R:测量噪声，R增大，动态响应变慢，收敛稳定性变好
    double R = pParam->R_Value;
    double Q = pParam->Q_Value;

    double x_mid = pParam->x_last;
    double x_now;

    double p_mid;
    double p_now;
    double kg;

    x_mid = pParam->x_last;                         //x_last=x(k-1|k-1),x_mid=x(k|k-1)
    p_mid = pParam->p_last + Q;                     //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声
    kg    = p_mid / (p_mid + R);                    //kg为kalman filter，R为噪声
    x_now = x_mid + kg * (pParam->resData - x_mid); //估计出的最优值

    p_now = (1 - kg) * p_mid; //最优值对应的covariance

    pParam->p_last = p_now; //更新covariance值
    pParam->x_last = x_now; //更新系统状态值

    return x_now;
}
/******************************* End of File (C）****************************/
