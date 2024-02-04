#ifndef COPY_CAST_
#define COPY_CAST_

#include <iostream>
#include <typeinfo>

#include "types.h"

template <typename S, typename T>
inline void copy_cast(void* from, void* to, u32 len) {
  for (u32 i = 0; i < len; ++i) {
    ((T*)to)[i] = (T)(((S*)from)[i]);
  }
}

template <typename T>
inline void _gl_copy_cast(void* from, void* to, u32 len, GL_Type to_type) {
  switch (to_type) {
    case gl_byte:
      copy_cast<T, i8>(from, to, len);
      break;
    case gl_unsigned_byte:
      copy_cast<T, u8>(from, to, len);
      break;
    case gl_short:
      copy_cast<T, i16>(from, to, len);
      break;
    case gl_unsigned_short:
      copy_cast<T, u16>(from, to, len);
      break;
    case gl_int:
      copy_cast<T, i32>(from, to, len);
      break;
    case gl_unsigned_int:
      copy_cast<T, u32>(from, to, len);
      break;
    case gl_float:
      copy_cast<T, f32>(from, to, len);
      break;
    case gl_double:
      copy_cast<T, f64>(from, to, len);
      break;
  }
}

void gl_copy_cast(void* from, void* to, u32 len, GL_Type from_type,
                  GL_Type to_type);

#endif  // COPY_CAST_
