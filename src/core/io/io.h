#pragma once

#include "../enum.h"
#include "../logger.hpp"
#include "../result.hpp"
#include "../types.h"

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <vulkan/vulkan.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace core {
namespace io {

namespace text {

std::string
read(std::string const& path);

} // namespace text

namespace image {

u8*
raw(std::string const& path, i32* width, i32* height, i32* comp, i32 req_comp);

void
free(u8* image);

} // namespace image

namespace gltf {

core::code
load(std::filesystem::path path, fastgltf::Asset* asset_ptr);

VkFilter
to_vulkan_filter(fastgltf::Filter filter);
VkSamplerMipmapMode
to_vulkan_mipmap_mode(fastgltf::Filter filter);
}

} // namespace io
} // namespace core
