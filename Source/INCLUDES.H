/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                           All Rights Reserved
*
*                                           MASTER INCLUDE FILE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Keil C port.
*********************************************************************************************************
*/

#ifndef __INCLUDES__
#define __INCLUDES__

//ϵͳ
#include    "uCosii\os_cpu.h"
#include    "uCosii\os_cfg.h"
#include    "uCosii\uCOS_II.h"

//CPU
//#include    "user\stc15f2k60s2.h"
#include    "user\IAP15W4K61S4.h"
#include    "user\serial.h"

#include 	"stdio.h"
#include 	"INTRINS.h"
#include 	"string.h"
#include    "source\common.h"

extern void Send_Char_Uart1(unsigned char ch) reentrant;
#endif
