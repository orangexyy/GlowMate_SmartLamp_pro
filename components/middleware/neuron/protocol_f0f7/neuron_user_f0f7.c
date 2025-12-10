/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_router.c
**  功能描述 :  路由层负责根据消息的地址、类型和子类型决定消息的处理方式，
                包括本地处理或转发到其他设备。
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/  

/****************************************************************************\
                               Includes
\****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "neuron_config_def.h"
#include "neuron_link_f0f7.h"
#include "neuron_user_f0f7.h"
#include "byte_buffer.h"
#include "mk_log.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
 // 命令ID
#define CMD_SET_MCODE 0x01
#define CMD_BLOCK_DATA_TRANS_HEAD 0x10
#define CMD_BLOCK_DATA_TRANS_PACKAGE 0x11
#define FILE_NAME_MAX_LEN 64

#define CMD_REPLY_OK 0
#define CMD_REPLY_ERROR 1

#define NEURON_PROTOCOL_M_CMD_ID            (0x01)                   //神经元M指令
#define NEURON_PROTOCOL_M_CMD_SN            (0x00)                   //神经元M指令 SN
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

typedef struct
{ 
    uint32_t totalSize;         // 总长度
    uint32_t rcvSize;           // 已经传输的数据
    uint32_t packageIdx;        // 包序
    uint8_t blockDataType;      // 块数据类型
    char fileName[FILE_NAME_MAX_LEN]; // 文件名
} TS_BLOCK_TRANS_MANAGER;

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static neuron_user_mcode_handler_t s_mcode_handler = NULL;
static neuron_user_gcode_handler_t s_gcode_handler = NULL;
static neuron_user_block_handler_t s_block_handler = NULL;
static TS_BLOCK_TRANS_MANAGER blockTrans = {0};
/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/**
 * @brief 回复命令
 * @param port_id 端口ID
 * @param cmd 命令ID
 * @param status 状态
 */
static bool neuron_user_reply(uint8_t port_id, uint8_t cmd, uint8_t status)
{
    uint8_t data[2] = {cmd, status};
    return neuron_link_f0f7_pack(port_id, data, sizeof(data));
}

/**
 * @brief 分发命令
 * @param port_id 端口ID
 * @param cmd_data 命令数据
 * @param cmd_len 命令长度
 * @param reply_data 回复数据
 */
void neuron_user_cmd_process(uint8_t port_id, char *cmd_data, uint16_t cmd_len, char *reply_data)
{
    if (cmd_data[0] == 'M' && s_mcode_handler != NULL)
    {
        s_mcode_handler(port_id, cmd_data, cmd_len, reply_data);
    }
    else if (cmd_data[0] == 'G' && s_gcode_handler != NULL)
    {   
        s_gcode_handler(port_id, cmd_data, cmd_len, reply_data);
    }
}

/**
 * @brief 分发命令
 * @param port_id 端口ID
 * @param cmd_data 命令数据
 * @param cmd_len 命令长度
 * @param reply_data 回复数据
 */
void neuron_user_dispatch_gcode(uint8_t port_id, char *cmd_data, uint16_t cmd_len)
{
    uint16_t reply_len;
    char reply_data[NEURON_F0F7_CMD_REPLY_LEN + 2] = {0};
    
    neuron_user_cmd_process(port_id, cmd_data, cmd_len, reply_data+2);

    reply_len = strlen(reply_data + 2);
    if (reply_len > NEURON_F0F7_CMD_REPLY_LEN) // 回复数据长度超过最大长度, 卡死, 需要修改
    {
        LOG_E("reply len too long, %d. max is %d", reply_len, NEURON_RAW_CMD_REPLY_LEN);
        while (1);
    }

    if (reply_len > 0)  
    {
        reply_data[0] = NEURON_PROTOCOL_M_CMD_ID;
        reply_data[1] = NEURON_PROTOCOL_M_CMD_SN;
        neuron_link_f0f7_pack(port_id, (uint8_t *)reply_data, reply_len + 2);
    }
}

