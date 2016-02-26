cmake_minimum_required(VERSION 2.8)

add_executable(final final.cpp)

find_package(Threads)
TARGET_LINK_LIBRARIES(final ${CMAKE_THREAD_LIBS_INIT})
