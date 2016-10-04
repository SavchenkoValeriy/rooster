#ifndef ROOSTER_INTERACTIVEMODE_H
#define ROOSTER_INTERACTIVEMODE_H

#include <llvm/ADT/DenseMap.h>
#include <functional>
#include <map>

namespace interactive {
  template <class ParsedType>
  ParsedType parse(const std::string &);

  template <class ReturnTy, class ...ArgsTy>
  using CallbackTy = std::function<ReturnTy (ArgsTy...)>;


  template <class CommandTy>
  class InteractiveMode {
  public:
    template <class CallbackReturnTy, class ...CallbackArgsTy>
    void registerCallback(typename CommandTy::Command command,
                          CallbackTy<CallbackReturnTy, CallbackArgsTy...> &callback) {
      registerCallbackImpl(command, callback, std::index_sequence_for<CallbackArgsTy...>{});
    }

  private:
    using CommandArgsContainer = std::vector<std::string>;
    using CallbackMap = std::map<typename CommandTy::Command,
                                 CallbackTy<void, CommandArgsContainer &> >;

    template <class CallbackReturnTy, class ...CallbackArgsTy, std::size_t ...Idx>
    void registerCallbackImpl(typename CommandTy::Command command,
                              CallbackTy<CallbackReturnTy, CallbackArgsTy...> &callback,
                              std::index_sequence<Idx...> sequence) {
      callbacks[command] = [callback](CommandArgsContainer &args) {
        callback(parseImpl<CallbackArgsTy, Idx>(args)...);
      };
    }

    template <class T, std::size_t position>
    static auto parseImpl(CommandArgsContainer &args) {
      return parse<T>(args.at(position));
    }

    CallbackMap callbacks;
  };
}

#endif /* ROOSTER_INTERACTIVEMODE_H */
