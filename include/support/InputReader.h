#ifndef ROOSTER_INPUTREADER_H
#define ROOSTER_INPUTREADER_H

#include <vector>
#include <string>

namespace interactive {
  using CommandArgsContainer = std::vector<std::string>;

  template <class T>
  struct InputReader {
    static CommandArgsContainer getArguments();
    static std::string getCommand();
  };
}

#endif /* ROOSTER_INPUTREADER_H */
