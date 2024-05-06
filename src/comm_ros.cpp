/*
 * @Author: wuyao 1955416359@qq.com
 * @Date: 2024-05-06 16:58:34
 * @LastEditors: wuyao 1955416359@qq.com
 * @LastEditTime: 2024-05-06 17:28:41
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
void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg)
{



}



int main(int argc, char** argv)
{
    // 初始化ROS节点
    ros::init(argc, argv, "odom_publisher");
    ros::NodeHandle n;

    // 订阅cmd_vel消息
    ros::Subscriber cmd_vel_sub = n.subscribe("/cmd_vel", 50, cmdVelCallback);

    // 进入循环，等待消息
    ros::spin();

    return 0;
}