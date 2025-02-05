#
# dependencies
#

# gtest
add_subdirectory(googletest)
add_library(lib::gtest ALIAS gtest)
add_library(lib::gtest_main ALIAS gtest_main)

# fmt
add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_library(lib::fmt ALIAS fmt)

# vulkan bootstrap
add_subdirectory(vk-bootstrap EXCLUDE_FROM_ALL)
add_library(lib::vkb ALIAS vk-bootstrap)

# vulkan memory allocator
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE vulkan-malloc)
add_library(lib::vma ALIAS vma)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)
add_library(lib::glm ALIAS glm)

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)
add_library(lib::stb ALIAS stb)

# fastgltf
add_subdirectory(fastgltf EXCLUDE_FROM_ALL)
add_library(lib::fastgltf ALIAS fastgltf)

# SDL3
add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_library(lib::sdl ALIAS SDL3-shared)

# imgui
find_package(Vulkan REQUIRED)
add_library(imgui STATIC)
target_include_directories(imgui PUBLIC imgui)
target_sources(imgui PRIVATE 
  imgui/imgui.h
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui_tables.cpp
  imgui/backends/imgui_impl_vulkan.cpp
  imgui/backends/imgui_impl_sdl3.cpp
)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan lib::sdl)
add_library(lib::imgui ALIAS imgui)
