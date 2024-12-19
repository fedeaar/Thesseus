cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Thesseus)

# deps
find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)
include_directories("./lib/stb" "./lib/glm" "./lib/glew-cmake/include" "./lib/vk-bootstrap/src" "./lib/vulkan-malloc/include")
add_subdirectory(./lib/SDL/)
add_subdirectory(./lib/glew-cmake/)
add_subdirectory(./lib/vk-bootstrap/)
add_subdirectory(./lib/vulkan-malloc/)

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
set(SCENE "two_cubes.scene.cpp" CACHE STRING "what scene to render from scene/collection")
set(FLAGS -std=c++1y -Wconversion -Wnarrowing -Wall -Wfloat-conversion -w)
set(LINK Vulkan::Vulkan GL GLU SDL3::SDL3 libglew_static vk-bootstrap vma)

# build
file(GLOB_RECURSE sources_main src/*.cpp src/*.h)

if (${BUILD} STREQUAL "build")
  list(FILTER sources_main EXCLUDE REGEX ".*\\.(test|scene)\\.cpp")
  add_executable(build ${sources_main} src/scene/collection/${SCENE})
  target_compile_options(build PUBLIC ${FLAGS})
  target_link_libraries(build PUBLIC ${LINK})

elseif (${BUILD} STREQUAL "debug")
  list(FILTER sources_main EXCLUDE REGEX ".*\\.(test|scene)\\.cpp")
  add_executable(debug ${sources_main} src/scene/collection/${SCENE})
  target_compile_options(debug PUBLIC ${FLAGS} --debug)
  target_link_libraries(debug PUBLIC ${LINK})

elseif (${BUILD} STREQUAL "tests")
  list(FILTER sources_main EXCLUDE REGEX ".*\\.(scene)\\.cpp")
  add_subdirectory("./lib/googletest")
  include_directories("./lib/googletest/googletest/include")
  list(FILTER sources_main EXCLUDE REGEX "main\\.cpp")
  add_executable(tests ${sources_main} src/scene/collection/${SCENE})
  target_compile_options(tests PUBLIC ${FLAGS})
  target_link_libraries(tests PUBLIC ${LINK} gtest gtest_main)
  
endif()
