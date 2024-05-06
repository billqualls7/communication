cmake_minimum_required(VERSION 3.0.2)
project(comm_ros)
include_directories(include/comm_ros)
include_directories(include)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)

find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Threads REQUIRED)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES comm_ros
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_library(communication STATIC  src/communication.cpp)
target_link_libraries(communication PUBLIC ${Boost_LIBRARIES} pthread)
target_include_directories(communication PUBLIC ${Boost_INCLUDE_DIRS} pthread)

# add_executable(demo src/demo.cpp )
# target_link_libraries(demo communication)


add_executable(comm_ros src/comm_ros.cpp )
target_link_libraries(comm_ros communication ${catkin_LIBRARIES})