# Xmodem固件升级使用说明

## 概述

本项目实现了基于Xmodem协议的STM32F103C8T6固件升级功能。通过串口通信，可以将固件文件从PC端发送到STM32单片机，实现固件的远程升级。

## 功能特性

- 支持Xmodem-1K（1024字节）和Xmodem（128字节）两种模式
- 使用CRC16校验确保数据传输的可靠性
- 自动重试机制，提高传输成功率
- 支持进度显示
- 完整的错误处理机制

## 系统架构

### Bootloader区域
- **起始地址**: 0x08000000
- **大小**: 4KB (0x1000)
- **功能**: 负责固件升级和跳转到Application

### Application区域
- **起始地址**: 0x08001000
- **最大大小**: 60KB (0xF000)
- **功能**: 用户应用程序

## 使用方法

### 1. 编译Bootloader

1. 使用Keil MDK或其他IDE打开 `project/bootloader/keil_project` 目录下的工程
2. 编译生成 `bootloader.bin` 文件
3. 使用ST-Link或其他烧录工具将bootloader烧录到STM32的0x08000000地址

### 2. 编译Application

1. 使用Keil MDK打开 `project/application/keil_project` 目录下的工程
2. **重要**: 修改Application的起始地址为 `0x08001000`
   - 在Keil中: Options for Target -> Target -> IROM1 Start: 0x08001000
3. 编译生成 `application.bin` 文件

### 3. 使用Xmodem工具发送固件

#### Windows系统

```bash
# 安装pyserial库
pip install pyserial

# 发送固件（使用1024字节模式，推荐）
python tools/xmodem_sender.py COM3 115200 application.bin

# 发送固件（使用128字节模式）
python tools/xmodem_sender.py COM3 115200 application.bin --128
```

#### Linux系统

```bash
# 安装pyserial库
pip install pyserial

# 发送固件（使用1024字节模式，推荐）
python3 tools/xmodem_sender.py /dev/ttyUSB0 115200 application.bin

# 发送固件（使用128字节模式）
python3 tools/xmodem_sender.py /dev/ttyUSB0 115200 application.bin --128
```

### 4. 在Bootloader中启动升级

在Bootloader的主循环中，需要调用以下函数来启动固件升级：

```c
#include "firmware_upgrade.h"
#include "drv_comm.h"

// 初始化固件升级模块（在系统初始化时调用一次）
firmware_upgrade_init(DRV_USART_ID_1);  // 使用USART1

// 启动固件升级（在需要升级时调用）
firmware_upgrade_start();

// 在主循环中处理升级
while(1)
{
    firmware_upgrade_process();
    
    // 检查升级状态
    uint8_t state = firmware_upgrade_get_state();
    if (state == FW_UPGRADE_STATE_COMPLETE)
    {
        // 升级完成，可以跳转到Application
        firmware_upgrade_jump_to_app();
    }
    else if (state == FW_UPGRADE_STATE_ERROR)
    {
        // 升级失败，处理错误
        // ...
    }
    
    // 其他任务...
}
```

## 协议说明

### Xmodem协议流程

1. **接收端发送'C'**: Bootloader发送字符'C'（0x43）请求CRC模式
2. **发送端发送数据包**: PC端发送包含固件数据的数据包
3. **接收端ACK/NAK**: Bootloader校验数据包，正确则发送ACK，错误则发送NAK
4. **重复步骤2-3**: 直到所有数据包发送完成
5. **发送端发送EOT**: PC端发送EOT（0x04）表示传输结束
6. **接收端发送ACK**: Bootloader发送最终ACK确认

### 数据包格式

#### Xmodem-1K (1024字节) 数据包
```
[STX(0x02)] [包序号] [包序号取反] [1024字节数据] [CRC16高字节] [CRC16低字节]
```

#### Xmodem (128字节) 数据包
```
[SOH(0x01)] [包序号] [包序号取反] [128字节数据] [CRC16高字节] [CRC16低字节]
```

### CRC16校验

使用Xmodem标准的CRC16算法：
- 多项式: 0x1021
- 初始值: 0x0000
- 输入反转: 否
- 输出反转: 否

## 代码结构

```
project/bootloader/drivers/
├── xmodem.c/h          # Xmodem协议实现
├── flash_iap.c/h       # Flash IAP操作封装
├── firmware_upgrade.c/h # 固件升级管理模块
└── drv_comm.c/h        # 串口驱动
```

## 注意事项

1. **Flash地址配置**: 确保Application的起始地址配置为0x08001000
2. **向量表偏移**: Application需要设置向量表偏移到0x08001000
3. **串口配置**: 确保Bootloader和PC端使用相同的波特率（推荐115200）
4. **文件大小**: Application固件大小不能超过60KB
5. **超时设置**: 如果传输过程中出现超时，可以增加超时时间
6. **中断优先级**: 确保串口中断优先级设置合理，避免影响Xmodem接收

## 故障排除

### 问题1: 未收到接收端准备信号
- **原因**: Bootloader未启动固件升级流程
- **解决**: 检查Bootloader是否正确调用`firmware_upgrade_start()`

### 问题2: 数据包校验失败
- **原因**: 串口通信干扰或波特率不匹配
- **解决**: 
  - 检查串口连接是否稳定
  - 确认波特率设置正确
  - 降低波特率重试

### 问题3: 传输超时
- **原因**: 系统响应慢或串口缓冲区不足
- **解决**: 
  - 增加超时时间
  - 检查系统负载
  - 使用128字节模式（更稳定但较慢）

### 问题4: 升级后无法运行
- **原因**: Application地址配置错误或向量表未设置
- **解决**: 
  - 确认Application起始地址为0x08001000
  - 检查Application的向量表偏移设置
  - 验证Application固件是否完整

## 示例代码

### Bootloader主程序示例

```c
#include "firmware_upgrade.h"
#include "drv_comm.h"
#include "system.h"

int main(void)
{
    // 系统初始化
    SysTick_Init(72);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // 初始化串口
    drv_usart_init(DRV_USART_ID_1, 115200, NULL);
    
    // 初始化固件升级模块
    firmware_upgrade_init(DRV_USART_ID_1);
    
    // 检查Application是否有效
    if (firmware_upgrade_is_app_valid())
    {
        // Application有效，延迟3秒后跳转（给用户时间进入升级模式）
        delay_ms(3000);
        
        // 如果3秒内收到升级命令，则不跳转
        // 这里可以添加按键检测等逻辑
        
        firmware_upgrade_jump_to_app();
    }
    else
    {
        // Application无效，等待固件升级
        firmware_upgrade_start();
    }
    
    // 主循环
    while(1)
    {
        // 处理固件升级
        firmware_upgrade_process();
        
        // 检查升级状态
        uint8_t state = firmware_upgrade_get_state();
        if (state == FW_UPGRADE_STATE_COMPLETE)
        {
            // 升级完成，跳转到Application
            delay_ms(1000);
            firmware_upgrade_jump_to_app();
        }
        
        delay_ms(10);
    }
}
```

## 参考资料

- [Xmodem协议规范](https://en.wikipedia.org/wiki/XMODEM)
- [STM32F103 Flash编程手册](https://www.st.com/resource/en/programming_manual/pm0075-stm32f10xxx-flash-memory-microcontrollers-stmicroelectronics.pdf)

## 版本历史

- **V0.0.1** (2024-01-XX)
  - 初始版本
  - 实现Xmodem协议接收
  - 实现Flash IAP操作
  - 实现固件升级管理
  - 提供PC端发送工具

