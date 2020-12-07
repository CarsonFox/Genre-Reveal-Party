cmake_minimum_required(VERSION 3.0)
project(Genre_Reveal_Party)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(csv-parser)
include_directories(csv-parser/include)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Og -g")

add_executable(serial serial.cpp common.cpp)
target_link_libraries(serial csv gcov)