# 1. On donne le chemin racine où chercher (include et lib)
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/ncurses")
endif()

# 2. On configure la recherche
set(CURSES_NEED_NCURSES TRUE)

# 3. On lance la recherche (SANS le mot "CONFIG")
find_package(Curses REQUIRED) 

# 4. Création et liaison
add_library(Term STATIC term.cpp)
target_link_libraries(Term PUBLIC ${CURSES_LIBRARIES})
target_include_directories(Term PRIVATE ${CURSES_INCLUDE_DIRS})
