inline Raw::full_attribute Raw::extend_format(const Raw::attribute& data,
                                              u32 start) {
  const u32 size = data.format.length * gl_sizeof(data.format.gl_type);
  return {data, start, size};
}

inline std::vector<Raw::full_attribute> Raw::extend_format(
    const std::vector<Raw::attribute>& format) {
  std::vector<Raw::full_attribute> out;
  u32 start = 0;
  for (const Raw::attribute& data : format) {
    const u32 size = data.format.length * gl_sizeof(data.format.gl_type);
    out.push_back({data, start, size});
    start += size;
  }
  return out;
}

inline u32 Raw::calculate_stride(
    const std::vector<Raw::full_attribute>& format) {
  const Raw::full_attribute& last = format[format.size() - 1];
  return last.start + last.size;
}
