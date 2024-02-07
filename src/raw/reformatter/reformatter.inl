
inline void Reformatter::_init_format_move(const move& elem, u32 idx) {
  assert(elem.from < base_format_.size());  // TODO: better exceptions
  const Raw::full_attribute& data = base_format_[elem.from];
  format_.push_back({data.attr, base_start_, data.size});
  base_start_ += data.size;
}

inline void Reformatter::_init_format_move_cast(const move_cast& elem,
                                                u32 idx) {
  assert(elem.from < base_format_.size());  // TODO: better exceptions
  assert(elem.cast.format.length <= base_format_[elem.from].attr.format.length);
  const Raw::full_attribute& data = Raw::extend_format(elem.cast, base_start_);
  format_.push_back(data);
  base_start_ += data.size;
}

inline void Reformatter::_init_format() {
  base_start_ = 0;
  for (u32 i = 0; i < actions_.size(); ++i) {
    const action& elem = actions_[i];
    switch (elem.type) {
      case MOVE:
        _init_format_move(elem.move_action, i);
        break;
      case MOVE_CAST:
        _init_format_move_cast(elem.cast_action, i);
        break;
    }
  }
}

inline void Reformatter::_move_copy(const move& elem, u32 idx) {
  const Raw::full_attribute& data = base_format_[elem.from];
  u32 size = data.size;
  void* from = base_data_.get() + base_start_ + data.start;
  void* to = data_.get() + start_;
  memcpy(to, from, size);
  start_ += size;
}

inline void Reformatter::_move_cast_copy(const move_cast& elem, u32 idx) {
  const Raw::full_attribute& data = base_format_[elem.from];
  void* from = base_data_.get() + base_start_ + data.start;
  void* to = data_.get() + start_;
  u32 size = format_[idx].size;
  u8 buf[size];
  gl_copy_cast(from, &buf, elem.cast.format.length, data.attr.format.gl_type,
               elem.cast.format.gl_type);
  mempcpy(to, &buf, size);
  start_ += size;
}

inline void Reformatter::_raw_copy() {
  start_ = 0;
  for (base_start_ = 0; base_start_ < base_size_; base_start_ += base_stride_) {
    for (u32 i = 0; i < actions_.size(); ++i) {
      const action& elem = actions_[i];
      switch (elem.type) {
        case MOVE:
          _move_copy(elem.move_action, i);
          break;
        case MOVE_CAST:
          _move_cast_copy(elem.cast_action, i);
          break;
      }
    }
  }
}
