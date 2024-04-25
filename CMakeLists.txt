cmake_minimum_required(VERSION 3.0.0)
project(communication VERSION 0.1.0 LANGUAGES C CXX)
include_directories(include)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



include(CTest)
enable_testing()

find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

add_library(communication STATIC  src/communication.cpp)
target_link_libraries(communication PUBLIC ${Boost_LIBRARIES})
target_include_directories(communication PUBLIC ${Boost_INCLUDE_DIRS})

add_executable(demo src/demo.cpp )
target_link_libraries(demo communication )


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
