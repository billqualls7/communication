/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-24 19:35:46
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-25 09:20:09
 * @FilePath: /code/communication/include/communication.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <iostream>
#include "boost/asio.hpp"
#include <string>
#include <boost/bind.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/array.hpp>
void say_hello();

using boost::asio::ip::tcp;
class Communication
{

    
private:
    /* data */



    void do_connect(const std::string& host, short port);
    void handle_resolve(const boost::system::error_code& error, 
                        tcp::resolver::iterator endpoint_iterator);
    void handle_connect(const boost::system::error_code& error);
    void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred);


public:
    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::array<char, 1024> read_buffer_;

    Communication(boost::asio::io_service& io_service, const std::string& host, short port);
    ~Communication();
    void async_read();
    // virtual void async_write() = 0;

    
};















#endif
