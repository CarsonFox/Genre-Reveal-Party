cmake_minimum_required(VERSION 3.0)
project(Genre_Reveal_Party)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

add_subdirectory(csv-parser)
include_directories(csv-parser/include)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

add_executable(serial serial.cpp common.cpp)
target_link_libraries(serial csv)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    add_executable(parallel parallel.cpp common.cpp)
    target_link_libraries(parallel PUBLIC OpenMP::OpenMP_CXX csv)
endif()

find_package(MPI)
if(MPI_FOUND)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS})
    add_executable(distributed distributed.cpp common.cpp)
    target_include_directories(distributed PUBLIC ${MPI_CXX_INCLUDE_DIRS})
    target_link_libraries(distributed ${MPI_CXX_LIBRARIES} csv)
endif()
