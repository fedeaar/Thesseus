#include "reformatter.h"

#include "gtest/gtest.h"
#include "raw.h"

namespace {

TEST(Model_Reformatter, shuffle) {
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

TEST(Model_Reformatter, remove) {
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
      {Reformatter::MOVE, move_action : {2}},
      {Reformatter::MOVE, move_action : {3}}};

  // expected
  struct __attribute__((__packed__)) raw_expected {
    GLfloat x1;
    GLfloat x2, y2, z2, w2;
  };
  const raw_expected expected_data[length] = {
      {0.3, 0.4, 0.4, 0.4, 0.4},
      {0.7, 0.8, 0.8, 0.8, 0.8},
  };
  const std::shared_ptr<void> expected_ptr((void*)&expected_data, [](void*) {});
  const std::vector<Raw::attribute> expected_format = {
      {{gl_float, 1}, GL_FALSE}, {{gl_float, 4}, GL_FALSE}};
  const Raw expected(expected_ptr, sizeof(expected_data), expected_format);

  // test
  Reformatter reformatter(raw_data, actions);
  auto result = reformatter.get();
  EXPECT_EQ(result, expected);
}

TEST(Model_Reformatter, shuffle_remove) {
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
      {Reformatter::MOVE, move_action : {2}},
      {Reformatter::MOVE, move_action : {1}},
      {Reformatter::MOVE, move_action : {3}}};

  // expected
  struct __attribute__((__packed__)) raw_expected {
    GLfloat x1;
    GLfloat x2, y2;
    GLfloat x3, y3, z3, w3;
  };
  const raw_expected expected_data[length] = {
      {0.3, 0.2, 0.2, 0.4, 0.4, 0.4, 0.4},
      {0.7, 0.6, 0.6, 0.8, 0.8, 0.8, 0.8},
  };
  const std::shared_ptr<void> expected_ptr((void*)&expected_data, [](void*) {});
  const std::vector<Raw::attribute> expected_format = {
      {{gl_float, 1}, GL_FALSE},
      {{gl_float, 2}, GL_FALSE},
      {{gl_float, 4}, GL_FALSE}};
  const Raw expected(expected_ptr, sizeof(expected_data), expected_format);

  // test
  Reformatter reformatter(raw_data, actions);
  auto result = reformatter.get();
  EXPECT_EQ(result, expected);
}

TEST(Model_Reformatter, cast_equal_size) {
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

TEST(Model_Reformatter, cast_different_size) {}

TEST(Model_Reformatter, cast_different_len) {}

TEST(Model_Reformatter, cast_normalize) {}

}  // namespace