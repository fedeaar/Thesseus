cmake_minimum_required(VERSION 3.13)

project(Thesseus)

file(GLOB_RECURSE sources_main src/*.cpp src/*.h)

include_directories("./lib/stb" "./lib/glm")

add_executable(out ${sources_main})
target_compile_options(out PUBLIC -std=c++1y -Wall -Wfloat-conversion -w)
target_link_libraries(out PUBLIC SDL2 GL GLU GLEW)
