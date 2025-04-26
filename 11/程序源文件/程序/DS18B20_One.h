#ifndef __DS18B20_ONE_H__
#define __DS18B20_ONE_H__

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>

sbit DQ_Pin = P1^7;                     //DS18B20�����ݿ�λP1.7
bit DQ_Err_Flag = 0;

unsigned char xdata DS18B20_Vstr[6] = {0};

void DS18B20_Reset();
void DS18B20_WriteByte(unsigned char dat);
unsigned char DS18B20_ReadByte();

/**************************************
����DS18B20��ʱ����15usΪ��λ���ʱ�д����ʱ��λΪ15us����ʱ������
ע�⣺������ʱ��������Ϊ12MHz
����:n ��ʱ��΢����
����ֵ:��
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
��λDS18B20,������豸�Ƿ����
����:��
����ֵ:��
**************************************/
void DS18B20_Reset()
{
    unsigned char i = 0;

    CY = 1;
    while (CY)
    {
        DQ_Pin = 0;                     //�ͳ��͵�ƽ��λ�ź�
        DS18B20_Delay480us();              //��ʱ����480us
        DQ_Pin = 1;                     //�ͷ�������
        DS18B20_Delay60us();       //�ȴ�60us
        CY = DQ_Pin;                    //����������
        DS18B20_Delay240us();      //�ȴ��豸�ͷ�������
        if (CY == 1 && i ++ > 15)
        {
            DQ_Err_Flag = 1;
            break;
        }
    }
}

/**************************************
��DS18B20��1�ֽ�����
����:��
����ֵ:���شӴ��ж���������
**************************************/
unsigned char DS18B20_ReadByte()
{
    unsigned char i;
    unsigned char dat = 0;

    for (i=0; i<8; i++)             //8λ������
    {
        dat >>= 1;
        DQ_Pin = 0;                     //��ʼʱ��Ƭ
        _nop_();                //��ʱ�ȴ�
        DQ_Pin = 1;                     //׼������
        _nop_();                //������ʱ
        if (DQ_Pin) dat |= 0x80;        //��ȡ����  
        DS18B20_Delay60us();               //�ȴ�ʱ��Ƭ����
    }

    return dat;
}

/**************************************
��DS18B20д1�ֽ�����
����:���͵�����
����ֵ:��
**************************************/
void DS18B20_WriteByte(unsigned char dat)
{
    char i;

    for (i=0; i<8; i++)             //8λ������
    {
        DQ_Pin = 0;                     //��ʼʱ��Ƭ
        _nop_();                //��ʱ�ȴ�
        dat >>= 1;                  //�ͳ�����
        DQ_Pin = CY;
        DS18B20_Delay60us();       //�ȴ�ʱ��Ƭ����
        DQ_Pin = 1;                     //�ָ�������
        _nop_();                //�ָ���ʱ
    }
}
/**************************************
��DS18B20�л�ȡ�¶�ֵ�ø���ֵ
����: ��
����ֵ:  ��ȡ�����¶�ֵ(��Ч��Χ-55.0~125.0)
**************************************/
float DS18B20_TF(void)
{
    unsigned char TPH;							//����¶�ֵ�ĸ��ֽ�
    unsigned char TPL;							//����¶�ֵ�ĵ��ֽ�
    short i16=0;
    float f32=0;

    DQ_Err_Flag = 0;

    DS18B20_Reset();                //�豸��λ
    if (DQ_Err_Flag)
    {
        return 0xffff;
    }
    DS18B20_WriteByte(0xCC);  //����ROM����
    DS18B20_WriteByte(0x44);   //��ʼת������
    while (!DQ_Pin);                    //�ȴ�ת�����
    DS18B20_Reset();                //�豸��λ
    DS18B20_WriteByte(0xCC);        //����ROM����
    DS18B20_WriteByte(0xBE);        //���ݴ�洢������
    TPL = DS18B20_ReadByte();       //���¶ȵ��ֽ�
    TPH = DS18B20_ReadByte();       //���¶ȸ��ֽ�
    i16 = 0;
    i16 = (TPH<<8) |TPL;				// ����λ(MSB)���λ(LSB)�ϲ�
    f32 = i16 * 0.0625;				// 12bit����ʱ�¶�ֵ����
    return(f32);					// ���ض�ȡ�����¶���ֵ(float��)
}

/**************************************
 ��DS18B20�ж�ȡ�¶����ݲ�ת��ΪASCII�ַ���
����:��
����ֵ:�¶�ֵ���ַ����׵�ַָ��
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
        t =  ~( -- t);//ȡ�����ľ���ֵ
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

