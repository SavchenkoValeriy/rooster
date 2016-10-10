#ifndef ROOSTER_INPUTREADER_H
#define ROOSTER_INPUTREADER_H

#include <vector>
#include <string>

namespace interactive {
  using CommandArgsContainer = std::vector<std::string>;

  template <class T>
  struct InputReader {
    static CommandArgsContainer getArguments(const std::string &);
    static std::string getCommand(const std::string &);
  };
}

#endif /* ROOSTER_INPUTREADER_H */
