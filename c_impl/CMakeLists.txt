cmake_minimum_required(VERSION 3.0)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)
file(GLOB SRC "src/*.c")
add_executable(chip8 ${SRC})
target_link_libraries(chip8 ${SDL2_LIBRARIES})
