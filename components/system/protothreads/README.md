# Protothreads 轻量级线程系统
## 概述
这是一个基于Protothreads的轻量级线程系统，专为嵌入式MCU设计。系统提供了简单易用的协程式线程管理，支持线程创建、挂起、恢复、定时器等功能，无需操作系统即可实现多任务并发。

## 特性
- **轻量级**: 极小的内存占用，适合资源受限的MCU环境
- **无栈协程**: 基于Protothreads技术，无需为每个线程分配独立栈空间
- **简单易用**: 提供简洁的API接口，降低开发复杂度
- **定时器支持**: 内置定时器功能，支持延时和超时操作
- **线程管理**: 支持线程创建、挂起、恢复、状态查询等操作
- **最大支持20个线程**: 可配置的线程数量限制

## 文件结构
```
protothreads/
├── thread.h          # 主要头文件，包含所有API定义
├── thread.c          # 核心实现文件
├── pt-1.4/           # Protothreads 1.4版本核心库
│   ├── pt.h          # Protothreads核心头文件
│   ├── pt-sem.h      # 信号量支持
│   ├── lc.h          # 本地延续(Local Continuations)
│   └── example-*.c   # 示例代码
└── README.md         # 本文档
```

## 快速开始
### 1. 系统初始化
```c
#include "thread.h"

// 在main函数中启动线程调度器
int main(void)
{
    // 系统初始化代码...
    
    // 启动线程调度器（永不返回）
    thread_run();
    
    return 0;
}
```
### 2. 创建线程函数
```c
// 线程函数示例
char led_blink_thread(thread_t *pt)
{
    thread_begin;  // 线程开始
    
    while(1) {
        LED_ON();
        thread_sleep(500);  // 延时500个系统时钟周期
        LED_OFF();
        thread_sleep(500);
    }
    
    thread_end;  // 线程结束
}
```
### 3. 创建和管理线程
```c
// 创建线程
TE_THREAD_STATUS status = thread_create(led_blink_thread);
if(status == THREAD_OK) {
    // 线程创建成功
}

// 挂起线程
thread_suspend(led_blink_thread);

// 恢复线程
thread_resume(led_blink_thread);

// 查询线程状态
TE_THREAD_STATUS thread_status = thread_get_status(led_blink_thread);
```

## API 参考
### 线程控制宏
| 宏 | 描述 |
|---|---|
| `thread_init(pt)` | 初始化线程结构体 |
| `thread_begin` | 线程函数开始标记 |
| `thread_end` | 线程函数结束标记 |
| `thread_restart()` | 重启当前线程 |
| `thread_exit()` | 退出当前线程 |
| `thread_yield()` | 让出CPU，等待下次调度 |
| `thread_abort()` | 中止当前线程 |

### 等待和延时宏

| 宏 | 描述 |
|---|---|
| `thread_wait_until(condition)` | 等待条件满足 |
| `thread_sleep(SysTick)` | 延时指定系统时钟周期数 |
| `thread_sleep_until(SysTick, condition)` | 延时或等待条件满足 |
| `thread_wait_until_it(condition)` | 等待条件满足（中断版本） |
| `thread_sleep_until_it(SysTick, condition)` | 延时或等待条件满足（中断版本） |

### 线程管理函数

| 函数 | 描述 |
|---|---|
| `thread_create(PT_FUNC func)` | 创建线程 |
| `thread_suspend(PT_FUNC func)` | 挂起线程 |
| `thread_resume(PT_FUNC func)` | 恢复线程 |
| `thread_get_status(PT_FUNC func)` | 获取线程状态 |
| `thread_run(void)` | 启动线程调度器 |

### 系统函数

| 函数 | 描述 |
|---|---|
| `thread_systick_handle(void)` | 系统时钟中断处理函数 |
| `thread_get_systick(void)` | 获取系统时钟计数 |
| `pt_timer_set(pt_timer_t* t, uint32_t interval)` | 设置定时器 |
| `pt_timer_expired(pt_timer_t* t)` | 检查定时器是否超时 |

### 线程状态

| 状态 | 描述 |
|---|---|
| `THREAD_WAITING` | 等待执行 |
| `THREAD_YIELDED` | 已让出CPU |
| `THREAD_EXITED` | 已退出 |
| `THREAD_ENDED` | 已结束 |
| `THREAD_SUSPENDED` | 已挂起 |
| `THREAD_OK` | 操作成功 |
| `THREAD_ERR` | 操作失败 |

## 使用示例
### 示例1: LED闪烁线程
```c
char led_blink_thread(thread_t *pt)
{
    thread_begin;
    
    while(1) {
        GPIO_SetBits(GPIOA, GPIO_Pin_0);  // LED ON
        thread_sleep(1000);
        GPIO_ResetBits(GPIOA, GPIO_Pin_0); // LED OFF
        thread_sleep(1000);
    }
    
    thread_end;
}
```
### 示例2: 按键检测线程
```c
char button_thread(thread_t *pt)
{
    thread_begin;
    
    while(1) {
        // 等待按键按下
        thread_wait_until(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0);
        
        // 按键防抖
        thread_sleep(50);
        
        // 等待按键释放
        thread_wait_until(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 1);
        
        // 执行按键动作
        button_action();
    }
    
    thread_end;
}
```
### 示例3: 串口通信线程
```c
char uart_thread(thread_t *pt)
{
    thread_begin;
    
    while(1) {
        // 等待接收数据
        thread_wait_until(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET);
        
        uint8_t data = USART_ReceiveData(USART1);
        
        // 处理接收到的数据
        process_uart_data(data);
    }
    
    thread_end;
}
```

## 注意事项
1. **线程函数限制**: 以 `thread_` 开头的函数只能在线程函数中使用，不能在普通函数中调用
2. **switch-case限制**: 不能在switch-case语句中嵌入 `thread_xxx()` 函数
3. **静态变量**: 循环变量等需要使用静态变量
4. **系统时钟**: 需要定期调用 `thread_systick_handle()` 来更新系统时钟
5. **线程数量**: 默认最大支持20个线程，可通过修改 `MAX_THREADS` 宏来调整

## 配置选项
### 最大线程数配置
```c
// 在包含thread.h之前定义
#define MAX_THREADS 10  // 修改最大线程数
#include "thread.h"
```
