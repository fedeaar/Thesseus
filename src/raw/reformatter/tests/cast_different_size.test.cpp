#include "../../raw.h"
#include "../reformatter.h"
#include "gtest/gtest.h"

namespace {

TEST(Raw_Reformatter, cast_different_size) {
  // input
  struct __attribute__((__packed__)) raw {
    GLint x1, y1, z1;
    GLint x2, y2, z2;
  };
  const u32 length = 2;
  const raw data[length] = {
      {1, 1, 1, 257, 259, 76680},
      {877, 3112, 13, 4, 41, 4},
  };
  const std::shared_ptr<void> ptr((void*)&data, [](void*) {});
  const std::vector<Raw::attribute> format = {{{gl_int, 3}, GL_FALSE},
                                              {{gl_int, 3}, GL_FALSE}};
  const Raw raw_data(ptr, sizeof(data), format);
  const std::vector<Reformatter::action> actions = {
      {Reformatter::MOVE_CAST, cast_action : {0, {{gl_short, 3}, GL_FALSE}}},
      {Reformatter::MOVE_CAST, cast_action : {1, {{gl_short, 3}, GL_FALSE}}}};

  // expected
  struct __attribute__((__packed__)) raw_expected {
    GLshort x1, y1, z1;
    GLshort x2, y2, z2;
  };
  const raw_expected expected_data[length] = {
      {(i16)1, (i16)1, (i16)1, (i16)257, (i16)259, (i16)76680},
      {(i16)877, (i16)3112, (i16)13, (i16)4, (i16)41, (i16)4},
  };
  const std::shared_ptr<void> expected_ptr((void*)&expected_data, [](void*) {});
  const std::vector<Raw::attribute> expected_format = {
      {{gl_short, 3}, GL_FALSE}, {{gl_short, 3}, GL_FALSE}};
  const Raw expected(expected_ptr, sizeof(expected_data), expected_format);

  // test
  Reformatter reformatter(raw_data, actions);
  auto result = reformatter.get();

  EXPECT_EQ(result, expected);
}

}  // namespace