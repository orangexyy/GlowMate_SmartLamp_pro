# Neuron 协议栈中间件
## 概述
Neuron协议栈是深圳市创客工场科技有限公司(MakeBlock)设计的嵌入式通信协议中间件，主要用于MCU设备间的通信。该协议栈支持多种协议格式，提供统一的管理接口，简化了设备间通信的开发复杂度。

**主要特点：**
- 支持F0F7、F3F4、F1F6、F8F9等多种协议格式
- 提供多端口统一管理
- 内置固件升级功能
- 支持数据流控制和路由转发
- 自动缓冲区管理

**参考文档：**
- [Neuron协议规范](https://makeblock.feishu.cn/wiki/PnQdwNQl9ilUNEktwIKcn5nUnPe)
- [Neuron协议详细文档](https://makeblock.feishu.cn/docx/TQaEdL57Ko3bOexga3GcNCPCnbh)

## 功能特性
- **多协议支持**: 支持F0F7、F3F4、F1F6、F8F9等多种协议格式
- **多端口管理**: 支持多端口的统一管理
- **缓冲区管理**: 自动管理接收和发送缓冲区
- **协议路由**: 支持数据转发和路由功能
- **升级支持**: 内置固件升级功能
- **流控制**: 支持数据流控制机制

## 目录结构
```
neuron/
├── neuron_manager.c              # 协议栈管理器主文件
├── neuron_manager.h              # 管理器头文件
├── neuron_config_def_example.h   # 配置定义示例文件
├── protocol_f0f7/                # F0F7协议实现
│   ├── neuron_link_f0f7.c        # F0F7链路层实现
│   ├── neuron_link_f0f7.h        # F0F7链路层头文件
│   ├── neuron_general_f0f7.c     # F0F7通用命令处理
│   ├── neuron_general_f0f7.h     # F0F7通用命令头文件
│   ├── neuron_system_f0f7.c      # F0F7系统命令处理
│   ├── neuron_system_f0f7.h      # F0F7系统命令头文件
│   ├── neuron_user_f0f7.c        # F0F7用户层实现
│   ├── neuron_user_f0f7.h        # F0F7用户层头文件
│   ├── neuron_stream_f0f7.c      # F0F7流数据处理
│   ├── neuron_stream_f0f7.h      # F0F7流数据头文件
│   ├── router_dispatch_f0f7.c    # F0F7路由分发
│   └── router_dispatch_f0f7.h    # F0F7路由分发头文件
└── protocol_f3f4/                # F3F4协议实现
    ├── neuron_link_f3f4.c        # F3F4链路层实现
    ├── neuron_link_f3f4.h        # F3F4链路层头文件
    ├── neuron_stream_f3f4.c      # F3F4流数据处理
    ├── neuron_stream_f3f4.h      # F3F4流数据头文件
    ├── router_dispatch_f3f4.c    # F3F4路由分发
    └── router_dispatch_f3f4.h    # F3F4路由分发头文件
```

## 架构设计
### 分层架构
Neuron协议栈采用分层设计，每层负责特定的功能，层与层之间通过标准接口进行交互。
```
┌─────────────────────────────────────────────────────────────┐
│                    应用层 (Application Layer)                │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐ │
│  │   MCODE     │ │   GCODE     │ │   升级      │ │  其他   │ │
│  │   处理      │ │   处理       │ │   功能      │ │  应用   │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘ │
└─────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────┐
│                    路由层 (Router Layer)                    │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐ │
│  │   消息      │ │   地址      │ │   类型      │ │  转发   │ │
│  │   路由      │ │   解析      │ │   匹配      │ │  处理   │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘ │
└─────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────┐
│                    链路层 (Link Layer)                      │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐ │
│  │   帧封装    │ │   帧解析    │ │   校验      │ │  流控   │ │
│  │   和发送    │ │   和接收    │ │   检测      │ │  制     │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘ │
└─────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────┐
│                    管理层 (Manager Layer)                   │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐ │
│  │   端口      │ │   协议      │ │   缓冲区    │ │  调度   │ │
│  │   管理      │ │   绑定      │ │   管理      │ │  处理   │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘ │
└─────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────┐
│                    硬件层 (Hardware Layer)                  │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐ │
│  │   UART1     │ │   UART2     │ │   SPI1      │ │  CAN    │ │
│  │   端口      │ │   端口      │ │   端口       │ │  端口   │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘ │
└─────────────────────────────────────────────────────────────┘
```
### 各层功能说明
#### 1. 管理层 (Manager Layer)
- **端口管理**: 负责多端口的注册、配置和生命周期管理
- **协议绑定**: 将不同的协议类型绑定到指定的端口
- **缓冲区管理**: 统一管理各端口的接收和发送缓冲区
- **调度处理**: 协调各端口的数据处理优先级
#### 2. 链路层 (Link Layer)
- **帧封装**: 将上层数据封装成符合协议规范的帧格式
- **帧解析**: 从接收到的原始数据中解析出有效帧
- **校验检测**: 进行数据完整性校验和错误检测
- **流控制**: 实现数据流控制，防止缓冲区溢出
#### 3. 路由层 (Router Layer)
- **消息路由**: 根据目标地址将消息路由到正确的处理模块
- **地址解析**: 解析设备地址、类型和子类型信息
- **类型匹配**: 根据消息类型选择相应的处理函数
- **转发处理**: 支持消息的本地处理和跨端口转发
#### 4. 应用层 (Application Layer)
- **MCODE处理**: 处理MCODE格式的命令和响应
- **GCODE处理**: 处理GCODE格式的命令和响应
- **升级功能**: 提供固件升级相关的命令处理
- **其他应用**: 支持自定义应用层协议扩展

## 快速开始
### 1. 配置设置
首先创建配置文件 `neuron_config_def.h`，参考 `neuron_config_def_example.h`：
```c
// 设备基本信息配置
#define NEURON_DEVICE_ADDRESS               (0x21)      // 设备地址 - 每个设备必须唯一
#define NEURON_DEVICE_TYPE                  (0x73)      // 产品类型 - 标识产品大类  
#define NEURON_DEVICE_SUB_TYPE              (0x11)      // 产品子类型 - 标识具体型号

// 协议栈基础配置
#define NEURON_MAX_PORT_NUM                 (4)         // 最大端口数
#define NEURON_F0F7_MAX_NUM                 (4)         // F0F7协议支持数量
#define NEURON_F3F4_MAX_NUM                 (4)         // F3F4协议支持数量
#define NEURON_ONCE_PROCESS_LEN             (256)       // 单次处理最大数据长度

// F0F7协议配置
#define NEURON_F0F7_LINK_CMD_AUTO_REPLY     (0)         // 链路层指令自动回复
#define NEURON_F0F7_CMD_REPLY_LEN           (256)       // 指令回复最大长度
#define NEURON_F0F7_STREAM_ENABLE           (0)         // 流链路使能
#define NEURON_F0F7_STREAM_ONCE_PROCESS_LEN (256)       // 流链路单次处理长度

// F3F4协议配置
#define NEURON_F3F4_CMD_REPLY_LEN           (256)       // 指令回复最大长度
#define NEURON_F3F4_FLOW_LEFT_COUNT_THRES   (9024)      // 流控剩余阈值
```
### 2. 初始化协议栈
```c
#include "neuron_manager.h"

// 发送回调函数 - 必须实现
uint32_t communication_send_callback(uint8_t port_id, uint8_t *data, uint16_t len) {
    // 根据port_id选择对应的硬件接口发送数据
    switch(port_id) {
        case PORT_ID_UART1:
            return uart1_send(data, len);
        case PORT_ID_UART2:
            return uart2_send(data, len);
        // ... 其他端口
        default:
            return 0;
    }
}

// 转发回调函数 - 可选实现
uint32_t ommunication_forward_callback(uint8_t port_id, uint8_t protocol_type, 
                            uint8_t *data, uint16_t len) {
    // 实现数据的转发
    // 根据port_id选择对应的硬件接口发送数据
    return len;
}

// 初始化协议栈
int ret = neuron_manager_init(communication_send_callback, ommunication_forward_callback);
if (ret != MK_SUCCESS) {
    // 处理初始化错误
    printf("Neuron manager init failed: %d\n", ret);
}
```
### 3. 配置端口和协议=
```c
// 定义缓冲区 - 建议大小：接收2048字节，发送1024字节
uint8_t uart1_rx_buffer[2048];
uint8_t uart1_tx_buffer[1024];
uint8_t uart2_rx_buffer[2048];
uint8_t uart2_tx_buffer[1024];

// 安装端口
ret = neuron_manager_install_port(PORT_ID_UART1, 
                                 uart1_rx_buffer, sizeof(uart1_rx_buffer),
                                 uart1_tx_buffer, sizeof(uart1_tx_buffer));
if (ret != MK_SUCCESS) {
    printf("Install UART1 port failed: %d\n", ret);
}

ret = neuron_manager_install_port(PORT_ID_UART2, 
                                 uart2_rx_buffer, sizeof(uart2_rx_buffer),
                                 uart2_tx_buffer, sizeof(uart2_tx_buffer));
if (ret != MK_SUCCESS) {
    printf("Install UART2 port failed: %d\n", ret);
}

// 定义协议接收缓冲区
uint8_t f0f7_rx_buffer[512];
uint8_t f3f4_rx_buffer[512];

// 绑定协议到端口
ret = neuron_manager_bind_protocol(PORT_ID_UART1, PROTOCOL_TYPE_F0F7, 
                                  f0f7_rx_buffer, sizeof(f0f7_rx_buffer));
if (ret != MK_SUCCESS) {
    printf("Bind F0F7 protocol to UART1 failed: %d\n", ret);
}

ret = neuron_manager_bind_protocol(PORT_ID_UART2, PROTOCOL_TYPE_F3F4, 
                                  f3f4_rx_buffer, sizeof(f3f4_rx_buffer));
if (ret != MK_SUCCESS) {
    printf("Bind F3F4 protocol to UART2 failed: %d\n", ret);
}
```
### 4. 设置回调函数（F0F7协议）
```c
// MCODE命令处理函数
void mcode_handler(uint8_t port_id, char *data, uint16_t len, char *reply_data) {
    printf("Received MCODE on port %d: %.*s\n", port_id, len, data);
    
    // 处理MCODE命令
    if (strncmp(data, "M114", 4) == 0) {
        // 处理位置查询命令
        snprintf(reply_data, 256, "X:100.0 Y:200.0 Z:50.0");
    } else if (strncmp(data, "M105", 4) == 0) {
        // 处理温度查询命令
        snprintf(reply_data, 256, "T:25.0 B:25.0");
    } else {
        // 未知命令
        snprintf(reply_data, 256, "error:Unknown command");
    }
}

// GCODE命令处理函数
void gcode_handler(uint8_t port_id, char *data, uint16_t len, char *reply_data) {
    printf("Received GCODE on port %d: %.*s\n", port_id, len, data);
    
    // 处理GCODE命令
    if (strncmp(data, "G28", 3) == 0) {
        // 处理回零命令
        snprintf(reply_data, 256, "ok");
    } else if (strncmp(data, "G1", 2) == 0) {
        // 处理移动命令
        snprintf(reply_data, 256, "ok");
    } else {
        // 未知命令
        snprintf(reply_data, 256, "error:Unknown command");
    }
}

// 升级回调函数
int upgrade_callback(uint8_t port_id, TS_NEURON_GENERAL_CMD cmd, 
                    uint8_t *data, uint16_t len, uint8_t *response, uint16_t *response_len) {
    switch(cmd) {
        case NEURON_GENERAL_CMD_UPGRADE_START:
            // 开始升级
            printf("Upgrade start on port %d\n", port_id);
            *response_len = 0;
            return REPLY_SUCC;
            
        case NEURON_GENERAL_CMD_UPGRADE_DATA:
            // 处理升级数据
            printf("Upgrade data on port %d, len: %d\n", port_id, len);
            // 这里应该将数据写入Flash
            *response_len = 0;
            return REPLY_SUCC;
            
        case NEURON_GENERAL_CMD_UPGRADE_END:
            // 结束升级
            printf("Upgrade end on port %d\n", port_id);
            *response_len = 0;
            return REPLY_SUCC;
            
        default:
            return REPLY_FAIL;
    }
}

// 注册回调函数, 根据项目实际功能注册
neuron_user_set_mcode_handler(mcode_handler);
neuron_user_set_gcode_handler(gcode_handler);
neuron_general_set_upgrade_callback(upgrade_callback);
```
### 5. 循环处理
```c
// 在主循环中调用处理函数
void main_loop(void) {
    while(1) {
        // 处理所有端口的数据 - 必须定期调用
        neuron_manager_process();
        
        // 其他任务处理
        system_task_process();
        user_task_process();
    }
}

// 在接收中断中调用接收函数
void uart1_rx_handler(uint8_t *data, uint16_t len) {
    // 将接收到的数据传递给协议栈
    neuron_manager_receive(PORT_ID_UART1, data, len, false);
}

void uart2_rx_handler(uint8_t *data, uint16_t len) {
    // 将接收到的数据传递给协议栈
    neuron_manager_receive(PORT_ID_UART2, data, len, false);
}
```

## API 参考
### 核心管理函数

| 函数名 | 功能描述 | 参数说明 | 返回值 |
|--------|----------|----------|--------|
| `neuron_manager_init()` | 初始化协议栈管理器 | send_cb: 发送回调<br>forward_cb: 转发回调 | MK_SUCCESS/MK_FAIL |
| `neuron_manager_install_port()` | 安装新端口 | port_id: 端口ID<br>rx_buf: 接收缓冲区<br>tx_buf: 发送缓冲区 | MK_SUCCESS/MK_FAIL |
| `neuron_manager_bind_protocol()` | 绑定协议支持 | port_id: 端口ID<br>protocol_type: 协议类型<br>rx_buf: 协议接收缓冲区 | MK_SUCCESS/MK_FAIL |
| `neuron_manager_receive()` | 接收数据 | port_id: 端口ID<br>data: 数据指针<br>len: 数据长度<br>is_forward: 是否转发 | MK_SUCCESS/MK_FAIL |
| `neuron_manager_process()` | 处理数据 | 无 | 无 |
| `neuron_manager_send_raw_data()` | 发送原始数据 | port_id: 端口ID<br>data: 数据指针<br>len: 数据长度 | MK_SUCCESS/MK_FAIL |
### F0F7协议函数
| 函数名 | 功能描述 | 参数说明 | 返回值 |
|--------|----------|----------|--------|
| `neuron_link_f0f7_register()` | 注册F0F7协议 | 无 | MK_SUCCESS/MK_FAIL |
| `neuron_link_f0f7_unpack()` | 解析F0F7数据 | data: 输入数据<br>len: 数据长度<br>frame: 输出帧结构 | MK_SUCCESS/MK_FAIL |
| `neuron_link_f0f7_pack_reply()` | 封装F0F7回复 | frame: 帧结构<br>data: 输出数据<br>len: 数据长度 | MK_SUCCESS/MK_FAIL |
| `neuron_user_set_mcode_handler()` | 设置MCODE处理函数 | handler: 处理函数指针 | 无 |
| `neuron_user_set_gcode_handler()` | 设置GCODE处理函数 | handler: 处理函数指针 | 无 |
### F3F4协议函数
| 函数名 | 功能描述 | 参数说明 | 返回值 |
|--------|----------|----------|--------|
| `neuron_link_f3f4_register()` | 注册F3F4协议 | 无 | MK_SUCCESS/MK_FAIL |
| `neuron_link_f3f4_unpack()` | 解析F3F4数据 | data: 输入数据<br>len: 数据长度<br>frame: 输出帧结构 | MK_SUCCESS/MK_FAIL |
| `neuron_link_f3f4_pack_reply()` | 封装F3F4回复 | frame: 帧结构<br>data: 输出数据<br>len: 数据长度 | MK_SUCCESS/MK_FAIL |
| `neuron_link_f3f4_pack_raw()` | 封装F3F4原始数据 | data: 输入数据<br>len: 数据长度<br>output: 输出数据 | MK_SUCCESS/MK_FAIL |

## 配置建议
### 缓冲区大小配置
| 缓冲区类型 | 最小建议 | 推荐大小 | 考虑因素 |
|------------|----------|----------|----------|
| 接收缓冲区 (rx_buffer) | 1024 字节 | 2048-4096 字节 | 最大数据包大小、处理延迟、波特率 |
| 发送缓冲区 (tx_buffer) | 512 字节 | 1024-2048 字节 | 发送频率、响应时间要求 |
| 协议接收缓冲区 | 256 字节 | 512-1024 字节 | 单次处理数据长度、协议开销 |
### 性能优化建议
1. **处理频率**: `neuron_manager_process()` 建议调用频率不低于1kHz
2. **缓冲区管理**: 根据实际数据量调整缓冲区大小，避免内存浪费
3. **错误处理**: 及时检查函数返回值，实现错误恢复机制
4. **线程安全**: 在多线程环境中使用互斥锁保护共享资源

## 注意事项
1. **内存管理**: 确保为每个端口分配足够的缓冲区空间，避免数据丢失
2. **线程安全**: 在多线程环境中使用时需要注意线程安全，使用互斥锁保护共享资源
3. **错误处理**: 及时检查函数返回值，实现完善的错误处理和恢复机制
4. **配置一致性**: 确保配置文件中的参数与实际使用场景匹配，特别是设备地址必须唯一
5. **协议兼容性**: 不同协议版本之间可能存在兼容性问题，建议使用最新版本
6. **性能考虑**: 根据实际应用需求调整缓冲区大小和处理频率
