cmake_minimum_required(VERSION 3.20)
project(ex3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(ex3
        main.cpp JobManager.cpp MapReduceFramework.cpp)
