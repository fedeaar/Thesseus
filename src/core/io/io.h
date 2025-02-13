#pragma once

#include "../result.hpp"
#include "../status.h"
#include "../types.h"

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

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

core::Status
load(std::filesystem::path path, fastgltf::Asset* asset_ptr);
}

} // namespace io
