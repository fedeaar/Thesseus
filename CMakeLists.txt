cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Thesseus)

# deps
include_directories("./lib/stb" "./lib/glm")

# bin
set(build_dir "bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${build_dir}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${build_dir}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${build_dir})

file(REMOVE_RECURSE ${build_dir})
file(COPY res DESTINATION ${build_dir})
file(COPY src/shader DESTINATION ${build_dir} FILES_MATCHING REGEX "^.+(\.vs|\.fs)$")

# options
set(BUILD "build" CACHE STRING "type of output: debug | build | test")

# build
file(GLOB_RECURSE sources_main src/*.cpp src/*.h)

if (${BUILD} STREQUAL "build")
  list(FILTER sources_main EXCLUDE REGEX ".*\\.test\\.cpp")
  add_executable(build ${sources_main})
  target_compile_options(build PUBLIC -std=c++1y -Wall -Wfloat-conversion -w)
  target_link_libraries(build PUBLIC SDL2 GL GLU GLEW)

elseif (${BUILD} STREQUAL "debug")
  list(FILTER sources_main EXCLUDE REGEX ".*\\.test\\.cpp")
  add_executable(debug ${sources_main})
  target_compile_options(debug PUBLIC -std=c++1y -Wall -Wfloat-conversion -w --debug)
  target_link_libraries(debug PUBLIC SDL2 GL GLU GLEW)
  
elseif (${BUILD} STREQUAL "tests")
  add_subdirectory("./lib/googletest")
  include_directories("./lib/googletest/googletest/include")
  list(FILTER sources_main EXCLUDE REGEX "main\\.cpp")
  add_executable(tests ${sources_main})
  target_compile_options(tests PUBLIC -std=c++1y -Wall -Wfloat-conversion -w)
  target_link_libraries(tests PUBLIC SDL2 GL GLU GLEW gtest gtest_main)

endif()
