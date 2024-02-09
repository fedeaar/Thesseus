#include "../../raw.h"
#include "../reformatter.h"
#include "gtest/gtest.h"

namespace {

TEST(Raw_Reformatter, cast_smaller_len) {
  // input
  struct __attribute__((__packed__)) raw {
    GLfloat x1, y1, z1;
  };
  const u32 length = 2;
  const raw data[length] = {{0.1, 0.1, 0.1}, {0.2, 0.2, 0.2}};
  const std::shared_ptr<void> ptr((void*)&data, [](void*) {});
  const std::vector<Raw::attribute> format = {
      {{gl_float, 3}, GL_FALSE},
  };
  const Raw raw_data(ptr, sizeof(data), format);
  const std::vector<Reformatter::action> actions = {
      {Reformatter::MOVE_CAST, cast_action : {0, {{gl_float, 2}, GL_FALSE}}}};

  // expected
  struct __attribute__((__packed__)) raw_expected {
    GLfloat x1, y1;
  };
  const raw_expected expected_data[length] = {{0.1, 0.1}, {0.2, 0.2}};
  const std::shared_ptr<void> expected_ptr((void*)&expected_data, [](void*) {});
  const std::vector<Raw::attribute> expected_format = {
      {{gl_float, 2}, GL_FALSE}};
  const Raw expected(expected_ptr, sizeof(expected_data), expected_format);

  // test
  Reformatter reformatter(raw_data, actions);
  auto result = reformatter.get();

  EXPECT_EQ(result, expected);
}

}  // namespace