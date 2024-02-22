#ifndef CORE_GL_CAST_
#define CORE_GL_CAST_

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

inline void gl_copy_cast(void* from, void* to, u32 len, GL_Type from_type,
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
      _gl_copy_cast<f32>(from, to, len, to_type);
      break;
    case gl_double:
      _gl_copy_cast<f64>(from, to, len, to_type);
      break;
  }
}

template <typename T>
inline void display_cast(void* data, u32 len, std::ostream& stream) {
  u32 i = 0;
  for (; i < len - 1; ++i) {
    stream << ((T*)data)[i] << " ";
  }
  stream << ((T*)data)[i];
}

inline void gl_display(void* data, GL_Type as, u32 len, std::ostream& stream) {
  switch (as) {
    case gl_byte:
      display_cast<i8>(data, len, stream);
      break;
    case gl_unsigned_byte:
      display_cast<u8>(data, len, stream);
      break;
    case gl_short:
      display_cast<i16>(data, len, stream);
      break;
    case gl_unsigned_short:
      display_cast<u16>(data, len, stream);
      break;
    case gl_int:
      display_cast<i32>(data, len, stream);
      break;
    case gl_unsigned_int:
      display_cast<u32>(data, len, stream);
      break;
    case gl_float:
      display_cast<f32>(data, len, stream);
      break;
    case gl_double:
      display_cast<f64>(data, len, stream);
      break;
  }
}

inline u32 gl_sizeof(GL_Type type) {
  switch (type) {
    case gl_byte:
      return sizeof(i8);
    case gl_unsigned_byte:
      return sizeof(u8);
    case gl_short:
      return sizeof(i16);
    case gl_unsigned_short:
      return sizeof(u16);
    case gl_int:
      return sizeof(i32);
    case gl_unsigned_int:
      return sizeof(u32);
    case gl_float:
      return sizeof(f32);
    case gl_double:
      return sizeof(f64);
  }
}

#endif  // CORE_GL_CAST_
