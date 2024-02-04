#include "gl_cast.hpp"

void gl_copy_cast(void* from, void* to, u32 len, GL_Type from_type,
                  GL_Type to_type) {
  switch (from_type) {
    case gl_byte:
      _gl_copy_cast<i8>(from, to, len, to_type);
      break;
    case gl_unsigned_byte:
      _gl_copy_cast<u8>(from, to, len, to_type);
      break;
    case gl_short:
      _gl_copy_cast<i16>(from, to, len, to_type);
      break;
    case gl_unsigned_short:
      _gl_copy_cast<u16>(from, to, len, to_type);
      break;
    case gl_int:
      _gl_copy_cast<i32>(from, to, len, to_type);
      break;
    case gl_unsigned_int:
      _gl_copy_cast<u32>(from, to, len, to_type);
      break;
    case gl_float:
      std::cout << "!" << std::endl;
      _gl_copy_cast<f32>(from, to, len, to_type);
      break;
    case gl_double:
      _gl_copy_cast<f64>(from, to, len, to_type);
      break;
  }
}
