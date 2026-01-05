cmake_minimum_required(VERSION 3.10)

project(CandyCrush LANGUAGES CXX)

option(USE_LIBTERM "Indique si le jeu utilise la backend Term" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_executable(CandyCrush src/main.cpp)

add_subdirectory(src/GameModes)
add_subdirectory(src/Term)

target_link_libraries(CandyCrush PRIVATE GameModes Matrix)

if(USE_LIBTERM)
  add_definitions(-DUSE_LIBTERM)
  target_link_libraries(CandyCrush PRIVATE Term)
endif()
