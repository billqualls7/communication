/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-24 19:32:55
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-24 20:30:58
 * @FilePath: /communication/src/communication.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "communication.h"

void say_hello(){
    std::cout << "Hello, from communication!\n";
}



Communication::Communication(std::string _IP , std::string _port)
                            :IP(_IP), port(_port)
{
    std::cout<<"IP:"<<IP<<" port:"<<port<<std::endl;
}

Communication::~Communication()
{
}


