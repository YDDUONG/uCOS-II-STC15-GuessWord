/*********************************************************************************************************
*                                               uC/OS-II
*                                               实时内核
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                               版权所有
*
*                                            MCU-51 专用代码
*                                           KEIL C51大模式编译
*
* 文件名 : SERIAL.C
* 作者   : Jean J. Labrosse
* 改编   : 杨屹 gdtyy@ri.gdt.com.cn 巨龙公司系统集成开发部 2002.09.27
********************************************************************************************************/
//**********************************************************************************
//杨屹    2002/08/20    第一版
//基于中断的串口驱动
//**********************************************************************************
//程序特点：
//        1.基于中断，可并发执行
//        2.参数可配置（收发缓冲区大小，最大字符串长度）
//**********************************************************************************
//使用方法:
//        先配制收发缓冲区大小等可变参数（在serial.h中的宏定义）
//        1.开头加入#include <reg51.h>语句，一定要有。
//        2.初始化串口        InitSerial();//本例中为20MHz晶体，300波特率，模式2初始化
//        3.初始化串口缓冲区  InitSerialBuffer();
//**********************************************************************************
//波特率计算公式：
//        TH1=256-(2^SMOD/32*Fosc/12*1/Bound)
//    其中：SMOD=0，1；Fosc=晶体或晶振频率；Bound=波特率
//    本例中，SMOD=0；Fosc=20*10E6；Bound=300，所以TH1=0x52。
//	  Baud rate(20Mhz)
//	  300(52);1200(D5);2400(EA);4800(F5);9600(FB);19200(FD);38400(FF);
//**********************************************************************************
#ifndef  OS_MASTER_FILE
#include "source\includes.h"
#endif
#include "source\includes.h"

#define COMM_NO_ERR 0
#define COMM_RX_EMPTY 1
#define COMM_TX_FULL 1

//高速串口驱动
//制约传输速率的关键因素是串行口的中断服务子程序的执行时间；uCOS要求  串口速率 < 19200
//简化驱动模型，快速中断即进入中断后立即关中断，退出时开中断 且不通知uCos 提高串口中断优先级
INT8U TxBuf[TBUF_SIZE] = {0}, RxBuf[RBUF_SIZE] = {0};         //收发缓冲区
//INT8U *inTxBuf = NULL, *outTxBuf = NULL, *inRxBuf = NULL, *outRxBuf = NULL; //收发缓冲区读写指针
INT8U RxNum = 0;
int gPosi_S1 = -1;
bit TIflag = 1;       //发送信号，Note:It must be 1.
bit gRec_Flag1 = 0;   //接收信号



void InitSerial() reentrant//串口初始化
{	//11.0592MHZ  9600
	SCON = 0x50;
	PCON &= 0x7f;		//波特率不倍速
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
 	AUXR &= 0xfe;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0f;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xDC;		    //1T
	TH1 = 0xDC;		    //
	TR1 = 1;
	ES = 1;
}

/*
void InitSerialBuffer(void) reentrant//串口缓冲区初始化
{
        ES = 0;
		inTxBuf = TxBuf;       //初始化发送接收输入输出指针
		outTxBuf = TxBuf;
        inRxBuf = RxBuf;
		outRxBuf = RxBuf;
        ES = 1;
}
*/
void serial(void) reentrant	  //串口应用功能部分
{    	//中断系统部分处理在汇编中实现，去掉interrupt 4{//串口中断服务子程序
	//INT8U *t;
	/*
    if (RI)	//接收中断处理
	{	
    	RI = 0;	  				
        t = inRxBuf;
		t++;
        if (t == (RxBuf + RBUF_SIZE))  //下一个位置到末尾
			t = RxBuf;
        if (t == outRxBuf) 
			return;                    //RxBuf Full	  因为初始化的时候两个指针是相同的
        *inRxBuf = SBUF;	//存放接收数据
        inRxBuf = t;        //指向下一个位置
    }
	*/
	if (RI)
	{
		RI = 0;
		gRec_Flag1 = 1;	
			
		if (RxNum < RBUF_SIZE)
			RxBuf[RxNum++] = SBUF;
		else
			SBUF = 0;
	}
	/*
	if (TI)					//发送中断处理
	{
        TI = 0;
		if (inTxBuf == outTxBuf) 	  //输入输出指针相同，发送缓存空  初始化的时候两个指针是相同的
		{
			TIflag = 1;				  //发送信号 = 	1，发送第一个数据之后继续发送 直到发送缓存为空 同时 TIflag = 1
			return;
		}
        SBUF = *(outTxBuf++); 			    //发送数据  指向下一个发送位置	每发送完一个数据 TI由硬件置1
        if (outTxBuf == TxBuf + TBUF_SIZE) 	//输出指针超过缓存长度，重新定位到缓存首地址
			outTxBuf = TxBuf;	
     }
	*/
}

