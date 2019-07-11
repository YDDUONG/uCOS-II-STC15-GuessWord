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
* 文件名 : OS_CPU.H
* 作者   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef __OS_CPU_H
#define __OS_CPU_H

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/*
*********************************************************************************************************
*                                       数据类型
*                                     (编译器相关)
*********************************************************************************************************
*/

//详见C51.PDF第176页
typedef unsigned char  BOOLEAN;       //注意：不要使用bit定义，因为在结构体里无法使用
typedef unsigned char  INT8U;         //无符号8位数
typedef signed   char  INT8S;         //有符号8位数
typedef unsigned int   INT16U;        //无符号16位数
typedef signed   int   INT16S;        //有符号16位数
typedef unsigned long  INT32U;        //无符号32位数
typedef signed   long  INT32S;        //有符号32位数
typedef float          FP32;          //单精度浮点数
typedef double         FP64;          //双精度浮点数

typedef unsigned char  OS_STK;        //栈单元宽度为8比特
typedef unsigned char  OS_CPU_SR;     /* Define size of CPU status register (PSW = 8 bits)  */

#define BYTE           INT8S          //兼容以前版本的数据类型
#define UBYTE          INT8U          //uC/OS-II可以不用这些数据类型
#define WORD           INT16S
#define UWORD          INT16U
#define LONG           INT32S
#define ULONG          INT32U

/* 
*********************************************************************************************************
*                              处理器相关代码 MCU-51 (大模式)
* 针对51单片机，只使用方法1(直接开关中断)
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD    1

#if      OS_CRITICAL_METHOD == 1
#define  OS_ENTER_CRITICAL()  EA=0    //关中断
#define  OS_EXIT_CRITICAL()   EA=1    //开中断
#endif

#if      OS_CRITICAL_METHOD == 2
/* As an undocumented keyword of keil c. __asm is supported in Keil C v6.20.
. No other means to define assemble language code in a macro, I have to use it here. If your compiler does not support __asm, use method 1 or 3 then. */
/* A2 AF MOV C, EA*/
/* C2 AF CLR EA   */
/* C0 D0 PUSH PSW */
#define  OS_ENTER_CRITICAL()    __asm DB 0A2H, 0AFH, 0C2H, 0AFH, 0C0H, 0D0H
/* D0 D0 POP PSW   */
/* 92 AF MOV EA, C */
#define  OS_EXIT_CRITICAL()     __asm DB 0D0H, 0D0H, 092H, 0AFH
#endif

#if      OS_CRITICAL_METHOD == 3
#define  OS_ENTER_CRITICAL()  (cpu_sr = EA, EA=0)   /* Disable interrupts  */
#define  OS_EXIT_CRITICAL()   (EA=cpu_sr)    		/* Enable  interrupts  */
#endif


#define  OS_STK_GROWTH    0           //MCU-51堆栈从下往上增长  1=向下，0=向上

#define  OS_TASK_SW()     OSCtxSw()   //因为MCU-51没有软中断指令，所以用程序调用代替。两者的堆栈格式相同，
                                      //RETI指令复位中断系统，RET则没有。实践表明，对于MCU-51，用子程序调
                                      //用入栈，用中断返回指令RETI出栈是没有问题的，反之中断入栈RET出栈则
                                      //不行。总之，对于入栈，子程序调用与中断调用效果是一样的，可以混用。
                                      //在没有中断发生的情况下复位中断系统也不会影响系统正常运行。
                                      //详见《uC/OS-II》第八章193页第12行
                                      
#define OS_ISR_PROTO_EXT	1
void OSCtxSw(void);

void InitHardware(void) reentrant;    //初始化硬件时钟中断等，系统初始化工作

#endif //_OS_CPU_H
