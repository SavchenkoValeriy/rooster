#ifndef ROOSTER_INTERACTIVEMODE_H
#define ROOSTER_INTERACTIVEMODE_H

#include <support/InputReader.h>
#include <llvm/Support/raw_ostream.h>
#include <functional>
#include <map>

// @brief @namespace interactive contains features and functions related
// to a generic mode to work interactively
namespace interactive {
  // @brief function to parse
  template <class ParsedType>
  ParsedType parse(const std::string &);

  template <class ReturnTy, class ...ArgsTy>
  using CallbackTy = std::function<ReturnTy (ArgsTy...)>;

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
        llvm::errs() << "Received input: " << input << "\n";
        auto command = InputReaderTy::getCommand(input);
        llvm::errs() << "Command: " << command << "\n";
        if (command == exit)
          return;
        auto arguments = InputReaderTy::getArguments(input);
        callbacks[command](arguments);
      }
    }

  private:
    using CommandArgs = typename InputReaderTy::CommandArgsContainer;
    using CallbackMap = std::map<Command, CallbackTy<void, CommandArgs &> >;

    template <class CallbackReturnTy, class ...CallbackArgsTy, std::size_t ...Idx>
    void registerCallbackImpl(const Command &command,
                              CallbackTy<CallbackReturnTy, CallbackArgsTy...> &callback,
                              std::index_sequence<Idx...> sequence) {
      callbacks[command] = [callback](CommandArgs &args) {
        callback(parseImpl<CallbackArgsTy, Idx>(args)...);
      };
    }

    template <class T, std::size_t position>
    static auto parseImpl(CommandArgs &args) {
      static_assert(std::is_reference<T>::value == 0,
                    "Argument shouldn't be a reference!");
      using ParsedTy = typename std::remove_cv<T>::type;
      return parse<ParsedTy>(args.at(position));
    }

    CallbackMap callbacks;
    Command exit = "exit";
  };
}

#endif /* ROOSTER_INTERACTIVEMODE_H */
