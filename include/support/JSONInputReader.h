#ifndef ROOSTER_JSONINPUTREADER_H
#define ROOSTER_JSONINPUTREADER_H

#include <support/InputReader.h>
#include <nlohmann/json.hpp>

namespace interactive {
  class JSONInputReader : public InputReader {
  public:
    using CommandArgsContainer = nlohmann::json;
    using CommandArgument = nlohmann::json;
    static CommandArgsContainer getArguments(const std::string &);
    static std::string getCommand(const std::string &);
    template <class T> static T parse(const CommandArgument &argument) {
      return argument.get<T>();
    }
  };
}

#endif /* ROOSTER_JSONINPUTREADER_H */
