#include "includes.h"

typedef unsigned char uChar8;

/**************************************定义全局变量********************************************************************/	
sbit LED = P5^5;                                //运行灯
OS_STK gameTaskStk[MaxStkSize+1];               //定义自己的任务堆栈
OS_STK LEDTaskStk[MaxStkSize+1];                //定义自己的任务堆栈
OS_STK LCD12864TaskStk[MaxStkSize+1];           //定义自己的任务堆栈
uChar8 isStudentInterface = 0;                  //标志位，为1表示执行，非1表示不执行LCD任务

/**************************************定义外部函数接口*****************************************************************/	
extern void IO_init(void);                      //IAP15端口初始化
extern void initLCD12864(void);                 //LCD12864初始化
extern void keyDriver(void);                    //按键驱动
extern void configureTimer0(void) reentrant;    //系统时钟time0初始化
extern void configureTimer1(uChar8 ms);         //定时器1初始化
extern void configureTimer2(void);              //定时器2初始化
extern void startInterface(void);               //开始界面
extern void studentInterface(void);             //开始界面

/**************************************定义用户任务***********************************************************************/	
void LEDTask(void *ppdata) reentrant;           //LED状态控制任务
void LCD12864Task(void *ppdata) reentrant;      //LCD控制任务
void gameTask(void *ppdata) reentrant;          //游戏任务

/**************************************主函数**********************************************************************/	
void main(void)
{
	IO_init();                                  //IAP15端口初始化
	initLCD12864();                             //LCD12864初始化
	configureTimer1(1);                         //定时器1初始化
	configureTimer2();                          //定时器2初始化
	EA  = 1;                                    //允许所有中断开启
	
	OSInit();                                   //ucosii系统初始化
	configureTimer0();                          //系统时钟time0  	    

	OSTaskCreate(LEDTask, (void*)0, &LEDTaskStk[0], 1);
	OSTaskCreate(LCD12864Task, (void*)0, &LCD12864TaskStk[0], 2);
	OSTaskCreate(gameTask, (void*)0, &gameTaskStk[0], 3);
	
	OSStart();                                  //开始任务调度
}




void LEDTask(void *ppdata) reentrant
{
	ppdata = ppdata;                            //防止编译出错
	ET0 = 1;                                    //根任务开时钟节拍中断
	
	while(1)
    {
		LED = !LED;
        OSTimeDly(OS_TICKS_PER_SEC);            //用户任务延时1s
    }    
}

void LCD12864Task(void *ppdata) reentrant
{
	ppdata = ppdata;                         
	
	while(1)
    {
		if(isStudentInterface == 1)             //标志位，LCD是否执行显示学生信息界面
		{
			studentInterface();                 //学生信息界面
		}
		OSTimeDly(OS_TICKS_PER_SEC);
    }  
}

void gameTask(void *ppdata) reentrant
{
	ppdata = ppdata;                           
	startInterface();                           //游戏开始界面
	
	while(1)
    {
		keyDriver();                            //按键驱动
        OSTimeDly(OS_TICKS_PER_SEC / 10);       //用户任务延时0.1s
    }    
}






