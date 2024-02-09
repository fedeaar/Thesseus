#include "../../raw.h"
#include "../reformatter.h"
#include "gtest/gtest.h"

namespace {

TEST(Raw_Reformatter, shuffle) {
  // input
  struct __attribute__((__packed__)) raw {
    GLfloat x1, y1, z1;
    GLfloat x2, y2;
    GLfloat x3;
    GLfloat x4, y4, z4, w5;
  };
  const u32 length = 2;
  const raw data[length] = {
      {0.1, 0.1, 0.1, 0.2, 0.2, 0.3, 0.4, 0.4, 0.4, 0.4},
      {0.5, 0.5, 0.5, 0.6, 0.6, 0.7, 0.8, 0.8, 0.8, 0.8},
  };
  const std::shared_ptr<void> ptr((void*)&data, [](void*) {});
  const std::vector<Raw::attribute> format = {{{gl_float, 3}, GL_FALSE},
                                              {{gl_float, 2}, GL_FALSE},
                                              {{gl_float, 1}, GL_FALSE},
                                              {{gl_float, 4}, GL_FALSE}};
  const Raw raw_data(ptr, sizeof(data), format);
  const std::vector<Reformatter::action> actions = {
      {Reformatter::MOVE, move_action : {2}},  // place format[2] at position 0
      {Reformatter::MOVE, move_action : {1}},
      {Reformatter::MOVE, move_action : {0}},
      {Reformatter::MOVE, move_action : {3}}};

  // expected
  const raw expected_data[length] = {
      {0.3, 0.2, 0.2, 0.1, 0.1, 0.1, 0.4, 0.4, 0.4, 0.4},
      {0.7, 0.6, 0.6, 0.5, 0.5, 0.5, 0.8, 0.8, 0.8, 0.8},
  };
  const std::shared_ptr<void> expected_ptr((void*)&expected_data, [](void*) {});
  const std::vector<Raw::attribute> expected_format = {
      {{gl_float, 1}, GL_FALSE},
      {{gl_float, 2}, GL_FALSE},
      {{gl_float, 3}, GL_FALSE},
      {{gl_float, 4}, GL_FALSE}};
  const Raw expected(expected_ptr, sizeof(expected_data), expected_format);

  // test
  Reformatter reformatter(raw_data, actions);
  auto result = reformatter.get();
  EXPECT_EQ(result, expected);
}

}  // namespace