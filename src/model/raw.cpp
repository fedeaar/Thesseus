#include "raw.h"

//
// public
//

bool Raw::attribute::operator==(const attribute& rhs) const {
  return format == format && normalize == normalize;
}

bool Raw::full_attribute::operator==(const full_attribute& rhs) const {
  return embedded == embedded && start == start && size == size;
}

Raw::Raw() {}

Raw::Raw(std::shared_ptr<void> raw, u32 size,
         const std::vector<Raw::full_attribute>& format)
    : raw_(raw), size_(size) {
  format_ = format;
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

bool Raw::operator==(const Raw& rhs) const {
  bool out = size_ == rhs.size_ && length_ == rhs.length_ &&
             stride_ == rhs.stride_ && format_ == rhs.format_;
  const u8* lhs_ptr = (u8*)raw_.get();
  const u8* rhs_ptr = (u8*)rhs.raw_.get();
  if (lhs_ptr != NULL && rhs_ptr != NULL) {
    for (u32 i = 0; i < size_ && out; ++i) {
      out &= lhs_ptr[i] == rhs_ptr[i];
    }
  } else {
    out &= lhs_ptr == rhs_ptr;  // == NULL
  }
  return out;
}

std::ostream& operator<<(std::ostream& stream, Raw rhs) {
  u8* start = (u8*)rhs.raw_.get();
  stream << "[";
  u32 s_len = rhs.size_;
  for (u32 i = 0; i < s_len;) {
    u32 f_len = rhs.format_.size();
    for (u32 j = 0; j < f_len; ++j) {
      const Raw::full_attribute& data = rhs.format_[j];
      u32 d_len = data.embedded.format.length;
      for (u32 k = 0; k < d_len; ++k) {
        switch (data.embedded.format.gl_type) {
          case gl_float:
            stream << ((float*)(start + i))[k];
            break;
          case gl_int:
            stream << ((int*)(start + i))[k];
            break;
          default:
            stream << "unknown gl_type: " << data.embedded.format.gl_type;
        }
        if (k + 1 < d_len) {
          stream << " ";
        }
      }
      if (j + 1 < f_len) {
        stream << " , ";
      }
      i += data.size;
    }
    if (i + 1 < s_len) {
      stream << " | ";
    }
  }
  stream << "]";

  return stream;
}