/**
 * @brief 分发命令
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_user_cmd_dispatch(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint16_t cmd_head_pos = 0;
    uint16_t cmd_len = 0;
    uint8_t *cmd_start = NULL;

    for (uint16_t cmd_index = 0; cmd_index < len; cmd_index++)
    { 
        if (data[cmd_index] == '\n')
        {
            data[cmd_index] = '\0'; // 把换行符替换为结束符, 方便后续处理
            
            cmd_len = cmd_index - cmd_head_pos;
            cmd_start = data + cmd_head_pos;
            
            if (cmd_len > 0) { 
                neuron_user_dispatch_gcode(port_id, (char *)cmd_start, cmd_len);
            }
            cmd_head_pos = cmd_index + 1;
        }
    }
}

/**
 * @brief 块数据传输
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_user_block(uint8_t port_id, uint8_t cmd_id, uint8_t *data, uint16_t len)
{ 
    BYTE_BUFFER byte_buffer; 
    byte_buffer_init(&byte_buffer, data, len, 0);

    if (cmd_id == CMD_BLOCK_DATA_TRANS_HEAD)
    {
        long blockTotalSize = 0;
        
        // 块数据传输头处理
        memset(&blockTrans, 0, sizeof(TS_BLOCK_TRANS_MANAGER));
 
        byte_buffer_read_byte_8(&byte_buffer, &blockTrans.blockDataType);
        byte_buffer_read_long(&byte_buffer, &blockTotalSize);
        
        blockTrans.totalSize = blockTotalSize;
        
        uint16_t fileNameLen = len - byte_buffer.index;
        fileNameLen = fileNameLen > FILE_NAME_MAX_LEN ? FILE_NAME_MAX_LEN : fileNameLen;

        for(uint32_t i = 0; i < fileNameLen; i++)
        {
            byte_buffer_read_byte_8(&byte_buffer, (uint8_t*)blockTrans.fileName + i);
        }
        
        if (s_block_handler != NULL) {
            s_block_handler(NEURON_BLOCK_START, blockTrans.fileName, NULL, 0);
        }
        neuron_user_reply(port_id, cmd_id, CMD_REPLY_OK);
    }
    else if (cmd_id == CMD_BLOCK_DATA_TRANS_PACKAGE)
    {
        // 块数据传输包
        long packageIdx;

        byte_buffer_read_long(&byte_buffer, &packageIdx);

        if(blockTrans.packageIdx == packageIdx) // 包序号正确
        {
            blockTrans.packageIdx++;
            if (s_block_handler != NULL) {
                blockTrans.rcvSize = s_block_handler(NEURON_BLOCK_ING, blockTrans.fileName, data, len);
                if (blockTrans.rcvSize == blockTrans.totalSize)
                {
                    s_block_handler(NEURON_BLOCK_END, blockTrans.fileName, NULL, 0);
                }
            }
            neuron_user_reply(port_id, cmd_id, CMD_REPLY_OK);
        } 
        else 
        {
            neuron_user_reply(port_id, cmd_id, CMD_REPLY_ERROR);
        }
    }
}

/**
 * @brief 本设备类型命令订阅者
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
void neuron_user_subscriber(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint8_t cmd_id = data[0];
    if (cmd_id == CMD_SET_MCODE) {
        neuron_user_cmd_dispatch(port_id, data+2, len-2);// 分发命令, 去掉cmd_id和SN
    }
    else if (cmd_id == CMD_BLOCK_DATA_TRANS_HEAD || cmd_id == CMD_BLOCK_DATA_TRANS_PACKAGE) {
        neuron_user_block(port_id, cmd_id, data+1, len-1);// 块数据传输, 去掉cmd_id
    }
}

/**
 * @brief 设置MCODE命令处理函数
 * @param handler 处理函数指针
 */
void neuron_user_set_mcode_handler(neuron_user_mcode_handler_t handler)
{
    s_mcode_handler = handler;
}

/**
 * @brief 设置GCODE命令处理函数
 * @param handler 处理函数指针
 */
void neuron_user_set_gcode_handler(neuron_user_gcode_handler_t handler)
{
    s_gcode_handler = handler;
}

/**
 * @brief 设置块数据传输处理函数
 * @param handler 处理函数指针
 */
void neuron_user_set_block_handler(neuron_user_block_handler_t handler)
{
    s_block_handler = handler;
}

/******************************* End of File (C）****************************/
