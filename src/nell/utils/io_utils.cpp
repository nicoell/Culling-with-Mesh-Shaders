#include <spdlog/spdlog.h>

#include <fstream>
#include <utils/io_utils.hpp>

namespace io_utils
{
std::string loadFile(const std::string &file)
{
  std::ifstream in_file(file);
  if (!in_file)
  {
    spdlog::error("Error opening file {}", file);
    throw std::system_error(errno, std::system_category(),
                            "Error opening file.");
  }
  return std::string{std::istreambuf_iterator{in_file}, {}};
}
}  // namespace io_utils
