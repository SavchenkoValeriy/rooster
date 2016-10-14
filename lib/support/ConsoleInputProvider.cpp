#include <support/ConsoleInputProvider.h>
#include <llvm/ADT/StringExtras.h>
#include <iostream>
#include <vector>

namespace interactive {
  std::string ConsoleInputProvider::read() {
    std::vector<std::string> lines;
    unsigned index = 0;
    while (true) {
      lines.push_back("");
      std::string &buffer = lines.back();
      std::getline(std::cin, buffer);
      if (buffer.empty()) {
        lines.pop_back();
        break;
      }
    }
    return std::move(llvm::join(lines.begin(), lines.end(), ""));
  }
}
