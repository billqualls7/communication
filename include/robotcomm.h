/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-25 07:49:52
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-25 09:04:33
 * @FilePath: /communication/include/robotcomm.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef ROBOTCOMM_H
#define ROBOTCOMM_H

#include "communication.h"
#include <boost/asio/streambuf.hpp>

class RobotCOM: public Communication{
    public:
        RobotCOM(boost::asio::io_service& io_service, const std::string& host, short port): 
                 Communication(io_service, host, port), socket_(io_service)
                 {}
        void async_read() override;


    private:
        boost::asio::streambuf read_buffer_;
        boost::asio::streambuf write_buffer_;
        boost::asio::ip::tcp::socket socket_;
        void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred);
        
};


#endif
