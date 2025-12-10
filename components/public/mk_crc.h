#ifndef _MK_CRC_H_
#define _MK_CRC_H_

#include "stdint.h"
#include "stdbool.h"

// 基于C语言的CRC校验库，包括常用的21个CRC参数模型实现
// http://www.ip33.com/crc.html

/*
| 模型名称           | 多项式公式                                                                      | 宽度   | 多项式   | 初始值   | 结果异或 | 输入反转 | 输出反转 |
| Name               | Polynomial Formula                                                              | Width  | Poly     | Init     | Xorout   | Refin    | Refout   |
| ------------------ | ------------------------------------------------------------------------------- | ------ | -------- | -------- | -------- | -------- | -------- |
| CRC-4/ITU          | x4 + x + 1                                                                      | 4      | 03       | 00       | 00       | TRUE     | TRUE     |
| CRC-5/EPC          | x5 + x3 + 1                                                                     | 5      | 09       | 09       | 00       | FALSE    | FALSE    |
| CRC-5/ITU          | x5 + x4 + x2 + 1                                                                | 5      | 15       | 00       | 00       | TRUE     | TRUE     |
| CRC-5/USB          | x5 + x2 + 1                                                                     | 5      | 05       | 1F       | 1F       | TRUE     | TRUE     |
| CRC-6/ITU          | x6 + x + 1                                                                      | 6      | 03       | 00       | 00       | TRUE     | TRUE     |
| CRC-7/MMC          | x7 + x3 + 1                                                                     | 7      | 09       | 00       | 00       | FALSE    | FALSE    |
| CRC-8              | x8 + x2 + x + 1                                                                 | 8      | 07       | 00       | 00       | FALSE    | FALSE    |
| CRC-8/ITU          | x8 + x2 + x + 1                                                                 | 8      | 07       | 00       | 55       | FALSE    | FALSE    |
| CRC-8/ROHC         | x8 + x2 + x + 1                                                                 | 8      | 07       | FF       | 00       | TRUE     | TRUE     |
| CRC-8/MAXIM        | x8 + x5 + x4 + 1                                                                | 8      | 31       | 00       | 00       | TRUE     | TRUE     |
| CRC-16/IBM         | x16 + x15 + x2 + 1                                                              | 16     | 8005     | 0000     | 0000     | TRUE     | TRUE     |
| CRC-16/MAXIM       | x16 + x15 + x2 + 1                                                              | 16     | 8005     | 0000     | FFFF     | TRUE     | TRUE     |
| CRC-16/USB         | x16 + x15 + x2 + 1                                                              | 16     | 8005     | FFFF     | FFFF     | TRUE     | TRUE     |
| CRC-16/MODBUS      | x16 + x15 + x2 + 1                                                              | 16     | 8005     | FFFF     | 0000     | TRUE     | TRUE     |
| CRC-16/CCITT       | x16 + x12 + x5 + 1                                                              | 16     | 1021     | 0000     | 0000     | TRUE     | TRUE     |
| CRC-16/CCITT-FALSE | x16 + x12 + x5 + 1                                                              | 16     | 1021     | FFFF     | 0000     | FALSE    | FALSE    |
| CRC-16/X25         | x16 + x12 + x5 + 1                                                              | 16     | 1021     | FFFF     | FFFF     | TRUE     | TRUE     |
| CRC-16/XMODEM      | x16 + x12 + x5 + 1                                                              | 16     | 1021     | 0000     | 0000     | FALSE    | FALSE    |
| CRC-16/DNP         | x16 + x13 + x12 + x11 + x10 + x8 + x6 + x5 +  x2 + 1                            | 16     | 3D65     | 0000     | FFFF     | TRUE     | TRUE     |
| CRC-32             | x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 +  x8 + x7 + x5 + x4 + x2 + x + 1 | 32     | 04C11DB7 | FFFFFFFF | FFFFFFFF | TRUE     | TRUE     |
| CRC-32/MPEG-2      | x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 +  x8 + x7 + x5 + x4 + x2 + x + 1 | 32     | 04C11DB7 | FFFFFFFF | 0        | FALSE    | FALSE    |
*/

uint8_t  mk_crc4_itu(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint8_t  mk_crc5_epc(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint8_t  mk_crc5_itu(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint8_t  mk_crc5_usb(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint8_t  mk_crc6_itu(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint8_t  mk_crc7_mmc(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint8_t  mk_crc8(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint8_t  mk_crc8_itu(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint8_t  mk_crc8_rohc(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint8_t  mk_crc8_maxim(uint8_t *data, uint16_t length, bool add_before_flag, uint8_t before_data);
uint16_t mk_crc16_ibm(uint8_t *data, uint16_t length, bool add_before_flag, uint16_t before_data);
uint16_t mk_crc16_maxim(uint8_t *data, uint16_t length, bool add_before_flag, uint16_t before_data);
uint16_t mk_crc16_usb(uint8_t *data, uint16_t length, bool add_before_flag, uint16_t before_data);
uint16_t mk_crc16_modbus(uint8_t *data, uint16_t length, bool add_before_flag, uint16_t before_data);
uint16_t mk_crc16_ccitt(uint8_t *data, uint16_t length, bool add_before_flag, uint16_t before_data);
uint16_t mk_crc16_ccitt_false(uint8_t *data, uint16_t length, bool add_before_flag, uint16_t before_data);
uint16_t mk_crc16_x25(uint8_t *data, uint16_t length, bool add_before_flag, uint16_t before_data);
uint16_t mk_crc16_xmodem(uint8_t *data, uint16_t length, bool add_before_flag, uint16_t before_data);
uint16_t mk_crc16_dnp(uint8_t *data, uint16_t length, bool add_before_flag, uint16_t before_data);
uint32_t mk_crc32(uint8_t *data, uint16_t length, bool add_before_flag, uint32_t before_data);
uint32_t mk_crc32_mpeg_2(uint8_t *data, uint16_t length, bool add_before_flag, uint32_t before_data);

#endif /* _MK_CRC_H_ */

/*

代码：
#include "stdio.h"
#include "mk_crc.h"

int main(void)
{
    uint8_t raw_data_1[4] = {0x12, 0x74, 0x5F, 0x19};
    uint8_t raw_data_2[4] = {0xDA, 0xAB, 0x26, 0x0E};
    uint8_t raw_data_all[8] = {raw_data_1[0], raw_data_1[1], raw_data_1[2], raw_data_1[3], raw_data_2[0], raw_data_2[1], raw_data_2[2], raw_data_2[3]};

    uint16_t out_data_all = mk_crc16_usb(raw_data_all, sizeof(raw_data_all));
    uint16_t out_data_all_add = mk_crc16_usb(raw_data_2, sizeof(raw_data_2), true, mk_crc16_usb(raw_data_1, sizeof(raw_data_1)));

    printf("out_data_all : %#X\r\n", out_data_all);
    printf("out_data_all_add : %#X\r\n", out_data_all_add);
    printf("All Success!\r\n");

    return 0;
}

输出：
out_data_all : 0X17CB
out_data_all_add : 0X17CB
All Success!

*/
