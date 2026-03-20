#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Xmodem协议固件发送工具
用于通过串口向STM32F103C8T6发送固件文件
支持Xmodem-1K (1024字节)和Xmodem (128字节)模式
"""

import serial
import sys
import os
import time
import struct

# Xmodem协议常量
XMODEM_SOH = 0x01    # 128字节数据包起始标志
XMODEM_STX = 0x02    # 1024字节数据包起始标志
XMODEM_EOT = 0x04    # 传输结束标志
XMODEM_ACK = 0x06    # 确认应答
XMODEM_NAK = 0x15    # 否定应答
XMODEM_CAN = 0x18    # 取消传输
XMODEM_C = 0x43      # 'C' - CRC模式请求

PACKET_SIZE_128 = 128
PACKET_SIZE_1024 = 1024
TIMEOUT = 3          # 超时时间（秒）
MAX_RETRIES = 10     # 最大重试次数

def calc_crc16(data):
    """计算CRC16校验值（Xmodem CRC）"""
    crc = 0
    for byte in data:
        crc = crc ^ (byte << 8)
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc = crc << 1
        crc = crc & 0xFFFF
    return crc

def send_packet(ser, packet_num, data, use_1k=True):
    """发送一个数据包"""
    if use_1k and len(data) == PACKET_SIZE_1024:
        header = XMODEM_STX
        packet_size = PACKET_SIZE_1024
    else:
        header = XMODEM_SOH
        packet_size = PACKET_SIZE_128
        # 如果数据不足128字节，用0x1A填充
        if len(data) < packet_size:
            data = data + bytes([0x1A] * (packet_size - len(data)))
    
    # 构建数据包
    packet = bytearray()
    packet.append(header)
    packet.append(packet_num & 0xFF)
    packet.append((~packet_num) & 0xFF)
    packet.extend(data[:packet_size])
    
    # 计算CRC16
    crc = calc_crc16(data[:packet_size])
    packet.append((crc >> 8) & 0xFF)
    packet.append(crc & 0xFF)
    
    # 发送数据包
    ser.write(packet)
    ser.flush()
    
    return packet_size

def wait_for_response(ser, expected, timeout=TIMEOUT):
    """等待响应"""
    start_time = time.time()
    while time.time() - start_time < timeout:
        if ser.in_waiting > 0:
            response = ser.read(1)[0]
            if response == expected:
                return True
            elif response == XMODEM_CAN:
                print("接收端取消了传输")
                return False
            elif response == XMODEM_NAK:
                return False
        time.sleep(0.01)
    return False

def send_file(port, baudrate, filepath, use_1k=True):
    """发送文件"""
    try:
        # 打开串口
        ser = serial.Serial(port, baudrate, timeout=TIMEOUT)
        print(f"已打开串口: {port}, 波特率: {baudrate}")
        
        # 读取文件
        if not os.path.exists(filepath):
            print(f"错误: 文件不存在: {filepath}")
            return False
        
        with open(filepath, 'rb') as f:
            file_data = f.read()
        
        file_size = len(file_data)
        print(f"文件大小: {file_size} 字节")
        
        # 等待接收端发送'C'请求CRC模式
        print("等待接收端准备就绪...")
        start_time = time.time()
        while time.time() - start_time < 10:
            if ser.in_waiting > 0:
                response = ser.read(1)[0]
                if response == XMODEM_C:
                    print("收到接收端CRC模式请求，开始传输...")
                    break
            time.sleep(0.1)
        else:
            print("错误: 未收到接收端的准备信号")
            ser.close()
            return False
        
        # 计算需要发送的数据包数量
        packet_size = PACKET_SIZE_1024 if use_1k else PACKET_SIZE_128
        num_packets = (file_size + packet_size - 1) // packet_size
        
        print(f"将发送 {num_packets} 个数据包 (使用{'1K' if use_1k else '128字节'}模式)")
        
        # 发送数据包
        packet_num = 1
        bytes_sent = 0
        retry_count = 0
        
        while bytes_sent < file_size or packet_num <= num_packets:
            # 获取当前数据包的数据
            start_pos = (packet_num - 1) * packet_size
            end_pos = min(start_pos + packet_size, file_size)
            packet_data = file_data[start_pos:end_pos]
            
            # 如果数据包为空，发送填充数据
            if len(packet_data) == 0:
                packet_data = bytes([0x1A] * packet_size)
            
            # 发送数据包
            actual_size = send_packet(ser, packet_num, packet_data, use_1k)
            
            # 等待ACK
            if wait_for_response(ser, XMODEM_ACK):
                bytes_sent += actual_size
                packet_num += 1
                retry_count = 0
                
                # 显示进度
                progress = (bytes_sent * 100) // file_size if file_size > 0 else 100
                print(f"\r进度: {progress}% ({bytes_sent}/{file_size} 字节)", end='', flush=True)
                
                # 如果已经发送完所有数据，发送EOT
                if bytes_sent >= file_size:
                    break
            else:
                retry_count += 1
                if retry_count >= MAX_RETRIES:
                    print(f"\n错误: 数据包 {packet_num} 发送失败，超过最大重试次数")
                    ser.close()
                    return False
                print(f"\n警告: 数据包 {packet_num} 未收到ACK，重试 ({retry_count}/{MAX_RETRIES})")
        
        # 发送EOT（传输结束）
        print("\n发送传输结束信号...")
        ser.write(bytes([XMODEM_EOT]))
        ser.flush()
        
        # 等待最终ACK
        if wait_for_response(ser, XMODEM_ACK, timeout=5):
            print("传输完成！")
            ser.close()
            return True
        else:
            print("错误: 未收到最终ACK")
            ser.close()
            return False
            
    except serial.SerialException as e:
        print(f"串口错误: {e}")
        return False
    except Exception as e:
        print(f"错误: {e}")
        return False

def main():
    """主函数"""
    if len(sys.argv) < 3:
        print("用法: python xmodem_sender.py <串口> <波特率> <文件路径> [--128]")
        print("示例: python xmodem_sender.py COM3 115200 firmware.bin")
        print("      python xmodem_sender.py /dev/ttyUSB0 115200 firmware.bin --128")
        print("\n参数说明:")
        print("  <串口>     串口名称，Windows: COM3, Linux: /dev/ttyUSB0")
        print("  <波特率>   串口波特率，通常为115200")
        print("  <文件路径> 要发送的固件文件路径")
        print("  --128      使用128字节模式（默认使用1024字节模式）")
        sys.exit(1)
    
    port = sys.argv[1]
    baudrate = int(sys.argv[2])
    filepath = sys.argv[3]
    use_1k = '--128' not in sys.argv
    
    print("=" * 60)
    print("Xmodem固件发送工具")
    print("=" * 60)
    print(f"串口: {port}")
    print(f"波特率: {baudrate}")
    print(f"文件: {filepath}")
    print(f"模式: {'Xmodem-1K (1024字节)' if use_1k else 'Xmodem (128字节)'}")
    print("=" * 60)
    
    if send_file(port, baudrate, filepath, use_1k):
        print("\n传输成功！")
        sys.exit(0)
    else:
        print("\n传输失败！")
        sys.exit(1)

if __name__ == '__main__':
    main()

