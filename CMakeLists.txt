cmake_minimum_required(VERSION 4.3)
project(myfind)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(myfind
        args.cpp
        args.h
        main.cpp)
