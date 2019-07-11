/*
*********************************************************************************************************
*                                               uC/OS-II
*                                               实时内核
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                               版权所有
*
*                                            MCU-51 专用代码
*                                           KEIL C51大模式编译
*
* 文件名 : SERIAL.H
* 作者   : Jean J. Labrosse
* 改编   : 杨屹 gdtyy@ri.gdt.com.cn 巨龙公司系统集成开发部 2002.09.27
*********************************************************************************************************
*/

//**********************************************************************************
//杨屹    2002/08/20    第一版
//基于中断的串口驱动及显示程序
//**********************************************************************************
//使用方法：（此范例自包含，独立于其他程序。）
//        1.开头加入#include <reg51.h>语句，一定要有。
//        2.初始化串口        InitSerial();//本例中为20MHz晶体，300波特率，模式2初始化
//        3.初始化串口缓冲区  InitSerialBuffer();
//        4.使用显示字节，字，长字，字符，字符串，清屏函数。
//**********************************************************************************
//波特率计算公式：
//        TH1=256-(2^SMOD/32*Fosc/12*1/Bound)
//    其中：SMOD=0，1；Fosc=晶体或晶振频率；Bound=波特率
//    本例中，SMOD=0；Fosc=20*10E6；Bound=300，所以TH1=0x52。
//**********************************************************************************
void InitSerial() reentrant;										//串口初始化  重入函数用在
//void InitSerialBuffer(void) reentrant;							    //串口缓冲区初始化
//INT8U CommPutChar(unsigned char ch) reentrant;						//显示字符
//void CommPutStr(unsigned char *str, unsigned int n) reentrant; 	    //显示字符串
//bit  CommGetChar(unsigned char *ch) reentrant;						//从串口缓冲区读1字节数据
void Clear_Buf(unsigned char length) reentrant;
void Process_Uart(void)	reentrant;
void serial(void) reentrant;//串口中断服务子程序

void Send_Str_Uart1(unsigned char str[],unsigned int n)	 reentrant;

//收发缓冲区长度
#define TBUF_SIZE        20     
#define RBUF_SIZE        20

