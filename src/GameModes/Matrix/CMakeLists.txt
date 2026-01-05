add_library(Matrix SHARED matrix.cpp)

add_subdirectory(Casali)

target_link_libraries(Matrix PUBLIC Casali)
