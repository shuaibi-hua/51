#ifndef __DS18B20_ONE_H__
#define __DS18B20_ONE_H__

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>

sbit DQ_Pin = P1^7;                     //DS18B20的数据口位P1.7
bit DQ_Err_Flag = 0;

unsigned char xdata DS18B20_Vstr[6] = {0};

void DS18B20_Reset();
void DS18B20_WriteByte(unsigned char dat);
unsigned char DS18B20_ReadByte();

/**************************************
由于DS18B20延时均以15us为单位，故编写了延时单位为15us的延时函数，
注意：以下延时函数晶振为12MHz
参数:n 延时的微妙数
返回值:空
**************************************/
void DS18B20_Delay60us()
{
    unsigned char i;

    i = 25;
    while (--i);
}
void DS18B20_Delay240us()		//@11.0592MHz
{
    unsigned char i;

    i = 108;
    while (--i);
}

void DS18B20_Delay480us()		//@11.0592MHz
{
    unsigned char i;

    _nop_();
    i = 218;
    while (--i);
}

/**************************************
复位DS18B20,并检测设备是否存在
参数:空
返回值:空
**************************************/
void DS18B20_Reset()
{
    unsigned char i = 0;

    CY = 1;
    while (CY)
    {
        DQ_Pin = 0;                     //送出低电平复位信号
        DS18B20_Delay480us();              //延时至少480us
        DQ_Pin = 1;                     //释放数据线
        DS18B20_Delay60us();       //等待60us
        CY = DQ_Pin;                    //检测存在脉冲
        DS18B20_Delay240us();      //等待设备释放数据线
        if (CY == 1 && i ++ > 15)
        {
            DQ_Err_Flag = 1;
            break;
        }
    }
}

/**************************************
从DS18B20读1字节数据
参数:空
返回值:返回从传感读到的数据
**************************************/
unsigned char DS18B20_ReadByte()
{
    unsigned char i;
    unsigned char dat = 0;

    for (i=0; i<8; i++)             //8位计数器
    {
        dat >>= 1;
        DQ_Pin = 0;                     //开始时间片
        _nop_();                //延时等待
        DQ_Pin = 1;                     //准备接收
        _nop_();                //接收延时
        if (DQ_Pin) dat |= 0x80;        //读取数据  
        DS18B20_Delay60us();               //等待时间片结束
    }

    return dat;
}

/**************************************
向DS18B20写1字节数据
参数:发送的数据
返回值:空
**************************************/
void DS18B20_WriteByte(unsigned char dat)
{
    char i;

    for (i=0; i<8; i++)             //8位计数器
    {
        DQ_Pin = 0;                     //开始时间片
        _nop_();                //延时等待
        dat >>= 1;                  //送出数据
        DQ_Pin = CY;
        DS18B20_Delay60us();       //等待时间片结束
        DQ_Pin = 1;                     //恢复数据线
        _nop_();                //恢复延时
    }
}
/**************************************
从DS18B20中获取温度值得浮点值
参数: 空
返回值:  读取到的温度值(有效范围-55.0~125.0)
**************************************/
float DS18B20_TF(void)
{
    unsigned char TPH;							//存放温度值的高字节
    unsigned char TPL;							//存放温度值的低字节
    short i16=0;
    float f32=0;

    DQ_Err_Flag = 0;

    DS18B20_Reset();                //设备复位
    if (DQ_Err_Flag)
    {
        return 0xffff;
    }
    DS18B20_WriteByte(0xCC);  //跳过ROM命令
    DS18B20_WriteByte(0x44);   //开始转换命令
    while (!DQ_Pin);                    //等待转换完成
    DS18B20_Reset();                //设备复位
    DS18B20_WriteByte(0xCC);        //跳过ROM命令
    DS18B20_WriteByte(0xBE);        //读暂存存储器命令
    TPL = DS18B20_ReadByte();       //读温度低字节
    TPH = DS18B20_ReadByte();       //读温度高字节
    i16 = 0;
    i16 = (TPH<<8) |TPL;				// 将高位(MSB)与低位(LSB)合并
    f32 = i16 * 0.0625;				// 12bit精度时温度值计算
    return(f32);					// 返回读取到的温度数值(float型)
}

/**************************************
 从DS18B20中读取温度数据并转换为ASCII字符串
参数:空
返回值:温度值的字符串首地址指针
**************************************/
unsigned char * DS18B20_TS(void)
{
    short t = DS18B20_TF() * 10;
    unsigned char i = 0;

    if(DQ_Err_Flag)
    {
        return NULL;
    }
    if (t < 0)
    {
        DS18B20_Vstr[i ++ ]='-';
    }
    if (t < 0)
    {
        t =  ~( -- t);//取负数的绝对值
    }
    if (t / 1000 > 0)
    {
        DS18B20_Vstr[i ++ ] = t / 1000 + 0x30;
    }
    if (t % 1000 / 100 > 0)
    {
        DS18B20_Vstr[i ++ ] = t % 1000 / 100 + 0x30;
    }

    DS18B20_Vstr[i ++ ] = t % 100 / 10 + 0x30;
    DS18B20_Vstr[i ++ ] = '.';

    DS18B20_Vstr[i ++ ] = t % 10 + 0x30;
    DS18B20_Vstr[i ++ ] = 0;
    return DS18B20_Vstr;
}

#endif /*__DS18B20_ONE_H__*/

