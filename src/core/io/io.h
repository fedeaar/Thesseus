#ifndef CORE_IO_
#define CORE_IO_

#include <fstream>
#include <iostream>
#include <string>

#include "../types.h"

namespace io {

namespace text {

std::string read(const std::string& path);

}  // namespace text

namespace image {

u8* raw(const std::string& path, i32* width, i32* height, i32* comp,
        i32 req_comp);
void free(u8* image);

}  // namespace image

}  // namespace io

#endif  // CORE_IO_
