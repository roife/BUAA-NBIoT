/*********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : thread.c
* By      : tony(gulonglong)
* Version : V1.3
*
* thread.c file have achieved some goals,as follow:
    void thread_os_run(void)
*   void thread_os_register(process_t *thread_node, const char *arg)
    void pthread_os_suspend(process_t *thread_node)
		void thread_os_resume  (process_t *thread_node)
    void pthread_os_stop   (process_t *thread_node)
		void thread_os_restart (process_t *thread_node)
		void thread_os_delete  (process_t *thread_node)
		process_t *get_thread_name(void *ThreadName)
		process_t *get_thread(uint32_t pid)

*********************************************************************************************************
*/
#include <stdint.h>
#include <string.h>

#include "thread.h"


static uint32_t  thread_id = 0; 

process_t *process_list   = NULL;

void thread_os_register(process_t *thread_node, const char *arg)
{
  process_t *q = process_list;

	if( thread_node == NULL )
	{
		os_log("input param '*thread_node' is NULL!\r\n");
		return;
	}
	
	if( process_list != NULL )
	{
		/* First make sure that we don't try to start a process that is
			 already running. */
		for( ; q != thread_node && q->next != process_list; q = q->next);

		/* If we found the process on the process list, we bail out. */
		if(q == thread_node) 
		{
			return;
		}
	}
	
	thread_node->status = THREAD_READY;
	thread_node->pid    = ++thread_id;
	thread_node->tp     = 0;
	thread_node->timetick   = 0;
	
  /* Put on the tail of process_list.*/
	if( process_list == NULL )
	{
		process_list = thread_node;
		thread_node->next = process_list;
	}
	else
  {
		thread_node->next = process_list;
		q->next = thread_node;
	}
}

void thread_os_suspend(process_t *thread_node)
{
	process_t *q;
	
	if( thread_node == NULL )
	{
		os_log("input param '*thread_node' is NULL!\r\n");
		return;
	}
	
	if( thread_node->status == THREAD_SUSPEND || thread_node->status == THREAD_STOP ||
  		thread_node->status == THREAD_NOUSE)
	{
		os_log("'%s' had stopped or suspended\r\n",thread_node->ThreadName);
		return;
	}
	
	if( !strcmp(thread_node->ThreadName,"shell thread") || !strcmp(thread_node->ThreadName,"ostimer thread") )
	{
		os_log("'%s' belong to the system-level thread , user has no right to suspend it");
		return;
	}
				
  /* First make sure that we don't try to suspend a process that is
     already suspended. */
  for(q = process_list; q != thread_node && q->next != process_list; q = q->next);

  /* If we found the process in the process list, we process it. */
  if(q == thread_node)
	{
		thread_node->status = THREAD_SUSPEND;

		os_log("suspend '%s'\r\n",thread_node->ThreadName);
	}
}

void thread_os_resume(process_t *thread_node)
{
	process_t *q;
	
	if( thread_node == NULL )
	{
		os_log("input param '*thread_node' is NULL!\r\n");
		return;
	}
	
	if( thread_node->status != THREAD_SUSPEND )
	{
		os_log("'%s' had run or resume\r\n", thread_node->ThreadName);
		return;
	}

  /* First make sure that we don't try to start a process that is
     already running. */
  for(q = process_list; q != thread_node && q->next != process_list; q = q->next);

  /* If we found the process on the process list, we process it. */
  if(q == thread_node) 
	{
		thread_node->status = THREAD_READY;
	
		os_log("resume '%s'\r\n", thread_node->ThreadName);
	}
}

void thread_os_stop(process_t *thread_node)
{
	process_t *q;
	
	if( thread_node == NULL )
	{
		os_log("input param '*thread_node' is NULL!\r\n");
		return;
	}
	
	if( thread_node->status == THREAD_STOP || thread_node->status == THREAD_NOUSE )
	{
		os_log("'%s' had stopped or not use!\r\n", thread_node->ThreadName);
		return;
	}

  /* First make sure that we don't try to start a process that is
     already running. */
  for(q = process_list; q != thread_node && q->next != process_list; q = q->next);

  /* If we found the process on the process list, we process it. */
  if(q == thread_node) 
	{
		thread_node->status   = THREAD_STOP;
		thread_node->tp       = 0;
		thread_node->timetick = 0;
		
		os_log("stop '%s'\r\n", thread_node->ThreadName);
	}
}

void thread_os_restart(process_t *thread_node)
{
	process_t *q;

	if( thread_node == NULL )
	{
		os_log("input param '*thread_node' is NULL!\r\n");
		return;
	}
	
	if( thread_node->status == THREAD_NOUSE )
	{
		os_log("this thread not use!\r\n");
		return;
	}
	
  /* First make sure that we don't try to start a process that is
     already running. */
  for(q = process_list; q != thread_node && q->next != process_list; q = q->next);

  /* If we found the process on the process list, we process it. */
  if( q == thread_node ) 
	{
		thread_node->status   = THREAD_READY;
		thread_node->tp       = 0;
		thread_node->timetick = 0;
		
		os_log("restart '%s'\r\n", thread_node->ThreadName);
	}
}

void thread_os_delete(process_t *thread_node)
{
	process_t *q;
	process_t *prev;

	if( thread_node == NULL )
	{
		os_log("input param '*thread_node' is NULL!\r\n");
		return;
	}
	
	if( thread_node->status == THREAD_NOUSE )
	{
		os_log("this thread had been deleted or init status!\r\n");
		return;
	}
	
  /* First make sure that we don't try to delete a process that had been deleted. */
  for(q = process_list; q != thread_node && q->next != process_list; prev = q,q = q->next);

  /* If we found the process on the process list, we delete it. */
  if( q == thread_node) 
	{
		/* uninit process */
		thread_node->status   = THREAD_NOUSE;
		thread_node->pid      = 0;
		thread_node->tp       = 0;
		thread_node->timetick = 0;
		
		os_log("delete '%s'\r\n", thread_node->ThreadName);
		
		/* delete it */
		prev->next = q->next;
	}
}


process_t *get_thread_name(void *ThreadName)
{
	process_t *q = process_list;
	
	if( q == NULL )
		return NULL;
		
	do
	{
		if( strcmp(q->ThreadName, ThreadName) == 0 )
		{
			return q;
		}
		q = q->next;
	}while(q != process_list);
	
	return NULL;
}

process_t *get_thread(uint32_t pid)
{
	process_t *q = process_list;
	
	if( q == NULL )
		return NULL;
		
	do
	{
		if( q->pid == pid )
		{
			return q;
		}
		q = q->next;
	}while(q != process_list);
	
	return NULL;
}

