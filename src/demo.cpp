/*
 * @Author: wuyao 1955416359qq.com
 * @Date: 2024-04-24 19:35:27
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-29 08:56:07
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


int main(int, char**){
   say_hello();
   // boost::asio::io_service io_service;
   // Communication TCPComm(io_service, "192.168.51.114",1234);
   // io_service.run();

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
















}