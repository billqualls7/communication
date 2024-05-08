/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-24 19:32:55
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-05-08 13:36:03
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



Communication::Communication(boost::asio::io_service& io_service, const std::string& host, const std::string& port)
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
            // ConsistTCPMessageVel();

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

void Communication::do_connect(const std::string& host, const std::string& port)
{   
    std::cout << "连接中..." << std::endl;
    tcp::resolver::query query(host, port);
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
            boost::asio::streambuf::const_buffers_type buffers = read_stream_bufbuffer_.data();
            std::size_t size = read_stream_bufbuffer_.size(); // 读取的字节数
            // std::cout << "读取的字节数: " << size << std::endl;


            std::copy_n(boost::asio::buffer_cast<const char*>(buffers), size, read_buffer_.begin());


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
                    // std::cout << std::dec<<"电池电量："<< static_cast<int>(subarray_.data()[3]) << std::endl;
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
            read_stream_bufbuffer_.consume(size);
            // 继续读取更多数据
            // status_analyze();
            async_read();
        } catch (std::exception& e) {
            std::cerr << "Exception in handle_read: " << e.what() << std::endl;
        }
    } else {
        // 读取失败，处理错误
        std::cerr << "Read error: " << error.message() << std::endl;
    }
}

// void Communication::async_read()
// {   
//     last_read_start_time_ = boost::posix_time::microsec_clock::universal_time();
//     // socket_.async_read_some(boost::asio::buffer(read_buffer_),
//     socket_.async_read_some(boost::asio::buffer(read_buffer_),
//             boost::bind(&Communication::handle_read, this,
//             boost::asio::placeholders::error,
//             boost::asio::placeholders::bytes_transferred));
// }


