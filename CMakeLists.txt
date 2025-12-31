cmake_minimum_required(VERSION 4.2)

project(CandyCrush LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(CandyCrush src/main.cpp src/Matrix/matrix.cpp src/Casali/casali.cpp)