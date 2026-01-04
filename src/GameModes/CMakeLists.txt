add_library(GameModes SHARED gamemodes.cpp)

add_subdirectory(Matrix)

target_link_libraries(GameModes PRIVATE Matrix)