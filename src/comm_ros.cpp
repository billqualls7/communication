/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-05-06 16:58:34
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-05-07 12:22:13
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
#include <tf/transform_datatypes.h>

std::mutex mutex_lock;
std::queue<geometry_msgs::Twist::ConstPtr> velBuf;
ros::Publisher odompub;
Send_VelMessage SendCmdVel;
RobotStatusMessage robot_message;

// std::string IPaddress = "192.168.0.140";
std::string IPaddress = "192.168.51.114";
std::string port = "1234";
boost::asio::io_service io_service;
Communication TCPComm(io_service, IPaddress, port);




void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& Velmsg)
{

    mutex_lock.lock();
    velBuf.push(Velmsg);
    mutex_lock.unlock();


}



void Wheel4RobotComm_send()
{
    while (ros::ok())
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

void Wheel4RobotComm_read()
{   
    ros::Rate rate(50.0);
    nav_msgs::Odometry Odom;
    Odom.header.frame_id = "/odom";
    Odom.child_frame_id = "/base_link";
    while (ros::ok()){

        robot_message = TCPComm.status_analyze();
        // std::cout << std::dec << "x: " << (robot_message.x) << " mm" << std::endl;
        // std::cout << std::dec << "y: " << (robot_message.y) << " mm" << std::endl;
        // std::cout << std::dec << "theta: " << (robot_message.theta) << " rad" << std::endl;
        // std::cout << std::dec << "v_theta: " << (robot_message.v_theta) << " rad/s" << std::endl;

        auto theta = robot_message.theta;
        // 归一化
        if (theta < 0) {
            theta += 360 * (-static_cast<int>(theta) / 360) + (-static_cast<int>(theta) % 360);
        } else {
            theta = fmod(theta, 360);
        }

        // 转换为弧度
        double theta_rad = theta * M_PI / 180.0;
        auto q = tf::createQuaternionFromYaw(theta_rad);
        q.normalize();
        Odom.header.stamp = ros::Time::now();
        Odom.pose.pose.position.x = robot_message.x;
        Odom.pose.pose.position.y = robot_message.y;
        Odom.pose.pose.position.z = 0;
        tf::quaternionTFToMsg(q, Odom.pose.pose.orientation);

        
        Odom.twist.twist.linear.x = robot_message.v_x;
        Odom.twist.twist.linear.y = robot_message.v_y;
        Odom.twist.twist.angular.z = robot_message.v_theta;

        odompub.publish(Odom);
        
        ros::spinOnce();
        rate.sleep();


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
    
    std::thread Wheel4RobotComm_send_process{Wheel4RobotComm_send};
    std::thread OpenTCP_process{OpenTcpConnet};
    std::thread Wheel4RobotComm_read_process{Wheel4RobotComm_read};

    ros::spin();
    return 0;
}