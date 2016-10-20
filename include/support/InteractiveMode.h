#ifndef ROOSTER_INTERACTIVEMODE_H
#define ROOSTER_INTERACTIVEMODE_H

#include <support/InputReader.h>
#include <llvm/Support/raw_ostream.h>
#include <exception>
#include <functional>
#include <map>
#include <sstream>

// @brief @namespace interactive contains features and functions related
// to a generic mode to work interactively
namespace interactive {
  template <class ReturnTy, class ...ArgsTy>
  using CallbackTy = std::function<ReturnTy (ArgsTy...)>;

  class InteractiveException : public std::exception {
  public:
    void setFailedCommand(const std::string &cmd) {
      command = cmd;
    }
    void setMessage(const std::string &msg) {
      message = msg;
    }
    virtual const char *what() const noexcept override {
      if (whatMessage.empty()) {
        std::stringstream ss;
        if (!command.empty())
          ss << "Command \'" << command << "\' has failed:\n";
        ss << message;
        whatMessage = ss.str();
      }
      return whatMessage.c_str();
    }
    InteractiveException() = default;
    virtual ~InteractiveException() = default;
  private:
    std::string message, command;
    mutable std::string whatMessage;
  };

   class ParseException : public InteractiveException {
   public:
     ParseException(unsigned i, const std::string &raw) :
       InteractiveException(), position(i) {
       std::stringstream ss;
       ss << "Couldn't parse the " << (i + 1) << getSuffix(i) << " argument";
       if (!raw.empty())
         ss << ": " << raw;
       ss << ".";
       setMessage(ss.str());
     }
     unsigned getPosition() const noexcept {
       return position;
     }
   private:
     static const std::string getSuffix(unsigned i) noexcept {
       if (i == 1) {
         return "st";
       } else if (i == 2) {
         return "nd";
       } else {
         return "th";
       }
     }
     unsigned position;
   };

  class WrongNumberOfArgumentsException : public InteractiveException {
  public:
    WrongNumberOfArgumentsException(unsigned expected, unsigned provided) :
      InteractiveException(),
      expected(expected),
      provided(provided) {
      std::stringstream ss;
      ss << "Wrong number of arguments has been provided (actual is "
         << provided << " when " << expected << " is expected).";
      setMessage(ss.str());
    }
    unsigned getExpectedNumber() const noexcept {
      return expected;
    }
    unsigned getProvidedNumber() const noexcept {
      return provided;
    }
  private:
    unsigned expected, provided;

  };

  template <class InputReaderTy,
            class InputProviderTy>
  class InteractiveMode {
  public:
    using Command = std::string;
    template <class CallbackReturnTy, class ...CallbackArgsTy>
    void registerCallback(const Command &command,
                          CallbackTy<CallbackReturnTy, CallbackArgsTy...> &callback) {
      registerCallbackImpl(command, callback, std::index_sequence_for<CallbackArgsTy...>{});
    }

    void run() {
      while(true) {
        auto input = InputProviderTy::read();
        auto command = InputReaderTy::getCommand(input);
        if (command == exit)
          return;
        auto arguments = InputReaderTy::getArguments(input);
        try {
          callbacks[command](arguments);
        } catch(InteractiveException &e) {
          e.setFailedCommand(command);
          throw;
        }
      }
    }

    void clear() {
      callbacks.clear();
    }

  private:
    using CommandArgs = typename InputReaderTy::CommandArgsContainer;
    using CallbackMap = std::map<Command, CallbackTy<void, CommandArgs &> >;

    template <class CallbackReturnTy, class ...CallbackArgsTy, std::size_t ...Idx>
    void registerCallbackImpl(const Command &command,
                              CallbackTy<CallbackReturnTy, CallbackArgsTy...> &callback,
                              std::index_sequence<Idx...> sequence) {
      callbacks[command] = [callback, &sequence](CommandArgs &args) {
        if (sequence.size() != args.size())
          throw WrongNumberOfArgumentsException(sequence.size(), args.size());
        callback(parseImpl<CallbackArgsTy, Idx>(args)...);
      };
    }

    template <class T, std::size_t position>
    static auto parseImpl(CommandArgs &args) {
      static_assert(std::is_reference<T>::value == 0,
                    "Argument shouldn't be a reference!");
      using ParsedTy = typename std::remove_cv<T>::type;
      try {
        return InputReaderTy::template parse<ParsedTy>(args.at(position));
      } catch(...) {
        throw ParseException(position, args.at(position));
      }
    }

    CallbackMap callbacks;
    Command exit = "exit";
  };
}

#endif /* ROOSTER_INTERACTIVEMODE_H */
