/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-05-06 16:58:34
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-05-06 20:11:35
 * @FilePath: /communication/src/comm_ros.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <cmath>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <chrono>
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include "communication.h"


std::mutex mutex_lock;
std::queue<geometry_msgs::Twist::ConstPtr> velBuf;
ros::Publisher odompub;
Send_VelMessage SendCmdVel;

std::string IPaddress = "192.168.0.140";
std::string port = "1234";
boost::asio::io_service io_service;
Communication TCPComm(io_service, IPaddress, port);




void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& Velmsg)
{

    mutex_lock.lock();
    velBuf.push(Velmsg);
    mutex_lock.unlock();


}



void Wheel4RobotComm()
{
    while (1)
    {
        if (! velBuf.empty()){
            mutex_lock.lock();
            geometry_msgs::Twist::ConstPtr vel = velBuf.front();

            SendCmdVel.v_x = vel->linear.x;
            SendCmdVel.v_y = vel->linear.y;
            SendCmdVel.v_theta = vel->angular.z;

            velBuf.pop();
            mutex_lock.unlock();

            ROS_INFO("SendCmdVel.v_x: %.3f", SendCmdVel.v_x);
            ROS_INFO("SendCmdVel.v_y: %.3f", SendCmdVel.v_y);
            ROS_INFO("SendCmdVel.v_theta: %.3f", SendCmdVel.v_theta);

            TCPComm.ConsistTCPMessageVel(SendCmdVel);

        }
    }
    

}



void OpenTcpConnet()
{
    
    io_service.run();

}


int main(int argc, char** argv)
{
    // 初始化ROS节点
    ros::init(argc, argv, "robot_communication");
    ros::NodeHandle n;


    ROS_INFO("IPaddress: %s", IPaddress.c_str());
    ROS_INFO("port: %s", port.c_str());
    n.getParam("/IPaddress", IPaddress);
    n.getParam("/port", port);
    
    // 订阅cmd_vel消息
    ros::Subscriber cmd_vel_sub = n.subscribe("/cmd_vel", 100, cmdVelCallback);
    odompub = n.advertise<nav_msgs::Odometry>("/odom",100);
    
    std::thread Wheel4robot_process{Wheel4RobotComm};
    std::thread OpenTCP_process{OpenTcpConnet};

    ros::spin();
    return 0;
}