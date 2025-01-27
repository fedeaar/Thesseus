#define STB_IMAGE_IMPLEMENTATION

#include "io.h"
#include <stb_image.h>

u8*
io::image::raw(const std::string& path,
               i32* width,
               i32* height,
               i32* comp,
               i32 req_comp)
{
  // TODO: proper error handling
  stbi_set_flip_vertically_on_load(true);
  return stbi_load(path.c_str(), width, height, comp, req_comp);
}

void
io::image::free(u8* image)
{
  stbi_image_free(image);
}
