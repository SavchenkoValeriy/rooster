#include <support/InteractiveMode.h>

namespace interactive {
  template <>
  std::string parse(const std::string &input) {
    return input;
  }

  template <>
  unsigned parse(const std::string &input) {
    return std::stoul(input);
  }
}
