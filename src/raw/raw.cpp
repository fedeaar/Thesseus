#include "raw.h"

//
// public
//

Raw::Raw() {}

Raw::Raw(std::shared_ptr<void> raw, u32 size,
         const std::vector<Raw::full_attribute>& format)
    : raw_(raw), size_(size), format_(format) {
  stride_ = Raw::calculate_stride(format_);
  assert(size % stride_ == 0);  // TODO: better exceptions
  length_ = size / stride_;
}

Raw::Raw(std::shared_ptr<void> raw, u32 size,
         const std::vector<Raw::attribute>& format)
    : raw_(raw), size_(size) {
  format_ = Raw::extend_format(format);
  stride_ = Raw::calculate_stride(format_);
  assert(size % stride_ == 0);  // TODO: better exceptions
  length_ = size / stride_;
}

std::shared_ptr<void> Raw::data() const { return raw_; };

const std::vector<Raw::full_attribute>& Raw::format() const { return format_; }

u32 Raw::length() const { return length_; }

u32 Raw::size() const { return size_; }

u32 Raw::stride() const { return stride_; }

//
// overloads
//

bool Raw::attribute::operator==(const attribute& rhs) const {
  return format == rhs.format && is_normalized == rhs.is_normalized &&
         should_skip == rhs.should_skip;
}

bool Raw::full_attribute::operator==(const full_attribute& rhs) const {
  return attr == rhs.attr && start == rhs.start && size == rhs.size;
}

bool Raw::operator==(const Raw& rhs) const {
  bool out = size_ == rhs.size_ && length_ == rhs.length_ &&
             stride_ == rhs.stride_ && format_ == rhs.format_;
  const u8* lhs_ptr = (u8*)raw_.get();
  const u8* rhs_ptr = (u8*)rhs.raw_.get();
  if (lhs_ptr != NULL && rhs_ptr != NULL) {
    for (u32 i = 0; i < size_ && out; ++i) {
      out = out && lhs_ptr[i] == rhs_ptr[i];
    }
  } else {
    out = out && lhs_ptr == rhs_ptr;
  }
  return out;
}

std::ostream& operator<<(std::ostream& stream, const Raw& rhs) {
  u8* start = (u8*)rhs.raw_.get();
  u32 s_len = rhs.size_;
  u32 f_len = rhs.format_.size();
  stream << "[";
  for (u32 offset = 0; offset < s_len;) {
    for (u32 j = 0; j < f_len; ++j) {
      const Raw::full_attribute& data = rhs.format_[j];
      gl_display(start + offset, data.attr.format.gl_type,
                 data.attr.format.length, stream);
      if (j + 1 < f_len) {
        stream << " , ";
      }
      offset += data.size;
    }
    if (offset + 1 < s_len) {
      stream << " | ";
    }
  }
  stream << "]";
  return stream;
}
