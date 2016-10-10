#ifndef ROOSTER_JSONINPUTREADER_H
#define ROOSTER_JSONINPUTREADER_H

#include <support/InputReader.h>
#include <nlohmann/json.hpp>

namespace interactive {
  class JSONInputReader : public InputReader<JSONInputReader> {
  public:
    using CommandArgsContainer = nlohmann::json;
    static CommandArgsContainer getArguments(const std::string &);
    static std::string getCommand(const std::string &);
  };
}

#endif /* ROOSTER_JSONINPUTREADER_H */
