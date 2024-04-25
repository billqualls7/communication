/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-24 19:35:46
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-24 20:35:35
 * @FilePath: /code/communication/include/communication.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <iostream>
#include "boost/asio.hpp"
#include <string>

void say_hello();


class Communication
{
private:
    /* data */
    std::string IP;
    std::string port;

public:
    Communication(std::string _IP , std::string _port);
    ~Communication();
};















#endif
