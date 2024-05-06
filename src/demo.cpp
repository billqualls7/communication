/*
 * @Author: wuyao 1955416359qq.com
 * @Date: 2024-04-24 19:35:27
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-05-06 19:40:47
 * @FilePath: /code/communication/src/demo.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "communication.h"
#include <iostream>
#include <chrono>



unsigned char xorChecksum(const unsigned char *data, size_t length) {
    unsigned char checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum ^= data[i];
    }
    return checksum;
}



std::vector<unsigned char> concatenateKnownLengthVectors(const std::vector<std::vector<unsigned char>>& vecs, size_t totalLength) {
    std::vector<unsigned char> result;
    result.reserve(totalLength);

    size_t currentLength = 0; // 当前已拼接的长度

    // 遍历所有向量并拼接它们
    for (const auto& vec : vecs) {
        // 计算可以拼接的当前向量的最大长度
        size_t vecSize = std::min(vec.size(), totalLength - currentLength);
        // 插入当前向量到结果向量
        result.insert(result.end(), vec.begin(), vec.begin() + vecSize);
        // 更新当前已拼接的长度
        currentLength += vecSize;
        // 如果已经达到总长度，就不需要继续拼接了
        if (currentLength == totalLength) {
            break;
        }
    }

    return result;
}


void Float_to_Byte(const float f, unsigned char* byte) {
    // 使用union来安全地重新解释浮点数的内存表示
    union {
        float f;
        uint32_t i;
    } float_union;

    float_union.f = f; // 将浮点数赋值给union的浮点数成员
    uint32_t longdata = float_union.i; // 然后安全地获取整数表示

    // 使用整数的位操作来设置字节数组
    byte[0] = (longdata & 0xFF000000) >> 24;
    byte[1] = (longdata & 0x00FF0000) >> 16;
    byte[2] = (longdata & 0x0000FF00) >> 8;
    byte[3] = longdata & 0x000000FF;
}

float Byte_to_Float(const char *p) {
    // 使用union来安全地重新解释字节的内存表示
	union {
            uint32_t i;
            float f;
        } float_union;
    float out_float;    
    uint32_t longdata = (static_cast<uint32_t>(p[0]) << 24) |
                        (static_cast<uint32_t>(p[1]) << 16) |
                        (static_cast<uint32_t>(p[2]) << 8) |
                            static_cast<uint32_t>(p[3]);

    float_union.i = longdata; // 将整数赋值给union的整数成员

    
    out_float = float_union.f;
    return out_float; // 然后获取浮点数
}


int main(int, char**){
   say_hello();
   boost::asio::io_service io_service;
   Communication TCPComm(io_service, "8.135.10.183","39294");
   io_service.run();

   unsigned char packet[] = {0x05,0x0a,0x05,0x0a,
                             0x01,0x03,0x0d,0x00,
                             0x00,0x26,0x00,0x01,
                             0x00,0x01,0x19,0x00,
                             0x00,0x00,0x00
                             };



   unsigned char checksum = xorChecksum(packet, sizeof(packet) - 1);
   std::cout << std::hex << static_cast<int>(checksum) << std::endl;

   Send_VelMessage SENDdemo;
                                             
   if (! SENDdemo.BCC_Check_MessBody.empty() && !SENDdemo.BCC_Check_Front19.empty())
   {   std::vector<unsigned char> fullMessage = concatenateKnownLengthVectors(SENDdemo.Vel_Mess, 32);
         for (unsigned char byte : fullMessage) {
            std::cout << std::hex << static_cast<int>(byte) << " ";
         }
    std::cout << std::endl;
   }

   SENDdemo.BCC_Check_MessBody.push_back(0x19);
   SENDdemo.BCC_Check_Front19.push_back(0xa0);
   std::cout << std::hex << static_cast<int>(SENDdemo.BCC_Check_Front19.data()[0]) << std::endl;
   SENDdemo.updateVelMess();
   if (! SENDdemo.BCC_Check_MessBody.empty() && !SENDdemo.BCC_Check_Front19.empty())
   {   std::vector<unsigned char> fullMessage = concatenateKnownLengthVectors(SENDdemo.Vel_Mess, 32);
         for (unsigned char byte : fullMessage) {
            std::cout << std::hex << static_cast<int>(byte) << " ";
         }
    std::cout << std::endl;
   }





    float myFloat = 3.14159;
    unsigned char byte[4]; // 定义一个字节数组来存储转换结果

    Float_to_Byte(myFloat, byte);
    for (int i = 0; i < 4; i++) {
        std::cout << std::hex << static_cast<int>(byte[i]) << " ";

    }
    std::cout << std::endl;

    // float float_data;
    // char cbyte = static_cast<char>(byte);
    // float_data = Byte_to_Float(byte);
    // std::cout<<std::dec<< float_data<<std::endl;

    









}