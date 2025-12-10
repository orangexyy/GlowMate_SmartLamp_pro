
/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include "Thread.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
static uint32_t s_pt_systick;
static TS_THREAD s_task[MAX_THREADS + 1u] = {0};
static TS_THREAD_INFO s_thread_info = {THREAD_WAITING, 0, MAX_THREADS};
/******************************************************************************\
                             Functions definitions
\******************************************************************************/

uint8_t pt_timer_expired(pt_timer_t* t)
{
    uint32_t clock ;
    clock = thread_get_systick();
    clock -= t->start;

    if (clock >= t->interval)
    {
        return 1;
    }

    return 0;
}

void pt_timer_set(pt_timer_t* t, uint32_t interval)
{
    t->interval = interval;
    t->start = thread_get_systick();
}

void thread_systick_handle(void)
{
    s_pt_systick++;
}

uint32_t thread_get_systick(void)
{
    return s_pt_systick;
}

TE_THREAD_STATUS thread_create(PT_FUNC func)
{
    uint8_t threadIndex = 0 ;

    /* First find a gap in the array (if there is one) */
    while((s_task[threadIndex].func != NULL) && (s_task[threadIndex].func != func) && (threadIndex < MAX_THREADS))
    {
        threadIndex++;
    }

    if(s_task[threadIndex].func == func)
    {
        s_task[threadIndex].pt.lc = 0;
        s_task[threadIndex].status = THREAD_WAITING;
        s_task[threadIndex].suspended = 0;
        return THREAD_OK;
    }

    if(threadIndex == MAX_THREADS)
    {
        return THREAD_ERR;
    }

    /* If we're here, there is a space in the task array */
    s_task[threadIndex].func      = func;
    s_task[threadIndex].pt.lc     = 0;
    s_task[threadIndex].status    = THREAD_WAITING;
    s_task[threadIndex].suspended = 0;
    s_thread_info.max_thread       = (sizeof(s_task) / sizeof(s_task[0])) - 1;
    return THREAD_OK;
}

void thread_run(void)
{
    static TS_THREAD* p;
    while(1)
    {
        for(s_thread_info.index=0, p=s_task; p->func; p++, s_thread_info.index++)
        {
            if(p->status < THREAD_EXITED)
            {
                p->status = (TE_THREAD_STATUS)p->func(&p->pt);
            }
        }
    }
}

TE_THREAD_STATUS thread_suspend(PT_FUNC func)
{
    uint8_t threadIndex = 0;

    while((s_task[threadIndex].func != NULL) && (s_task[threadIndex].func != func) && (threadIndex < MAX_THREADS))
    {
        threadIndex++;
    }

    if(s_task[threadIndex].func == func)
    {
        if(s_task[threadIndex].status != THREAD_SUSPENDED) 
        {
            s_task[threadIndex].status = THREAD_SUSPENDED;
            return THREAD_OK;
        }
    }

    return THREAD_ERR;
}

TE_THREAD_STATUS thread_resume(PT_FUNC func)
{
    uint8_t threadIndex = 0;

    while ((s_task[threadIndex].func != NULL) && (s_task[threadIndex].func != func) && (threadIndex < MAX_THREADS))
    {
        threadIndex++;
    }

    if (s_task[threadIndex].func == func)
    {
        if (s_task[threadIndex].status == THREAD_SUSPENDED)
        {
            s_task[threadIndex].status = THREAD_WAITING;
            s_task[threadIndex].suspended = 0;
            return THREAD_OK;
        }
    }

    return THREAD_ERR;
}

TE_THREAD_STATUS thread_get_status(PT_FUNC func)
{
    uint8_t threadIndex = 0;

    while ((s_task[threadIndex].func != NULL)  && (s_task[threadIndex].func != func) && (threadIndex < MAX_THREADS))
    {
        threadIndex++;
    }

    if (s_task[threadIndex].func == func)
    {
        return s_task[threadIndex].status;
    }

    return THREAD_ERR;
}

/******************************* End of File (C) ******************************/
