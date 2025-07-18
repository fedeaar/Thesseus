#define STB_IMAGE_IMPLEMENTATION

#include "io.h"
#include <stb_image.h>

u8*
core::io::image::raw(std::string const& path,
                     i32* width,
                     i32* height,
                     i32* comp,
                     i32 req_comp)
{
  // TODO: proper error handling
  stbi_set_flip_vertically_on_load(true);
  return stbi_load(path.c_str(), width, height, comp, req_comp);
}

u8*
core::io::image::raw(u8* data,
                     u32 length,
                     i32* width,
                     i32* height,
                     i32* comp,
                     i32 req_comp)
{
  // TODO: proper error handling
  stbi_set_flip_vertically_on_load(true);
  return stbi_load_from_memory(data, length, width, height, comp, req_comp);
}

void
core::io::image::free(u8* image)
{
  stbi_image_free(image);
}