void Send_Char_Uart1(unsigned char ch) reentrant
{
  ES = 0;                      //关串口1中断，在产生发送完成中断标志时不会真正进入中断函数
  TI = 0;                      //清零串口发送完成中断标志
  SBUF = ch;                   //发送数据
  while(TI == 0);	           //等待发送完毕
  TI = 0;					   //清零发送完成中断标志
  ES = 1;						
}

void Send_Str_Uart1(unsigned char str[],unsigned int n)	 reentrant
{
   unsigned int i;
   for(i = 0;i < n;i++) Send_Char_Uart1(str[i]);
}

/*
bit CommGetChar(unsigned char *ch) reentrant//从串口缓冲区读1字节数据
{
        //ES=0;        
        if(inRxBuf == outRxBuf) //接收缓存空
		{
			ES = 1;
			return 0;
		}
        *ch = *outRxBuf;  
		outRxBuf++;
        if(outRxBuf == RxBuf + RBUF_SIZE) 
			outRxBuf = RxBuf;
        //ES=1; 
		return 0;       
}
*/
/*
INT8U CommPutChar(unsigned char ch) reentrant//显示字符
{
        unsigned char *t;
        ES = 0;        
        if (TIflag)	     //发送信号 = 1  第一次发送数据
		{		
        	TIflag = 0;	   
            TI = 1;		 //启动中断发送
        }
        t = inTxBuf;	 //输入指针
		t++;
        if (t == TxBuf + TBUF_SIZE) 	//输入指针超过缓存长度，重新定位到缓存首地址
			t = TxBuf;
        if (t == outTxBuf) 	 //发送缓存满  不能发送
		{
			ES = 1;
			return (COMM_TX_FULL);
		}                 
        *inTxBuf = ch;	  //将数据放入发送缓存等待发送
        inTxBuf = t;	  //输入指针加1
        ES = 1;
		return (COMM_NO_ERR);
}
*/
/*
void CommPutStr(unsigned char *str, unsigned int n) reentrant//显示字符串
{
        unsigned char i;
	
        for(i = 0;i < n;i++) CommPutChar(*(str + i));
}
*/

/*串口接收处理*/
void Process_Uart(void)	reentrant
{
	INT8U CMD = 0xff;
	//Feed_WDT();
	if (gRec_Flag1)
	{
	    OSTimeDly(OS_TICKS_PER_SEC/10);	 //100ms
		Send_Str_Uart1(RxBuf, 20);
		gPosi_S1 = strpos(RxBuf, 0x7e);
		if (gPosi_S1 != -1) 		   //如果找到
		{
			CMD = RxBuf[gPosi_S1 + 2];   //获取命令

			switch (CMD)
			{						   
				case 0x0a:        //取消配置语音模式
//					ZY1730_CONFIG = 0;
					break;
				case 0xdd:
					DE485 = 1;    //发送使能
				//	Send_Str_Uart1("485 Test!\r\n", 11);
					DE485 = 0;
					break;
				default:		  //嵌套部分用来处理不经常发生的消息
					switch (CMD)
					{
						case 0x10:			  //录音
						//	ZY1730_CONFIG = 1;
							if((RxBuf[gPosi_S1 + 3] >= 0x10) && (RxBuf[gPosi_S1 + 4] <= 0xff))
							{
						//		Send_Str_Uart1("Start Rec!",10);
						//		Start_Rec_Voice(S1_Rec_Buf[gPosi_S1 + 3], S1_Rec_Buf[gPosi_S1 + 4]);  //录音长短决定于区段的大小
							}
							break;
						case 0x11:			  //7e 02 11 xx xx 0d 0a	   测试
						//	ZY1730_CONFIG = 1;
							if((RxBuf[gPosi_S1 + 3] >= 0x10) && (RxBuf[gPosi_S1 + 4] <= 0xff))
							{
						//		Send_Str_Uart1("Playing Rec!",12);
						//		Play_Voice(S1_Rec_Buf[gPosi_S1 + 3], S1_Rec_Buf[gPosi_S1 + 4]);//播音	   0x10~0xff
							}
							break;
						case 0x12:			  //擦除
						//	ZY1730_CONFIG = 1;
							if((RxBuf[gPosi_S1 + 3] >= 0x10) && (RxBuf[gPosi_S1 + 4] <= 0xff))
							{
						//		Send_Str_Uart1("Eerseing!",9);
						//		Erase_Voice(S1_Rec_Buf[gPosi_S1 + 3], S1_Rec_Buf[gPosi_S1 + 4]);
							}
							break;						
					}
			}
		}
		Clear_Buf(RxNum);
		gRec_Flag1 = 0;	
		RxNum = 0;
		gPosi_S1 = -1;
	}
}

void Clear_Buf(unsigned char length) reentrant
{
	unsigned char i;
	ES = 0;
	for (i = 0; i < length; i++)
		RxBuf[i] = 0;
	ES = 1;
}



