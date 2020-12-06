cmake_minimum_required(VERSION 3.0)
project(Genre_Reveal_Party)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

include_directories(lib)

add_executable(serial serial.cpp common.cpp)