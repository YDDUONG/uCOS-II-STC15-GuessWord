#include <stdio.h>                   //sprintf()
#include <stdlib.h>                  //�����
#include <IAP15W4K61S4.h>

typedef unsigned char uChar8;

/**************************************����ȫ�ֱ���***********************************************************************/	
sbit LED = P5^5;                     //LED��
static int ANS[5], i, j, grade = 0;  //ANS[]�洢ÿһ�ֵĴ𰸣�grade��ʾ�÷֣�i, jΪѭ������
static int KEY[5] = {0, 0, 0, 0, 0}; //KEY[]�洢�û�����Ĵ�
uChar8 sign = 0;                     //sign��ʾ������±꣬ͨ��sign��һ���һ��ʵ�������ƶ�
uChar8 string[20];                   //Ҫ��ӡ����Ļ�ϵ���Ϣ����ʱȫ�ֱ���
uChar8 interface = 0;                //һ����������, interface = 0��ʾ������Ӧ��ʼ����Ĳ�����= 1��ʾ������Ӧ��Ϸҳ�棻 = 2��ʾ��Ӧ��Ϸ��������
uChar8 startUnderLineId = 0;         //startUnderLineId = 0 ��ʾ�»�����"��ʼ"����= 1 ��ʾ�»�����"�˳�"��
uChar8 gameModel = 0 ;               //gameModel = 0 ��ʾ��ǰ���ڲ����֣�= 1��ʾ����ĸ

/**************************************�����ⲿ�����ӿ�************************************************************************/
extern void startInterface(void);                                //��ʼ����
extern void gameInterface(void);                                 //��Ϸ����
extern void LCD12864ShowString(uChar8 x, uChar8 y, uChar8* str); //��y�е�x�У���ʼ��ӡstr
extern void LCD12864EraseArea(uChar8 x, uChar8 y, uChar8 size);  //�ӵ�y�е�x�п�ʼ��ɾ��size�ַ�
extern void LCD12864EraseAll(void);                              //����������ʾ��
extern void img_line_start1(void);                               //����ʼ����"��ʼ"�����»���
extern void img_dele_line_start1(void);                          //ɾ����ʼ����"��ʼ"�����»���
extern void img_line_start2(void);                               //����ʼ����"�˳�"�����»���
extern void img_dele_line_start2(void);                          //ɾ����ʼ����"�˳�"�����»���
extern void img_clear(void);                                     //��ͼ����
extern void startUnderLine(uChar8 id);
extern void underLine(uChar8 id);                                //������Ϸ������»��ߣ����������±�
extern void img_dele_underLine(void);                            //ɾ�������»���
extern void display(void);                                       //����ʱ��ʾ����
extern uChar8 isStudentInterface;                                //����LCD12864Task���Ƿ�ִ���Ƕκ�����������ϵͳ����Ϣ���ݻ���
extern int time;                                                 //ʱ��


/**************************************��������*******************************************************************************/
void KeyAction(uChar8 KeyValue);     //������������ 
void up(void);                       //����
void down(void);                     //����
void enterGame(void);                //ȷ�ϼ���������Ϸ

void add0(void);                     //���ּӼ�����ʾ
void sub0(void);
void add1(void);                     //��ĸ�Ӽ�����ʾ
void sub1(void);
void enterCmp(void);                 //�Ƚ��û��ش��Ƿ���ȷ
void left(void);                     //�»�������
void right(void);                    //�»�������
void dele0(void);                    //ɾ������������ǰλ��0
void dele1(void);                    //ɾ������������ǰλ��A
void mode(void);                     //�����ֺͲ���ĸģʽ�л�
void esc(void);
void clear0(void);                   //������λ����0
void clear1(void);                   //������λ����A

void randANS(void);                  //�����
void steTime(void);                  //���ü�ʱ��
	

//������������
void KeyAction(uChar8 KeyValue) 
{
	if(interface == 0)               //��ʼ����İ�������
	{
		switch(KeyValue) 
		{
			case 0x31: up(); break;
			case 0x32: down(); break;
			case 0x33: enterGame(); break;
			default: break;
		}
	}
	
	if(interface == 1)               //��Ϸ����İ�������
	{
		if(gameModel == 0)           //�����ְ�������
		{
			switch(KeyValue) 
			{
				case 0x31: add0(); break;
				case 0x32: sub0(); break;
				case 0x33: enterCmp(); break;
				case 0x34: left(); break;
				case 0x35: right(); break;
				case 0x36: dele0(); break;
				case 0x37: mode(); break;
				case 0x38: esc(); break;
				case 0x39: clear0(); break;
				default: break;
			}
		}
		else                         //����ĸ��������
		{
			switch(KeyValue)
			{
				case 0x31: add1(); break;
				case 0x32: sub1(); break;
				case 0x33: enterCmp(); break;
				case 0x34: left(); break;
				case 0x35: right(); break;
				case 0x36: dele1(); break;
				case 0x37: mode(); break;
				case 0x38: esc(); break;
				case 0x39: clear1(); break;
				default: break;
			}
		}
		
	}
	
	if(interface == 2)               //��Ϸ��������İ�������
	{
		switch(KeyValue) 
		{
			case 0x38: esc(); break;
			default: break;
		}		
	}
}

