#include <IAP15W4K61S4.h>
#include <stdio.h>

typedef unsigned char uChar8;
typedef unsigned int uInt16;

/**************************************����ȫ�ֱ���********************************************************************/	
sbit LED = P5^5;                                                 //LED��
uInt16 count = 0;                                                //������
int time = 100;                                                  //����ʱ��ʼʱ��
struct {                                                         //��ʱ����ʼֵ�ṹ��
    uChar8 TH;
    uChar8 TL;
}Timer1, Timer2;

/**************************************�����ⲿ�����ӿ�************************************************************************/
extern void LCD12864ShowString(uChar8 x, uChar8 y, uChar8* str); //��y�е�x�У���ʼ��ӡstr
extern void LCD12864EraseArea(uChar8 x, uChar8 y, uChar8 size);  //�ӵ�y�е�x�п�ʼ��ɾ��size�ַ
extern void img_dele_underLine(void);                            //ɾ�������»���
extern void keyScanner(void);                                    //����ɨ��
extern uChar8 interface;                                         //ȫ�ֱ�������ʾ��ǰ��������

/**************************************��������************************************************************************/
void configureTimer0(void) reentrant;                            //ϵͳʱ��time0����
void configureTimer1(uChar8 ms);                                 //��ʱ��1����
void TimerCounter_1 (void);                                      //��ʱ��1�жϣ�ɨ�谴��
void configureTimer2(void);                                      //��ʱ��2����
void TimerCounter_2 (void);                                      //��ʱ��1�жϣ�����ʱ
void display(void);                                              //��ʾ����ʱ


//ϵͳʱ��time0����
void configureTimer0(void) reentrant
{
    TMOD &= 0xF0;
	TMOD |= 0x01;                                                //��ʱ��0��ģʽ1(16λ��ʱ��)������TR0���ƣ���ʱ��1�������ʷ�����
//  TH0 = 0xDC;                                                  //����Tick=100��/��(��0.01��/��),TH,TLֵ��CPU��Ƶ���й�(11.0592M)
//  TL0 = 0x00;  	                                             //OS_CPU_C.C�ж�ʱ���ж���ӦҲҪ���ã�OS_CFG.H��OS_TICKS_PER_SECҲ�й�ϵ
    TR0  = 1;		                                             //��������
}

//��ʱ��1����
void configureTimer1(uChar8 ms)
{
    unsigned long timer = 11059200 / 12;
    timer = timer * ms / 1000;
    timer = 65536 - timer;
    TH1 = (uChar8)(timer >> 8);
    TL1 = (uChar8)timer;
    Timer1.TH = TH1;
    Timer1.TL = TL1;
    TMOD &= 0xF0;
    TMOD |= 0x01;
    ET1 = 1;
    TR1 = 1;
}

//��ʱ��1�ж�
void TimerCounter_1 (void) interrupt 3 
{ 
    TH1 = Timer1.TH;
    TL1 = Timer1.TL;
	keyScanner();
}


//��ʱ��2����
void configureTimer2(void)
{
	AUXR &= 0xFB;
	T2H = 0xFC;
	T2L = 0X67;
	Timer2.TH = T2H;
    Timer2.TL = T2L;
	//IE2 |= 0x04;
	AUXR |= 0x10;
}

//��ʱ��2�ж�
void TimerCounter_2 (void) interrupt 12
{ 
	T2H = Timer2.TH;
	T2L = Timer2.TL;
	count++;
	if(count == 1000)
	{
		display();
		//LED = !LED;
		count = 0;
	}
}


//��ʾ����ʱ
void display(void)
{
	uChar8 str[20];
	sprintf(str, "����ʱ: %03d", time--);
	LCD12864EraseArea(0, 2, 20);
	LCD12864ShowString(1, 2, str);
	if(time == -1)
	{
		img_dele_underLine();
		sprintf(str, "��Ϸ����");
		LCD12864EraseArea(0, 1, 20);
		LCD12864ShowString(2, 1, str);
		IE2 = 0x00;                 //�رն�ʱ��2
		interface = 2;
	}
}