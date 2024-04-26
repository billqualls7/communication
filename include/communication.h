/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-04-24 19:35:46
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-04-26 05:58:32
 * @FilePath: /code/communication/include/communication.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <iostream>
#include <iomanip>
#include <string>
#include <queue>
#include <mutex>
#include <vector>
#include "boost/asio.hpp"
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/streambuf.hpp>
void say_hello();

using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using boost::system::error_code;

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

    float x;           
    float y;
    float theta; 

    uint8_t PositioningStatus;
    uint8_t MotionMode ;

};





class Communication
{

    
private:
    /* data */

    std::vector<unsigned char> query_data = {0x05, 0x0A, 0x05, 0x0A, 0x01, 0x03, 0x09, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    deadline_timer timer_;

    void do_connect(const std::string& host, long port);
    void handle_resolve(const boost::system::error_code& error, 
                        tcp::resolver::iterator endpoint_iterator);
    void handle_connect(const boost::system::error_code& error);
    void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred);
    void query();
    void handle_query(const boost::system::error_code& error, std::size_t bytes_transferred);
    void timer_send();
    void handle_timeout(const boost::system::error_code& error);
public:
    tcp::resolver resolver_;
    tcp::socket socket_;
    // boost::array<char, 64> read_buffer_;
    std::array<char, 64> read_buffer_;
    std::queue<std::array<char, 64>> read_data_queue_;
    // std::vector<char> read_buffer_;
    std::mutex mutex_lock;
    RobotStatusMessage robot_message;
    
    Communication(boost::asio::io_service& io_service, const std::string& host, long port);
    ~Communication();
    void async_read();
    void status_analyze();
    // virtual void async_write() = 0;

    
};















#endif
