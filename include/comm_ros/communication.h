/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-24 19:35:46
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-05-08 13:36:00
 * @FilePath: /code/communication/include/communication.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string>
#include <queue>
#include <mutex>
#include <vector>
#include <iomanip>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void say_hello();

using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using boost::system::error_code;



#define X_PTR_ 24
#define Y_PTR_ 28
#define THETA_PTR_ 32

struct RobotStatusMessage 
{
    uint8_t respond;
    uint8_t FaultStatus;
    uint8_t StopStatus;
    uint8_t Battery;
    uint8_t ChargingStatus ;
    uint8_t MotionStatus ;
    uint8_t AvoidanceStatus ;
    uint8_t LaserObstacleSwitch ;

    int32_t Wheel_1_Enconder;
    int32_t Wheel_2_Enconder;
    int32_t Wheel_3_Enconder;
    int32_t Wheel_4_Enconder;

    float x = 0;           
    float y = 0;
    float theta = 0; 

    uint8_t PositioningStatus;
    uint8_t MotionMode ;

    float v_x;
    float v_y;
    float v_theta;

};



    // std::vector<char> Package_Sequence_Num = {0x01,0x00}; //需要赋值 小于492字节，序号为1
    // std::vector<char> Finall_Package_Flag = {0x01};
    // 这个组合不能超过492字节

struct Send_VelMessage
{
    float v_x;
    float v_y;
    float v_theta;


    std::vector<unsigned char> Head_Mess = {0x05, 0x0A, 0x05, 0x0A};
    std::vector<unsigned char> Commuincation_flag = {0x01}; // ?
    std::vector<unsigned char> Command = {0x13, 0x0A, 0x00, 0x00};
    std::vector<unsigned char> Len_Mess = {0x0C, 0x00};
    std::vector<unsigned char> Package_Sequence_Num = {0x01,0x00}; 
    std::vector<unsigned char> Finall_Package_Flag = {0x01};// 

    std::vector<unsigned char> Send_Vel_Head = { 0x05, 0x0A, 0x05, 0x0A,
                                        0x01,
                                        0x13, 0x0A, 0x00, 0x00,
                                        0x0C, 0x00,
                                        0x01, 0x00,
                                        0x01}; //专属14bytes

    std::vector<unsigned char> BCC_Check_MessBody = {0x00};   // 待更新
    std::vector<unsigned char> Keep_bytes = {0x00,0x00,0x00,0x00};
    std::vector<unsigned char> BCC_Check_Front19 = {0x00}; // 待更新

    std::vector<unsigned char> v_x_bytes;
    std::vector<unsigned char> v_y_bytes;
    std::vector<unsigned char> v_theta_bytes;

    std::vector<std::vector<unsigned char>> Vel_Mess = {Send_Vel_Head, BCC_Check_MessBody, 
                                               Keep_bytes, BCC_Check_Front19,
                                               v_x_bytes,
                                               v_y_bytes,
                                               v_theta_bytes};
    

    uint8_t totlalength = 32;

    Send_VelMessage() {

        updateVelMess();
    }


    void updateVelMess() {
        Vel_Mess = {Send_Vel_Head, BCC_Check_MessBody, 
                    Keep_bytes, BCC_Check_Front19,
                    v_x_bytes,
                    v_y_bytes,
                    v_theta_bytes};
    }



};



class Communication
{

    
private:
    /* data */

        std::vector<unsigned char> query_data = {   0x05, 0x0A, 0x05, 0x0A, 
                                                    0x01, 
                                                    0x03, 0x09, 0x00, 0x00, 
                                                    0x00, 0x00, 
                                                    0x01, 0x00, 
                                                    0x01, 
                                                    0x00, 
                                                    0x00, 0x00, 0x00, 0x00, 
                                                    0x00};
                                                    
    std::vector<unsigned char> roscontrl_data = {   0x05, 0x0A, 0x05, 0x0A, 
                                                    0x01, 
                                                    0x17, 0X0A, 0x00, 0x00, 
                                                    0x01, 0x00, 
                                                    0x01, 0x00,
                                                    0x01, 
                                                    0x01, 
                                                    0x00, 0x00, 0x00, 0x00, 
                                                    0x1C,
                                                    0X01};
    deadline_timer timer_;
    boost::posix_time::ptime last_read_start_time_;
    int diff_time;
    bool first_read_flag = true;



    float last_x_;
    float last_y_;
    float last_theta_;

    float detal_last_x_;
    float detal_last_y_;
    float detal_last_theta_;

    std::size_t bytes_to_read = 57;

    void do_connect(const std::string& host, const std::string& port);
    void handle_resolve(const boost::system::error_code& error, 
                        tcp::resolver::iterator endpoint_iterator);
    void handle_connect(const boost::system::error_code& error);
    void handle_read(const boost::system::error_code& error, 
                     std::size_t bytes_transferred);
    void query();
    void handle_query(const boost::system::error_code& error, 
                      std::size_t bytes_transferred);
    void timer_send();
    void handle_timeout(const boost::system::error_code& error);
    float Byte_to_Float(const char *p);
    std::vector<unsigned char> Float_to_Byte(const float f);
    void handle_sendcmdvel(const boost::system::error_code& error, std::size_t bytes_transferred);
    void sendcmdvel(const std::vector<unsigned char>& data);
    

public:
    tcp::resolver resolver_;
    tcp::socket socket_;
    // std::array<char, 64> first_read_buffer_;
    std::array<char, 64> last_read_buffer_;
    char* first_read_buffer_ptr_;
    std::array<char, 64> read_buffer_;
    boost::asio::streambuf read_stream_bufbuffer_;

    std::queue<std::array<char, 64>> read_data_queue_;
    std::mutex mutex_lock;
    
    Send_VelMessage send_velstuct;
    
    Communication(boost::asio::io_service& io_service, const std::string& host, const std::string& port);
    ~Communication();
    void async_read();
    void status_analyze(RobotStatusMessage& robot_message);
    unsigned char xorChecksum(const std::vector<unsigned char>& data);
    std::vector<unsigned char> concatenateKnownLengthVectors(const std::vector<std::vector<unsigned char>>& vecs, size_t totalLength);
    void ConsistTCPMessageVel(const Send_VelMessage& sendvelmsg);
    // virtual void async_write() = 0;

    
};















#endif
