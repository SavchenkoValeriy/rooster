#ifndef ROOSTER_INTERACTIVEMODE_H
#define ROOSTER_INTERACTIVEMODE_H

#include <support/InputReader.h>
#include <functional>
#include <map>

// @brief namespace interactive contains features and functions related
// to a generic mode to work interactively
namespace interactive {
  // @brief function to parse
  template <class ParsedType>
  ParsedType parse(const std::string &);

  template <class ReturnTy, class ...ArgsTy>
  using CallbackTy = std::function<ReturnTy (ArgsTy...)>;

  template <class CommandTy, class InputReaderTy>
  class InteractiveMode {
  public:
    template <class CallbackReturnTy, class ...CallbackArgsTy>
    void registerCallback(typename CommandTy::Command command,
                          CallbackTy<CallbackReturnTy, CallbackArgsTy...> &callback) {
      registerCallbackImpl(command, callback, std::index_sequence_for<CallbackArgsTy...>{});
    }

  private:
    using CommandArgs = typename InputReaderTy::CommandArgsContainer;
    using CallbackMap = std::map<typename CommandTy::Command,
                                 CallbackTy<void, CommandArgs &> >;

    template <class CallbackReturnTy, class ...CallbackArgsTy, std::size_t ...Idx>
    void registerCallbackImpl(typename CommandTy::Command command,
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
  };
}

#endif /* ROOSTER_INTERACTIVEMODE_H */
