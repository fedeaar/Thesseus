#include "../../raw.h"
#include "../reformatter.h"
#include "gtest/gtest.h"

namespace {

TEST(Raw_Reformatter, cast_equal_size) {
  // input
  struct __attribute__((__packed__)) raw {
    GLfloat x1, y1, z1;
    GLfloat x2, y2, z2;
  };
  const u32 length = 2;
  const raw data[length] = {
      {0.1, 0.1, 0.1, 1.2, 1.2, 1.2},
      {0.1, 0.1, 0.1, 2.2, 2.2, 2.2},
  };
  const std::shared_ptr<void> ptr((void*)&data, [](void*) {});
  const std::vector<Raw::attribute> format = {{{gl_float, 3}, GL_FALSE},
                                              {{gl_float, 3}, GL_FALSE}};
  const Raw raw_data(ptr, sizeof(data), format);
  const std::vector<Reformatter::action> actions = {
      {Reformatter::MOVE_CAST, cast_action : {0, {{gl_int, 3}, GL_FALSE}}},
      {Reformatter::MOVE_CAST, cast_action : {1, {{gl_int, 3}, GL_FALSE}}}};

  // expected
  struct __attribute__((__packed__)) raw_expected {
    GLint x1, y1, z1;
    GLint x2, y2, z2;
  };
  const raw_expected expected_data[length] = {
      {0, 0, 0, 1, 1, 1},
      {0, 0, 0, 2, 2, 2},
  };
  const std::shared_ptr<void> expected_ptr((void*)&expected_data, [](void*) {});
  const std::vector<Raw::attribute> expected_format = {{{gl_int, 3}, GL_FALSE},
                                                       {{gl_int, 3}, GL_FALSE}};
  const Raw expected(expected_ptr, sizeof(expected_data), expected_format);

  // test
  Reformatter reformatter(raw_data, actions);
  auto result = reformatter.get();

  EXPECT_EQ(result, expected);
}

}  // namespace