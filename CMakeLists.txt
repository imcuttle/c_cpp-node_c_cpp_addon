cmake_minimum_required(VERSION 3.6)
project(linux-c)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES tcp-ip-review/arp.cpp)
add_executable(app ${SOURCE_FILES})

find_package (Threads)
target_link_libraries (app ${CMAKE_THREAD_LIBS_INIT})
