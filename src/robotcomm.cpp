/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-25 07:49:39
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-25 08:37:20
 * @FilePath: /communication/src/robotcomm.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "robotcomm.h"



void RobotCOM::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error) {
        try {
            // 从缓冲区中提取数据
            std::string received_data(boost::asio::buffer_cast<const char*>(read_buffer_.data()), bytes_transferred);
            
            // 处理收到的数据
            std::cout << "Received data: " << received_data << std::endl;

            // 清空缓冲区以准备下一次读取
            read_buffer_.consume(bytes_transferred);

            // 继续读取更多数据
            async_read();
        } catch (std::exception& e) {
            std::cerr << "Exception in handle_read: " << e.what() << std::endl;
        }
    } else {
        // 读取失败，处理错误
        std::cerr << "Read error: " << error.message() << std::endl;
    }
}

void RobotCOM::async_read()
{
     boost::asio::async_read(socket_, read_buffer_,
        boost::bind(&RobotCOM::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}