/************************************��ʼ���水��*****************************************************************************/
void up(void)
{
//	LED = !LED;
	if(startUnderLineId == 0)
	{
		startUnderLineId = 0;
	}
	else
	{
		startUnderLineId--;
	}
	
	startUnderLine(startUnderLineId);
}
void down(void)
{
//	LED = !LED;
	if(startUnderLineId == 2)
	{
		startUnderLineId = 2;
	}
	else
	{
		startUnderLineId++;
	}

	startUnderLine(startUnderLineId);
}

void enterGame(void)
{
//	LED = !LED;
	if(startUnderLineId == 0)
	{
		interface = 1;    //������Ϸҳ��
		randANS();
		gameInterface();
	}
	if(startUnderLineId == 1) //�˳�
	{
		uChar8 tab[] = "���˳�!";
		img_clear();
		LCD12864ShowString(2, 1, tab);
		//while(1);
		startUnderLineId = 0;
		interface = 2;
	}
	
	if(startUnderLineId == 2) //�˳�
	{
		isStudentInterface = 1;
		startUnderLineId = 0;
		interface = 2;
	}
}

/************************************�����ְ������ܺ���*****************************************************************************/
void add0(void)
{
//	LED = !LED;
	KEY[sign]++;
	if(KEY[sign] > 9) KEY[sign] = 0;
	sprintf(string, "    %d %d %d %d %d", KEY[0], KEY[1], KEY[2], KEY[3], KEY[4]);
	LCD12864EraseArea(0, 1, 16);
	LCD12864ShowString(0, 1, string);
}

void sub0(void)
{
//	LED = !LED;
	KEY[sign]--;
	if(KEY[sign] < 0) KEY[sign] = 9;
	sprintf(string, "    %d %d %d %d %d", KEY[0], KEY[1], KEY[2], KEY[3], KEY[4]);
	LCD12864EraseArea(0, 1, 16);
	LCD12864ShowString(0, 1, string);
}

void left(void)
{
//	LED = !LED;
	if(sign == 4) sign = 0;
	else sign++;
	underLine(sign);
}

void right(void)
{
//	LED = !LED;
	if(sign == 0) sign = 4;
	else sign--;
	underLine(sign);
}

void dele0(void)
{
//	LED = !LED;
	KEY[sign] = 0;
	sprintf(string, "    %d %d %d %d %d", KEY[0], KEY[1], KEY[2], KEY[3], KEY[4]);
	LCD12864EraseArea(0, 1, 16);
	LCD12864ShowString(0, 1, string);
}

void mode(void)
{
//	LED = !LED;
	IE2 = 0x00;    //�رն�ʱ��2
	switch(gameModel)
	{
		case 0:
			gameModel = 1;
			clear1();
			break;
		case 1:
			gameModel = 0;
			clear0();
			break;		
	}
	randANS();
	steTime();
}

void esc(void)
{
//	LED = !LED;
	interface = 0;
	isStudentInterface = 0;     //�ر�����2
	IE2 = 0x00;                 //�رն�ʱ��2
	LCD12864EraseAll();
	startInterface();
}

void clear0(void)
{
//	LED = !LED;
	KEY[0] = 0;
	KEY[1] = 0;
	KEY[2] = 0;
	KEY[3] = 0;
	KEY[4] = 0;
	sprintf(string, "    %d %d %d %d %d", KEY[0], KEY[1], KEY[2], KEY[3], KEY[4]);
	LCD12864EraseArea(0, 1, 16);
	LCD12864ShowString(0, 1, string);
}


/************************************����ĸ��������*****************************************************************************/
void add1(void)
{
//	LED = !LED;
	KEY[sign]++;
	if(KEY[sign] > 25) KEY[sign] = 0;
	sprintf(string, "    %c %c %c %c %c", (uChar8)(KEY[0] + 65), (uChar8)(KEY[1] + 65), (uChar8)(KEY[2] + 65), (uChar8)(KEY[3] + 65), (uChar8)(KEY[4] + 65));
	LCD12864EraseArea(0, 1, 16);
	LCD12864ShowString(0, 1, string);
}

