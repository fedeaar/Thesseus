# TODO@cmake: don't depend o custom target?
function(compile_shaders)
  find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)
  file(GLOB_RECURSE GLSL_SOURCE_FILES
      ${PROJECT_SOURCE_DIR}/res/shaders/*.frag
      ${PROJECT_SOURCE_DIR}/res/shaders/*.vert
      ${PROJECT_SOURCE_DIR}/res/shaders/*.comp)
  foreach(GLSL ${GLSL_SOURCE_FILES})
    message(STATUS "BUILDING SHADER ${GLSL}")
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${FILE_NAME}.spv)
    add_custom_command(
      OUTPUT ${SPIRV}
      COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
      DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
  endforeach(GLSL)
  add_custom_target(build-time-make-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders)
  add_custom_target(
    shaders
    ALL
    DEPENDS ${SPIRV_BINARY_FILES})
endfunction()
