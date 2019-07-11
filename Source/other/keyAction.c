#include <stdio.h>                   //sprintf()
#include <stdlib.h>                  //随机数
#include <IAP15W4K61S4.h>

typedef unsigned char uChar8;

/**************************************定义全局变量***********************************************************************/	
sbit LED = P5^5;                     //LED灯
static int ANS[5], i, j, grade = 0;  //ANS[]存储每一轮的答案，grade表示得分，i, j为循环变量
static int KEY[5] = {0, 0, 0, 0, 0}; //KEY[]存储用户输入的答案
uChar8 sign = 0;                     //sign表示数组的下标，通过sign加一或减一来实现左右移动
uChar8 string[20];                   //要打印到屏幕上的信息，临时全局变量
uChar8 interface = 0;                //一共三个界面, interface = 0表示键盘响应开始界面的操作；= 1表示按键响应游戏页面； = 2表示响应游戏结束界面
uChar8 startUnderLineId = 0;         //startUnderLineId = 0 表示下划线在"开始"处；= 1 表示下划线在"退出"处
uChar8 gameModel = 0 ;               //gameModel = 0 表示当前处于猜数字；= 1表示猜字母

/**************************************定义外部函数接口************************************************************************/
extern void startInterface(void);                                //开始界面
extern void gameInterface(void);                                 //游戏界面
extern void LCD12864ShowString(uChar8 x, uChar8 y, uChar8* str); //第y行第x列，开始打印str
extern void LCD12864EraseArea(uChar8 x, uChar8 y, uChar8 size);  //从第y行第x列开始，删除size字符
extern void LCD12864EraseAll(void);                              //擦除整个显示屏
extern void img_line_start1(void);                               //画开始界面"开始"处的下划线
extern void img_dele_line_start1(void);                          //删除开始界面"开始"处的下划线
extern void img_line_start2(void);                               //画开始界面"退出"处的下划线
extern void img_dele_line_start2(void);                          //删除开始界面"退出"处的下划线
extern void img_clear(void);                                     //绘图清屏
extern void startUnderLine(uChar8 id);
extern void underLine(uChar8 id);                                //管理游戏界面的下划线，传入数组下标
extern void img_dele_underLine(void);                            //删除所有下划线
extern void display(void);                                       //倒计时显示函数
extern uChar8 isStudentInterface;                                //任务LCD12864Task内是否执行那段函数，来代替系统内信息传递机制
extern int time;                                                 //时间


/**************************************函数声明*******************************************************************************/
void KeyAction(uChar8 KeyValue);     //按键驱动函数 
void up(void);                       //上移
void down(void);                     //下移
void enterGame(void);                //确认键：进入游戏

void add0(void);                     //数字加减并显示
void sub0(void);
void add1(void);                     //字母加减并显示
void sub1(void);
void enterCmp(void);                 //比较用户回答是否正确
void left(void);                     //下划线左移
void right(void);                    //下划线右移
void dele0(void);                    //删除操作：将当前位置0
void dele1(void);                    //删除操作：将当前位置A
void mode(void);                     //猜数字和才字母模式切换
void esc(void);
void clear0(void);                   //将所有位置置0
void clear1(void);                   //将所有位置置A

void randANS(void);                  //随机答案
void steTime(void);                  //重置计时器
	

//按键驱动函数
void KeyAction(uChar8 KeyValue) 
{
	if(interface == 0)               //开始界面的按键驱动
	{
		switch(KeyValue) 
		{
			case 0x31: up(); break;
			case 0x32: down(); break;
			case 0x33: enterGame(); break;
			default: break;
		}
	}
	
	if(interface == 1)               //游戏界面的按键驱动
	{
		if(gameModel == 0)           //猜数字按键功能
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
		else                         //猜字母按键功能
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
	
	if(interface == 2)               //游戏结束界面的按键驱动
	{
		switch(KeyValue) 
		{
			case 0x38: esc(); break;
			default: break;
		}		
	}
}

/************************************开始界面按键*****************************************************************************/
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
		interface = 1;    //进入游戏页面
		randANS();
		gameInterface();
	}
	if(startUnderLineId == 1) //退出
	{
		uChar8 tab[] = "已退出!";
		img_clear();
		LCD12864ShowString(2, 1, tab);
		//while(1);
		startUnderLineId = 0;
		interface = 2;
	}
	
	if(startUnderLineId == 2) //退出
	{
		isStudentInterface = 1;
		startUnderLineId = 0;
		interface = 2;
	}
}

/************************************猜数字按键功能函数*****************************************************************************/
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
	IE2 = 0x00;    //关闭定时器2
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
	isStudentInterface = 0;     //关闭任务2
	IE2 = 0x00;                 //关闭定时器2
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


/************************************猜字母按键函数*****************************************************************************/
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

/************************************判断正确与否*****************************************************************************/
void randANS(void)                  //随机生成答案
{
	if(gameModel == (uChar8)0)      //数字
	{
		for(i = 0; i < 5; i++)
		{
			ANS[i] = rand() % 10;
		}
	}
	else
	{
		for(i = 0; i < 5; i++)      //字母
		{
			ANS[i] = rand() % 25;
		}
	}
}

uChar8 AnsCmpKey(void)
{
	for(i = 0; i < 5; i++)
	{
		if(ANS[i] < KEY[i])         //偏大
		{
			return 2;
		}
		if(ANS[i] > KEY[i])         //偏小
		{
			return 3;
		}
	}
	return 1;
}

void enterCmp(void)
{
	uChar8 str1[] = "  答对了";
	uChar8 str2[] = "继续答题";
	uChar8 str3[] = "答案太小";
	uChar8 str4[] = "答案太大";
	uChar8 str5[] = "猜字游戏"; 

//	LED = !LED;	
	img_dele_underLine();            //删除下划线
	if(AnsCmpKey() == (uChar8)1)     //回答正确
	{
		IE2 = 0x00;                  //关闭定时器2
		grade = grade + 10;          //统计分数
		sprintf(string, "得分: %d", grade);
		LCD12864EraseArea(0, 3, 16);
		LCD12864ShowString(2, 3, string);
			
		LCD12864ShowString(2, 0, str1);
		for(i = 1000; i != 0; i--)   //延时
		for(j = 600; j != 0; j--);
		LCD12864ShowString(2, 0, str2);
		for(i = 1000; i != 0; i--)   //延时
		for(j = 200; j != 0; j--);
		LCD12864ShowString(2, 0, str5);
		
		if(gameModel == (uChar8)1)   //重置游戏
		{
			clear1();
		}
		else
		{
			clear0();
		}
		randANS();                  //随机生成新一轮答案
		steTime();                  //重置定时器
	}
	else                                    //回答错误
	{
		LCD12864EraseArea(0, 1, 16);
		if(AnsCmpKey() == (uChar8)2)
		{
			LCD12864ShowString(2, 1, str4); //答案太大
		}
		
		if(AnsCmpKey() == (uChar8)3)
		{
			LCD12864ShowString(2, 1, str3); //答案太小
		}
		
		for(i = 1000; i != 0; i--)          //延时
		for(j = 100; j != 0; j--);
		
		LCD12864EraseArea(0, 1, 16);
		if(gameModel == (uChar8)0)          //返回游戏
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
	underLine(sign);                        //重新画下划线
}


void steTime(void)
{
	IE2 |= 0x04;    //打开定时器2
	time = 100;     //重置时间
	display();
}

 