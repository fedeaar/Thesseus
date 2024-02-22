#ifndef CORE_TYPES_
#define CORE_TYPES_

#include <GL/glew.h>

#include <glm/glm.hpp>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef glm::vec<2, int> v2i;
typedef glm::vec3 v3f;
typedef glm::vec4 v4f;
typedef glm::mat4 m4f;

enum GL_Type {
  gl_byte = GL_BYTE,
  gl_unsigned_byte = GL_UNSIGNED_BYTE,
  gl_short = GL_SHORT,
  gl_unsigned_short = GL_UNSIGNED_SHORT,
  gl_int = GL_INT,
  gl_unsigned_int = GL_UNSIGNED_INT,
  gl_float = GL_FLOAT,
  gl_double = GL_DOUBLE,
};

#endif  // CORE_TYPES_
