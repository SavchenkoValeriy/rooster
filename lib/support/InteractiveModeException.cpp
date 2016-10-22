#include <support/InteractiveModeException.h>
#include <sstream>

namespace {
  const std::string getSuffix(unsigned i) noexcept {
    if (i == 1) {
      return "st";
    } else if (i == 2) {
      return "nd";
    } else {
      return "th";
    }
  }
}

namespace interactive {
  void InteractiveModeException::setFailedCommand(const std::string &cmd) {
    command = cmd;
  }
  void InteractiveModeException::setMessage(const std::string &msg) {
    message = msg;
  }
  const char *InteractiveModeException::what() const noexcept {
    if (whatMessage.empty()) {
      std::stringstream ss;
      if (!command.empty())
        ss << "Command \'" << command << "\' has failed:\n";
      ss << message;
      whatMessage = ss.str();
    }
    return whatMessage.c_str();
  }

  ParseException::ParseException(unsigned i, const std::string &raw) :
    InteractiveModeException(), position(i) {
    std::stringstream ss;
    ss << "Couldn't parse the " << (i + 1) << getSuffix(i + 1) << " argument";
    if (!raw.empty())
      ss << ": " << raw;
    ss << ".";
    setMessage(ss.str());
  }

  unsigned ParseException::getPosition() const noexcept {
    return position;
  }

  WrongNumberOfArgumentsException::WrongNumberOfArgumentsException(unsigned expected,
                                                                   unsigned provided) :
    InteractiveModeException(),
    expected(expected),
    provided(provided) {
    std::stringstream ss;
    ss << "Wrong number of arguments has been provided (actual is "
       << provided << " when " << expected << " is expected).";
    setMessage(ss.str());
  }
  unsigned WrongNumberOfArgumentsException::getExpectedNumber() const noexcept {
    return expected;
  }
  unsigned WrongNumberOfArgumentsException::getProvidedNumber() const noexcept {
    return provided;
  }

  WrongCommandException::WrongCommandException(const std::string &cmd) :
    command(cmd) {
    std::stringstream ss;
    ss << "Wrong command (\"" << command << "\") has been provided.";
    setMessage(ss.str());
  }

  const std::string &WrongCommandException::getCommand() const noexcept {
    return command;
  }
}
