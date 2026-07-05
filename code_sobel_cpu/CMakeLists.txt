cmake_minimum_required(VERSION 3.16)

project(OpenCV_Test)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(OpenCV_Test
    main.cpp
)

target_link_libraries(OpenCV_Test PRIVATE ${OpenCV_LIBS})