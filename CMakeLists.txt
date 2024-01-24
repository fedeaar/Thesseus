cmake_minimum_required(VERSION 3.13)

project(Thesseus)

set(build_dir "bin")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${build_dir}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${build_dir}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${build_dir})

include_directories("./lib/stb" "./lib/glm")

file(GLOB_RECURSE resources res/*)
file(REMOVE_RECURSE ${build_dir})
file(COPY ${resources} DESTINATION ${build_dir}/res)

file(GLOB_RECURSE sources_main src/*.cpp src/*.h)

add_executable(out ${sources_main})
target_compile_options(out PUBLIC -std=c++1y -Wall -Wfloat-conversion -w)
target_link_libraries(out PUBLIC SDL2 GL GLU GLEW)
