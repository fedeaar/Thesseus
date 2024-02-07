#ifndef MODEL_REFORMATTER_
#define MODEL_REFORMATTER_

#include "../core/gl_cast.hpp"
#include "../core/types.h"
#include "../shader/shader.h"
#include "raw.h"

class Reformatter {
 public:
  enum action_type {
    MOVE,
    MOVE_CAST,
  };
  struct move {
    u32 from;
    action_type type = MOVE;
  };
  struct move_cast {
    u32 from;
    Raw::attribute cast;
    action_type type = MOVE_CAST;
  };
  struct action {
    action_type type;
    union {
      move move_action;
      move_cast cast_action;
    };
  };

 private:
  const std::vector<Raw::full_attribute> base_format_;
  const u32 base_length_, base_size_, base_stride_;
  u32 base_start_ = 0;
  std::shared_ptr<void> base_data_;

  const std::vector<action> actions_;

  std::vector<Raw::full_attribute> format_;
  u32 size_, stride_, start_ = 0;
  std::shared_ptr<void> data_;

  Raw result_;

  inline void _init_format();
  inline void _init_format_move(const move& elem, u32 idx);
  inline void _init_format_move_cast(const move_cast& elem, u32 idx);

  inline void _raw_copy();
  inline void _move_copy(const move& elem, u32 idx);
  inline void _move_cast_copy(const move_cast& elem, u32 idx);
  inline void _copy_int_cast(const move_cast& elem, u32 idx);

 public:
  Reformatter(const Raw& raw, const std::vector<action>& actions);

  Raw get();
};

#include "reformatter.inl"

#endif  // MODEL_REFORMATTER_
