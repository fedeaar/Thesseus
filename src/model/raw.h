#ifndef MODEL_RAW_
#define MODEL_RAW_

#include <memory>
#include <vector>

#include "../core/types.h"
#include "../shader/shader.h"

class Raw {
 public:
  struct attribute {
    Shader::input format;
    GLboolean normalize;

    bool operator==(const attribute& rhs) const;
  };
  struct full_attribute {
    attribute embedded;
    u32 start;
    u32 size;

    bool operator==(const full_attribute& rhs) const;
  };

  static inline Raw::full_attribute extend_format(
      const Raw::attribute& attribute, u32 start);
  static inline std::vector<Raw::full_attribute> extend_format(
      const std::vector<Raw::attribute>& format);
  static inline u32 calculate_stride(
      const std::vector<Raw::full_attribute>& format);

 private:
  std::shared_ptr<void> raw_ = NULL;
  u32 size_ = 0, length_ = 0, stride_ = 0;
  std::vector<full_attribute> format_;

 public:
  Raw();
  Raw(std::shared_ptr<void> raw, u32 size,
      const std::vector<full_attribute>& format);
  Raw(std::shared_ptr<void> raw, u32 size,
      const std::vector<attribute>& format);

  std::shared_ptr<void> data() const;
  const std::vector<full_attribute>& format() const;
  u32 length() const;
  u32 size() const;
  u32 stride() const;

  bool operator==(const Raw& rhs) const;

  friend std::ostream& operator<<(std::ostream& stream, Raw rhs);
};

#include "raw.inl"

#endif  // MODEL_RAW_