void sub1(void)
{
//	LED = !LED;
	KEY[sign]--;
	if(KEY[sign] < 0) KEY[sign] = 25;
	sprintf(string, "    %c %c %c %c %c", (uChar8)(KEY[0] + 65), (uChar8)(KEY[1] + 65), (uChar8)(KEY[2] + 65), (uChar8)(KEY[3] + 65), (uChar8)(KEY[4] + 65));
	LCD12864EraseArea(0, 1, 16);
	LCD12864ShowString(0, 1, string);
}

void dele1(void)
{
//	LED = !LED;
	KEY[sign] = 0;
	LCD12864EraseArea(0, 1, 16);
	sprintf(string, "    %c %c %c %c %c", (uChar8)(KEY[0] + 65), (uChar8)(KEY[1] + 65), (uChar8)(KEY[2] + 65), (uChar8)(KEY[3] + 65), (uChar8)(KEY[4] + 65));
	LCD12864ShowString(0, 1, string);
}

void clear1(void)
{
//	LED = !LED;
	KEY[0] = 0;
	KEY[1] = 0;
	KEY[2] = 0;
	KEY[3] = 0;
	KEY[4] = 0;
	LCD12864EraseArea(0, 1, 16);
	sprintf(string, "    %c %c %c %c %c", (uChar8)(KEY[0] + 65), (uChar8)(KEY[1] + 65), (uChar8)(KEY[2] + 65), (uChar8)(KEY[3] + 65), (uChar8)(KEY[4] + 65));
	LCD12864ShowString(0, 1, string);
}

/************************************�ж���ȷ���*****************************************************************************/
void randANS(void)                  //������ɴ�
{
	if(gameModel == (uChar8)0)      //����
	{
		for(i = 0; i < 5; i++)
		{
			ANS[i] = rand() % 10;
		}
	}
	else
	{
		for(i = 0; i < 5; i++)      //��ĸ
		{
			ANS[i] = rand() % 25;
		}
	}
}

uChar8 AnsCmpKey(void)
{
	for(i = 0; i < 5; i++)
	{
		if(ANS[i] < KEY[i])         //ƫ��
		{
			return 2;
		}
		if(ANS[i] > KEY[i])         //ƫС
		{
			return 3;
		}
	}
	return 1;
}

void enterCmp(void)
{
	uChar8 str1[] = "  �����";
	uChar8 str2[] = "��������";
	uChar8 str3[] = "��̫С";
	uChar8 str4[] = "��̫��";
	uChar8 str5[] = "������Ϸ"; 

//	LED = !LED;	
	img_dele_underLine();            //ɾ���»���
	if(AnsCmpKey() == (uChar8)1)     //�ش���ȷ
	{
		IE2 = 0x00;                  //�رն�ʱ��2
		grade = grade + 10;          //ͳ�Ʒ���
		sprintf(string, "�÷�: %d", grade);
		LCD12864EraseArea(0, 3, 16);
		LCD12864ShowString(2, 3, string);
			
		LCD12864ShowString(2, 0, str1);
		for(i = 1000; i != 0; i--)   //��ʱ
		for(j = 600; j != 0; j--);
		LCD12864ShowString(2, 0, str2);
		for(i = 1000; i != 0; i--)   //��ʱ
		for(j = 200; j != 0; j--);
		LCD12864ShowString(2, 0, str5);
		
		if(gameModel == (uChar8)1)   //������Ϸ
		{
			clear1();
		}
		else
		{
			clear0();
		}
		randANS();                  //���������һ�ִ�
		steTime();                  //���ö�ʱ��
	}
	else                                    //�ش����
	{
		LCD12864EraseArea(0, 1, 16);
		if(AnsCmpKey() == (uChar8)2)
		{
			LCD12864ShowString(2, 1, str4); //��̫��
		}
		
		if(AnsCmpKey() == (uChar8)3)
		{
			LCD12864ShowString(2, 1, str3); //��̫С
		}
		
		for(i = 1000; i != 0; i--)          //��ʱ
		for(j = 100; j != 0; j--);
		
		LCD12864EraseArea(0, 1, 16);
		if(gameModel == (uChar8)0)          //������Ϸ
		{
			sprintf(string, "    %d %d %d %d %d", KEY[0], KEY[1], KEY[2], KEY[3], KEY[4]);
			LCD12864ShowString(0, 1, string);
		}
		else
		{
			sprintf(string, "    %c %c %c %c %c", (uChar8)(KEY[0] + 65), (uChar8)(KEY[1] + 65), (uChar8)(KEY[2] + 65), (uChar8)(KEY[3] + 65), (uChar8)(KEY[4] + 65));
			LCD12864ShowString(0, 1, string);
		}
	}
	underLine(sign);                        //���»��»���
}


void steTime(void)
{
	IE2 |= 0x04;    //�򿪶�ʱ��2
	time = 100;     //����ʱ��
	display();
}

 