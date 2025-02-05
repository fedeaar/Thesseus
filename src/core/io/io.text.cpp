#include "io.h"

std::string
io::text::read(const std::string& path)
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
    // TODO: use logger
    std::cerr << "File Error: " + path + " could not be read." << std::endl;
    throw e;
  }
}
