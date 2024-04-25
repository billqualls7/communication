/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-24 19:32:55
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-25 09:22:38
 * @FilePath: /communication/src/communication.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "communication.h"
using boost::asio::ip::tcp;

void say_hello(){
    std::cout << "Hello, from communication!\n";
}



Communication::Communication(boost::asio::io_service& io_service, const std::string& host, short port)
                            : resolver_(io_service), socket_(io_service)
{
    std::cout<<"IP:"<<host<<" port:"<<port<<std::endl;
    do_connect(host, port);
    // async_read();

}

Communication::~Communication()
{
}



void Communication::handle_connect(const boost::system::error_code& error)
{
    if (!error) {
            std::cout << "连接成功！！！" << std::endl;// 连接成功
            async_read();

        }
    else {
        std::cerr << "连接失败，错误码：" << error << "，错误消息：" 
                  << error.message() << std::endl;
        // 可以在这里添加重试逻辑或其他错误恢复策略
    }
}

void Communication::handle_resolve(const boost::system::error_code& error, 
                        tcp::resolver::iterator endpoint_iterator)
{

    if (!error) {
            boost::asio::async_connect(socket_, endpoint_iterator,
                boost::bind(&Communication::handle_connect, this,
                            boost::asio::placeholders::error));
        }
    else {
        std::cerr << "解析失败，错误码：" << error 
                  << "，错误消息：" << error.message() << std::endl;
     }


}

void Communication::do_connect(const std::string& host, short port)
{   
    std::cout << "连接中..." << std::endl;
    tcp::resolver::query query(host, std::to_string(port));
    resolver_.async_resolve(query,
                        boost::bind(&Communication::handle_resolve, this,
                        boost::asio::placeholders::error, 
                        boost::asio::placeholders::iterator));



}




void Communication::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error) {
        try {
            // 从缓冲区中提取数据
            std::cout << "Received data: " << read_buffer_.data() << std::endl;            
            // 处理收到的数据

            // 清空缓冲区以准备下一次读取

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

void Communication::async_read()
{
    socket_.async_read_some(boost::asio::buffer(read_buffer_),
            boost::bind(&Communication::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

// void Communication::async_read(){}
// void Communication::async_write(){}
