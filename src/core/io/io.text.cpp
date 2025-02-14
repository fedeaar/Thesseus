#include "io.h"

std::string
core::io::text::read(std::string const& path)
{
  std::ifstream stream;
  stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    stream.open(path);
    std::string data((std::istreambuf_iterator<char>(stream)),
                     (std::istreambuf_iterator<char>()));
    stream.close();
    return data;
  } catch (std::ifstream::failure& e) {
    Logger::err(
      "core::io::text::read", "file Error: {} could not be read.", path);
    throw e;
  }
}
