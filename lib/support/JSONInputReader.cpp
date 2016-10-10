#include <support/JSONInputReader.h>
#include <boost/lexical_cast.hpp>
#include <string>

using namespace nlohmann;

namespace {
  struct JSONInputCache {
    std::string cachedInput = "";
    json parsed;
    bool isCached(const std::string &input) const {
      return input == cachedInput;
    }
    const json &getCache() {
      return parsed;
    }
    void pushCache(json &&newCachedValue, const std::string &forInput) {
      parsed = newCachedValue;
      cachedInput = forInput;
    }
  };

  const json &readJSON(const std::string &input) {
    static JSONInputCache cache;
    if (!cache.isCached(input)) {
      cache.pushCache(json::parse(input), input);
    }
    return cache.getCache();
  }
}

namespace interactive {
  JSONInputReader::CommandArgsContainer JSONInputReader::getArguments(const std::string &userInput) {
    auto jsonCommand = readJSON(userInput);
    if (jsonCommand.size() == 0)
      return CommandArgsContainer();
    return {jsonCommand.begin() + 1, jsonCommand.end()};
  }
  std::string JSONInputReader::getCommand(const std::string &userInput) {
    auto jsonCommand = readJSON(userInput);
    if (jsonCommand.size() == 0)
      return "";
    return jsonCommand[0];
  }
}
