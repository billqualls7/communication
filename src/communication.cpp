/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-24 19:32:55
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-29 08:57:35
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
            
            boost::posix_time::ptime now_read_time = boost::posix_time::microsec_clock::universal_time();
            if (last_read_start_time_ != boost::posix_time::ptime()) {
                boost::posix_time::time_duration diff = now_read_time - last_read_start_time_;
                diff_time = diff.total_milliseconds();
                // std::cout << std::dec << "Read interval: " << static_cast<int>(time) << " ms" << std::endl;
            }
            last_read_start_time_ = now_read_time;


            
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
    last_read_start_time_ = boost::posix_time::microsec_clock::universal_time();
    socket_.async_read_some(boost::asio::buffer(read_buffer_),
            boost::bind(&Communication::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
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



void Communication::status_analyze()
{
    if (read_data_queue_.size() == 1 && first_read_flag){
        mutex_lock.lock();
        first_read_buffer_ptr_  = read_data_queue_.front().data();
        mutex_lock.unlock();
        std::cout<<"first read buffer"<<std::endl;

        float f_x_ = Byte_to_Float((first_read_buffer_ptr_ + X_PTR_));
        float f_y_ = Byte_to_Float((first_read_buffer_ptr_ + Y_PTR_));
        float f_theta_ = Byte_to_Float((first_read_buffer_ptr_ + THETA_PTR_));


        last_x_ = f_x_;
        last_y_ = f_y_;
        last_theta_ = f_theta_;

        first_read_flag = false;

    }



    if (read_data_queue_.size() > 1)
    {   
        mutex_lock.lock();
        read_data_queue_.pop();
        auto now_read_buffer_ptr_  = read_data_queue_.front().data();
        mutex_lock.unlock();
        
        auto x = Byte_to_Float((now_read_buffer_ptr_ + X_PTR_));
        auto y = Byte_to_Float((now_read_buffer_ptr_ + Y_PTR_));
        auto theta = Byte_to_Float((now_read_buffer_ptr_ + THETA_PTR_));

        robot_message.x = x/1000;
        robot_message.y = y/1000;
        robot_message.theta = theta;
        robot_message.v_x = (x-last_x_)/diff_time;
        robot_message.v_y = (y-last_y_)/diff_time;
        robot_message.v_theta = (theta-last_theta_)/1000/diff_time;



        std::cout << std::dec << "x: " << (x) << " mm" << std::endl;
        std::cout << std::dec << "y: " << (y) << " mm" << std::endl;
        std::cout << std::dec << "theta: " << (theta) << " rad" << std::endl;
        std::cout << std::dec << "v_theta: " << (robot_message.v_theta) << " rad/s" << std::endl;

        std::cout << std::dec << "Read interval: " << diff_time << " ms" << std::endl;


        // std::cout << std::hex <<  now_read_buffer_ptr_[3] << std::endl;
        last_x_ = x;
        last_y_ = y;
        last_theta_ = theta;




    }
    
}



float Communication::Byte_to_Float(char *p)
{
	float float_data=0;
	unsigned long longdata = 0;
	longdata = (*p<< 24) + (*(p+1) << 16) + (*(p + 2) << 8) + (*(p + 3) << 0);
	float_data = *(float*)&longdata;
	return float_data;
}



unsigned char* Float_to_Byte(float f)
{
	float float_data = 0;
    unsigned char byte[4];
	unsigned long longdata = 0;
	longdata = *(unsigned long*)&f;           //注意，会丢失精度
	byte[0] = (longdata & 0xFF000000) >> 24;
	byte[1] = (longdata & 0x00FF0000) >> 16;
	byte[2] = (longdata & 0x0000FF00) >> 8;
	byte[3] = (longdata & 0x000000FF);
	return byte;
}



unsigned char Communication::xorChecksum(const unsigned char *data, size_t length) {
    unsigned char checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum ^= data[i];
    }
    return checksum;
}


std::vector<unsigned char> Communication::concatenateKnownLengthVectors(const std::vector<std::vector<unsigned char>>& vecs, size_t totalLength) {
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