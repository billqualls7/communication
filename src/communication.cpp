/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-24 19:32:55
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-26 06:00:31
 * @FilePath: /communication/src/communication.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "communication.h"
using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using boost::system::error_code;
void say_hello(){
    std::cout << "Hello, from communication!\n";
}



Communication::Communication(boost::asio::io_service& io_service, const std::string& host, long port)
                            : resolver_(io_service), socket_(io_service), timer_(io_service)
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
            timer_send();

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

void Communication::do_connect(const std::string& host, long port)
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
            // std::cout << "Received data: " << read_buffer_.data() << std::endl;
            
            
            char* data_ptr = read_buffer_.data();
            if (data_ptr[0] == 0x05){
                if (data_ptr[5] == 0x03 && data_ptr[6] == 0x0D) //机器人状态
                {
                    std::array<char, 64> subarray_;
                    std::copy(read_buffer_.begin() + 20, read_buffer_.end(), subarray_.begin());
                    // std::cout << subarray_.size() << std::endl;
                    // std::cout <<"subarray_.data()[3]"<< subarray_.data()[3] << std::endl;
                    // std::cout <<"data_ptr"<< static_cast<int>(data_ptr[23]) << std::endl;
                    // std::cout <<"data_ptr"<< std::showbase << std::hex << static_cast<int>(data_ptr[23]) << std::endl;
                    mutex_lock.lock();
                    read_data_queue_.push(subarray_);
                    mutex_lock.unlock();

                } 
                // auto hexbuffer = static_cast<int>(data_ptr[0]);
                // std::cout << std::showbase << std::hex << hexbuffer << std::endl;
                // std::cout << read_data_queue_.size() << std::endl;


            }

            // std::cout << "Received data: " << static_cast<int>(read_buffer_.data()[0])<< std::endl;            
            
            // std::cout << std::showbase << std::hex << (data_ptr[0] + data_ptr[1] + data_ptr[2] +data_ptr[3]) << std::endl;
            // 处理收到的数据


            // std::cout << "bytes_transferred: " << bytes_transferred << std::endl;

            // 清空缓冲区以准备下一次读取

            // 继续读取更多数据
            status_analyze();
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




void Communication::status_analyze()
{
    if (read_data_queue_.size() > 1)
    {   
        mutex_lock.lock();

        robot_message.Battery = (read_data_queue_.front().data()[3]);
        read_data_queue_.pop();
        mutex_lock.unlock();

        // std::cout << static_cast<uint8_t>(robot_message.Battery) << std::endl;
        std::cout << std::showbase << std::hex << static_cast<int>(robot_message.Battery) << std::endl;



    }
    
}

// void Communication::async_read(){}
// void Communication::async_write(){}



void Communication::query()
{

    // 异步发送数据
    boost::asio::async_write(socket_, boost::asio::buffer(query_data),
        boost::bind(&Communication::handle_query, this, 
        boost::asio::placeholders::error, 
        boost::asio::placeholders::bytes_transferred));



}


void Communication::handle_query(const boost::system::error_code& error, std::size_t bytes_transferred){
    if(error){

        std::cerr << "Failed to query robot status: " << error.message() << std::endl;
    }

}



void Communication::timer_send()
{
    timer_.expires_from_now(boost::posix_time::milliseconds(50));
    timer_.async_wait(boost::bind(&Communication::handle_timeout, this, 
                      boost::asio::placeholders::error));

}


void Communication::handle_timeout(const boost::system::error_code& error)
{
    
    if (!error){
        query();

        timer_send();
    }
    else {
        // 如果有错误，打印错误信息
        std::cerr << "Timer error: " << error.message() << std::endl;
    }
    

}