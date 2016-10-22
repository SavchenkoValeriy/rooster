#ifndef ROOSTER_INTERACTIVEMODEEXCEPTION_H
#define ROOSTER_INTERACTIVEMODEEXCEPTION_H

#include <exception>
#include <string>

namespace interactive {
  class InteractiveModeException : public std::exception {
  public:
    void setFailedCommand(const std::string &cmd);
    void setMessage(const std::string &msg);
    virtual const char *what() const noexcept override;
    InteractiveModeException() = default;
    virtual ~InteractiveModeException() = default;
  private:
    std::string message, command;
    mutable std::string whatMessage;
  };

  class ParseException : public InteractiveModeException {
  public:
    ParseException(unsigned i, const std::string &raw);
    unsigned getPosition() const noexcept;
  private:
    unsigned position;
  };

  class WrongNumberOfArgumentsException : public InteractiveModeException {
  public:
    WrongNumberOfArgumentsException(unsigned expected, unsigned provided);
    unsigned getExpectedNumber() const noexcept;
    unsigned getProvidedNumber() const noexcept;
  private:
    unsigned expected, provided;
  };

  class WrongCommandException : public InteractiveModeException {
  public:
    WrongCommandException(const std::string &command);
    const std::string &getCommand() const noexcept;
  private:
    std::string command;
  };

  class IllegalFormatException : public InteractiveModeException {
  public:
    IllegalFormatException();
  };
}

#endif /* ROOSTER_INTERACTIVEMODEEXCEPTION_H */
