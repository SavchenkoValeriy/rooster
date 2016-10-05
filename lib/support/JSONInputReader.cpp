#include <support/JSONInputReader.h>

namespace interactive {
  template <>
  CommandArgsContainer JSONInputReader::getArguments() { return CommandArgsContainer(); }
  template <>
  std::string JSONInputReader::getCommand() { return ""; }
}
