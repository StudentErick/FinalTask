cmake_minimum_required(VERSION 3.10)
project(FinalTask)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(FinalTask main.cpp user_data.h TcpSocket.cpp TcpSocket.h ThreadPool.h ThreadSafeQueue.h TcpServer.cpp TcpServer.h)