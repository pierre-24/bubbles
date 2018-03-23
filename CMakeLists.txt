cmake_minimum_required(VERSION 3.9)
project(bubbles)

set(CMAKE_C_STANDARD 99)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
add_executable(bubbles
        ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES}
        game.c
        game.h
        game_object_definitions.h
        main.c
        Makefile
        images.c
        images.h
        utils.c
        utils.h
        game_object_definitions.h
        game_object_definitions.c
        datafile.c
        datafile.h animations.c animations.h levels.c levels.h)

include_directories(bubbles ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_link_libraries(bubbles ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

