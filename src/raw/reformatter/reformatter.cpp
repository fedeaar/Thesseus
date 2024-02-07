#include "reformatter.h"

Reformatter::Reformatter(const Raw& raw, const std::vector<action>& actions)
    : base_format_(raw.format()),
      base_length_(raw.length()),
      base_size_(raw.size()),
      base_stride_(raw.stride()),
      base_data_(raw.data()),
      actions_(actions) {
  _init_format();
  stride_ = Raw::calculate_stride(format_);
  size_ = base_length_ * stride_;
  data_ = std::shared_ptr<u8>(new u8[size_]);
  _raw_copy();
  result_ = Raw(data_, size_, format_);
}

Raw Reformatter::get() { return result_; }
