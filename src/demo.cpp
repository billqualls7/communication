/*
 * @Author: wuyao 1955416359qq.com
 * @Date: 2024-04-24 19:35:27
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-26 05:04:02
 * @FilePath: /code/communication/src/demo.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "communication.h"
#include <iostream>
#include <chrono>



int main(int, char**){
   say_hello();
   boost::asio::io_service io_service;
   Communication TCPComm(io_service, "118.202.30.226",1234);
   
   // TCPComm.async_read();
   io_service.run();

   // while (1)
   // {
   //    TCPComm.status_analyze();
   // }
   


}