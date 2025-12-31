cmake_minimum_required(VERSION 3.10)

project(CandyCrush LANGUAGES CXX)

find_package(Curses REQUIRED)

add_executable(candycrush_term
  src/term/main.cc
  src/term/term.cc)

target_include_directories(candycrush_term PRIVATE ${CURSES_INCLUDE_DIR})
target_include_directories(candycrush_term PUBLIC inc/term)

target_link_libraries(candycrush_term ${CURSES_LIBRARIES})

add_executable(CandyCrush src/main.cpp)

