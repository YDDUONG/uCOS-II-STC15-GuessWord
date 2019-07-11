/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                          (c) Copyright 1992-2001, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : uCOS_II.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#define  OS_GLOBALS                           /* Declare GLOBAL variables                              */
#include "source\includes.h"


#define  OS_MASTER_FILE                       /* Prevent the following files from including includes.h */
#include ".\ucosii\os_core.c"
#include ".\ucosii\os_flag.c"
#include ".\ucosii\os_mbox.c"
#include ".\ucosii\os_mem.c"
#include ".\ucosii\os_mutex.c"
#include ".\ucosii\os_q.c"
#include ".\ucosii\os_sem.c"
#include ".\ucosii\os_task.c"
#include ".\ucosii\os_time.c"
