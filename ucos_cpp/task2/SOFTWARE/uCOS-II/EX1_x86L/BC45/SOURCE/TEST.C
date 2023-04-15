/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include "includes.h"
#include "stdio.h"
//#include "stdlib.h"
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK        Task1_Stk[TASK_STK_SIZE]; 
OS_STK        Task2_Stk[TASK_STK_SIZE];
INT8U         err;
OS_EVENT      *ResourceMutex1;
OS_EVENT      *ResourceMutex2;

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  Task1();                       /* Function prototypes of tasks                  */
        void  Task2();
static  void  TaskStartCreateTasks(void);
static  void  MutexCreate(void);
		Node* ListPrintNode(Node *head);
		Node* ListPrintResource(Node *head);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{

	head = NULL;
	head2 = NULL;

    printf("[Time]  [Event]   [From]  [To]\r\n");
    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    TaskStartCreateTasks();

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
	MutexCreate();

    OS_EXIT_CRITICAL();
    OSTimeSet(0);
    OSStart();                                             /* Start multitasking                       */
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
/*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
        OSTaskCreate(Task1, (void *)0, &Task1_Stk[TASK_STK_SIZE], 3);
        OSTaskCreate(Task2, (void *)0, &Task2_Stk[TASK_STK_SIZE], 4);
}

static void MutexCreate(void)
{
       ResourceMutex1 = OSMutexCreate(1,&err);
       ResourceMutex2 = OSMutexCreate(2,&err);
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/


void  Task1()
{
  int ready;
  int C = 11;
  int resource = 0;
  int status = 0;
  OSTCBCur->compTime = C;
  for(;;){
      ready = OSTime;
      if(ready < 5){
            ready = 5 - ready;
            OSTimeDly(ready);
      }
      while(OSTCBCur->compTime > 0){
        if(status != OSTCBCur->compTime){
             if(resource == 2){
                OSMutexPend(ResourceMutex2,0,&err);
             }
             else if(resource == 5){
                OSMutexPend(ResourceMutex1,0,&err);
             }
             else if(resource == 8){
                OSMutexPost(ResourceMutex1);
             }
             resource = resource + 1;
             status = OSTCBCur->compTime;
         }
      }
      OSMutexPost(ResourceMutex2);
	  OSTimeDly(1);
	  head2 = ListPrintResource(head2);
	  head = ListPrintNode(head);
      OSTaskSuspend(3);
    }
}


void  Task2()
{
  int C = 12;
  int resource = 0;
  int status = 0;
  OSTCBCur->compTime = C;

  for(;;){
      while(OSTCBCur->compTime > 0){
         if(status != OSTCBCur->compTime){
            if(resource == 2){
               OSMutexPend(ResourceMutex1,0,&err);
            }
            else if(resource == 8){
               OSMutexPend(ResourceMutex2,0,&err);
            }
            else if(resource == 10){
               OSMutexPost(ResourceMutex2);
            }
            resource = resource + 1;
            status = OSTCBCur->compTime;
         }
      }
      OSMutexPost(ResourceMutex1);
      OSTaskSuspend(4);
    }
}

Node* ListPrintResource(Node *head){
	Node *cur = head;
	Node *temp;
	int idx;
	if(head == NULL){
		return head;
	}
	while(cur != NULL){
	 	idx = cur->state;
	 	switch(idx)
	 	{
	 	 	case 1:
                printf("%-10d lock    R%d Prio %d changes to %d\n",cur->ticks, cur->resource, cur->prio_cur, cur->prio_next);
	 	 	 	break;
 	 	 	case 2:
   	 	 	 	printf("%-10d unlock  R%d Prio %d changes to %d\n",cur->ticks, cur->resource, cur->prio_cur, cur->prio_next);
	 	 	 	break;
 	 	}
	 	cur = cur->next;
 	 	temp = head;
 	 	head = head->next;
	 	free(temp);
	}
	return head;
}


Node* ListPrintNode(Node *head){
	Node *cur = head;
	Node *temp;
	int idx;
	if(head == NULL){
		return head;
	}
	while(cur != NULL){
	 	idx = cur->state;
	 	switch(idx)
	 	{
	 	 	case 1:
 	 	 	 	printf("%-10d%-16s%3d%9d\n", cur->ticks,"Preempt", cur->prio_cur, cur->prio_next);
	 	 	 	break;
 	 	 	case 2:
          	 	printf("%-10d%-16s%3d%9d\n", cur->ticks,"Complete", cur->prio_cur, cur->prio_next);
	 	 	 	break;
 	 	}
	 	cur = cur->next;
 	 	temp = head;
 	 	head = head->next;
	 	free(temp);
	}
	return head;
}

