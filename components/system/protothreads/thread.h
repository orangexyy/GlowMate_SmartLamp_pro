
/******************************************************************************\
                                    Note
注意事项：
1.在thread.h中以  thread_ 开头的函数均为用户可调用的API，其他函数请勿在外部使用。
2.thread_xxx()函数只能在线程函数中使用，普通函数中不能使用。
3.switch case 语句中不能嵌入thread_xxx()函数。
4.循环变量等需要使用静态变量
\******************************************************************************/
#ifndef __THREAD_H__
#define __THREAD_H__
/**************************************************************************\
                             Includes
\**************************************************************************/
#include <stdint.h>
#include "pt.h"
#include "pt-sem.h"
/******************************************************************************\
                          Macro definitions
\******************************************************************************/
#ifndef MAX_THREADS
#define MAX_THREADS             (20)
#endif
/******************************************************************************\
                         Typedef definitions
\******************************************************************************/
typedef struct pt thread_t;
typedef char (*PT_FUNC)(thread_t* pt);

typedef struct pt_timer_t
{
    uint32_t start;
    uint32_t interval;

} pt_timer_t;

typedef enum
{
    THREAD_WAITING = 0,
    THREAD_YIELDED,
    THREAD_EXITED,
    THREAD_ENDED,
    THREAD_SUSPENDED,
    THREAD_OK,
    THREAD_ERR
} TE_THREAD_STATUS;

typedef struct
{
    PT_FUNC func;
    thread_t pt;
    TE_THREAD_STATUS  status;
    uint8_t suspended;
} TS_THREAD;

typedef struct
{
    TE_THREAD_STATUS status;
    uint8_t  index;
    uint8_t  max_thread;
} TS_THREAD_INFO;
/******************************************************************************\
                    Global variables and functions
\******************************************************************************/
void pt_timer_set(pt_timer_t* t, uint32_t interval);
uint8_t pt_timer_expired(pt_timer_t* t);

#define PT_WAIT_UNTIL_IT(pt, condition)     \
            do {                            \
                LC_SET((pt)->lc);           \
                if(!(condition)) {          \
                    return PT_WAITING;      \
                }                           \
            } while(0)
            
//用户API函数:任务API
#define thread_init(pt)                      PT_INIT(pt)
#define thread_begin                         static pt_timer_t dly; dly=dly; PT_BEGIN(pt);	 
#define thread_end                           PT_END(pt);
#define thread_restart()                     PT_RESTART(pt)
#define thread_exit()                        PT_EXIT(pt)
#define thread_yield()                       PT_YIELD(pt)
#define thread_timeout()                     pt_timer_expired(&dly)
#define thread_interrupt()                   do{while(1){thread_yield();}}while(0)
#define thread_wait_until(cond)              PT_WAIT_UNTIL(pt, cond)
#define thread_sleep(SysTick)                do{pt_timer_set((&dly),(SysTick));thread_wait_until(pt_timer_expired(&dly));}while(0)
#define thread_sleep_until(SysTick, cond)    do{pt_timer_set((&dly),(SysTick));thread_wait_until((cond)||pt_timer_expired(&dly));}while(0)
#define thread_abort()                       thread_exit()
#define thread_join(x)                       do{thread_wait_until(thread_get_status(x) == THREAD_EXITED);}while(0)
#define thread_is_alive(x)                    (thread_get_status(x) < THREAD_EXITED)
#define thread_wait_until_it(condition)      PT_WAIT_UNTIL_IT(pt, condition)
#define thread_sleep_until_it(SysTick, con)  do{pt_timer_set((&dly),(SysTick));thread_wait_until_it((con)||pt_timer_expired(&dly));}while(0)
//------------------------------------------------------------------------------
//用户API函数
void thread_run(void);
void thread_systick_handle(void);
uint32_t thread_get_systick(void);
TE_THREAD_STATUS thread_create(PT_FUNC func);
TE_THREAD_STATUS thread_suspend(PT_FUNC func);
TE_THREAD_STATUS thread_resume(PT_FUNC func);
TE_THREAD_STATUS thread_get_status(PT_FUNC func);

#endif
/******************************* End of File (H) ******************************/

