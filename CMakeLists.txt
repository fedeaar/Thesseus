cmake_minimum_required(VERSION 3.5)
project(Thesseus 
  VERSION 0.1
  DESCRIPTION "A procedurally generated dungeon crawler that continually learns how to kill you"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# config
#

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# dependencies
#

# extern - vulkan
find_package(Vulkan REQUIRED)
add_library(lib::vulkan ALIAS Vulkan::Vulkan)

# local
add_subdirectory(lib)
include(CompileShaders)

#
# build
#

# clear
# TODO@cmake: this does not seem to be working
file(REMOVE_RECURSE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY res DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# find sources 
# TODO@cmake use subdirectories
file(GLOB_RECURSE SOURCES_MAIN src/*.cpp src/*.h)

# flags
set(FLAGS -Wconversion -Wnarrowing -Wall -Wfloat-conversion -w)
set(LINK 
  lib::fmt 
  lib::vulkan 
  lib::vkb 
  lib::vma 
  lib::sdl 
  lib::imgui 
  lib::stb
  lib::fastgltf 
  lib::glm)

# build shaders
compile_shaders()

# build 
if (${CMAKE_BUILD_TYPE} STREQUAL Release)
  list(FILTER SOURCES_MAIN EXCLUDE REGEX ".*\\.test\\.cpp")
  add_executable(build ${SOURCES_MAIN})
  target_compile_options(build PUBLIC ${FLAGS})
  target_link_libraries(build PUBLIC ${LINK})

elseif (${CMAKE_BUILD_TYPE} STREQUAL Debug)
  list(FILTER SOURCES_MAIN EXCLUDE REGEX ".*\\.test\\.cpp")
  add_executable(debug ${SOURCES_MAIN})
  target_compile_options(debug PUBLIC ${FLAGS})
  target_link_libraries(debug PUBLIC -lm ${LINK})

elseif (${CMAKE_BUILD_TYPE} STREQUAL Test)
  list(FILTER SOURCES_MAIN EXCLUDE REGEX "main\\.cpp")
  add_executable(tests ${SOURCES_MAIN})
  target_compile_options(tests PUBLIC ${FLAGS})
  target_link_libraries(tests PUBLIC ${LINK} lib::gtest lib::gtest_main)
  
endif()