void Communication::async_read()
{   
    last_read_start_time_ = boost::posix_time::microsec_clock::universal_time();
 // 使用 async_read 而不是 async_read_some
    boost::asio::async_read(socket_, read_stream_bufbuffer_,
        boost::asio::transfer_at_least(1),
        boost::bind(&Communication::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}


// 异步发送速度查询指令数据
void Communication::query()
{

    
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


void Communication::status_analyze(RobotStatusMessage& robot_message)
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

        if (abs(x)<1){ x = 0;}
        if (abs(y)<1){ y = 0;}
        if (abs(theta)<1){ theta = 0;}

        auto detal_x = (x-last_x_)/1000.0f;
        auto detal_y = (y-last_y_)/1000.0f;
        auto datal_theta = theta-last_theta_;


        // std::cout << std::dec << "y : " << y << " mm" << std::endl;
        robot_message.x +=detal_x;
        robot_message.y += detal_y;
        robot_message.theta += datal_theta;

        robot_message.v_x = (x-last_x_)/diff_time;
        robot_message.v_y = (y-last_y_)/diff_time;
        robot_message.v_theta = (theta-last_theta_)*1000.0f/diff_time;

        robot_message.Battery =  static_cast<int>(now_read_buffer_ptr_[3]);



        // std::cout << std::dec << "x: " << (x) << " mm" << std::endl;
        // std::cout << std::dec << "robot_message.y : " << robot_message.y << " m" << std::endl;
        // std::cout << std::dec << "theta: " << (theta) << " rad" << std::endl;
        // std::cout << std::dec << "v_theta: " << (robot_message.v_theta) << " rad/s" << std::endl;

        // std::cout << std::dec << "Read interval: " << diff_time << " ms" << std::endl;


        // std::cout << std::hex <<  now_read_buffer_ptr_[3] << std::endl;
        last_x_ = x;
        last_y_ = y;
        last_theta_ = theta;

        // return robot_message;


    }
    
}


// 小端模式
float Communication::Byte_to_Float(const char *p)
{

	union {
            uint32_t i;
            float f;
        } float_union;
    float out_float;    

    // uint32_t longdata = (static_cast<uint32_t>(p[3]) << 24) |
    //                     (static_cast<uint32_t>(p[2]) << 16) |
    //                     (static_cast<uint32_t>(p[1]) << 8) |
    //                         static_cast<uint32_t>(p[0]);

    // float_union.i = longdata; // 将整数赋值给union的整数成员
    std::memcpy(&float_union.i, p, sizeof(float_union.i));
    
    // out_float = float_union.f;
    return float_union.f; // 然后获取浮点数
}



std::vector<unsigned char> Communication::Float_to_Byte(const float f) {
 // 使用union来避免违反严格别名规则
    union {
        float floatValue;
        unsigned char bytes[4];
    } floatUnion;

    // 将float值赋给union的float部分
    floatUnion.floatValue = f;

    // 创建一个vector来存储字节
    std::vector<unsigned char> bytes;

    // 将union的bytes数组内容复制到vector中
    for (int i = 0; i < 4; ++i) {
        // bytes.push_back(floatUnion.bytes[3-i]);
        bytes.push_back(floatUnion.bytes[i]); // 小端模式
    }

    return bytes;
}


unsigned char Communication::xorChecksum(const std::vector<unsigned char>& data) {
    unsigned char checksum = 0;
    for (size_t i = 0; i < data.size(); ++i) {
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



void Communication::ConsistTCPMessageVel(const Send_VelMessage& sendvelmsg){
    send_velstuct.v_x = 0.5;
    send_velstuct.v_y = 0.1;
    send_velstuct.v_theta = -0.5;

    send_velstuct.v_x_bytes = Float_to_Byte(sendvelmsg.v_x);
    send_velstuct.v_y_bytes = Float_to_Byte(sendvelmsg.v_y);
    send_velstuct.v_theta_bytes = Float_to_Byte(sendvelmsg.v_theta);

    std::vector<std::vector<unsigned char>> vel_body = {send_velstuct.v_x_bytes,
                                                        send_velstuct.v_y_bytes,
                                                        send_velstuct.v_theta_bytes};
    std::vector<unsigned char> vel_body_vector = concatenateKnownLengthVectors(vel_body,12);
    unsigned char check_velbody = xorChecksum(vel_body_vector);
    send_velstuct.BCC_Check_MessBody[0] = check_velbody;

    std::vector<std::vector<unsigned char>> frontmess19 = {send_velstuct.Send_Vel_Head, 
                                                           send_velstuct.BCC_Check_MessBody, 
                                                           send_velstuct.Keep_bytes};
    std::vector<unsigned char> frontmess19_vector = concatenateKnownLengthVectors(frontmess19,19);
    unsigned char check_frontmess19 = xorChecksum(frontmess19_vector);
    send_velstuct.BCC_Check_Front19[0] = check_frontmess19;

    std::vector<std::vector<unsigned char>> full_velmess = {
                                                            frontmess19_vector,
                                                            send_velstuct.BCC_Check_Front19,
                                                            vel_body_vector
                                                            };

    std::vector<unsigned char> full_velmess_vector = concatenateKnownLengthVectors(full_velmess,32);
    for (unsigned char byte : full_velmess_vector) {
            std::cout << std::hex << static_cast<int>(byte) << " ";
         }
    std::cout << std::endl;

    

    sendcmdvel(full_velmess_vector);

    // std::cout << std::dec << full_velmess_vector.size() << std::endl;





}






// 异步发送速度指令
void Communication::sendcmdvel(const std::vector<unsigned char>& data)
{

    
    boost::asio::async_write(socket_, boost::asio::buffer(data),
        boost::bind(&Communication::handle_sendcmdvel, this, 
        boost::asio::placeholders::error, 
        boost::asio::placeholders::bytes_transferred));



}


void Communication::handle_sendcmdvel(const boost::system::error_code& error, std::size_t bytes_transferred){
    if(error){

        std::cerr << "Failed to query robot status: " << error.message() << std::endl;
    }

}