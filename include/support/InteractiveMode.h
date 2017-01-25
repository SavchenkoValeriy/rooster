#ifndef ROOSTER_INTERACTIVEMODE_H
#define ROOSTER_INTERACTIVEMODE_H

#include <support/InteractiveModeException.h>
#include <support/InputReader.h>
#include <llvm/Support/raw_ostream.h>
#include <functional>
#include <map>

/// @Brief @namespace interactive contains features and functions related
/// to a generic mode to work interactively
namespace interactive {
  template <class ReturnTy, class ...ArgsTy>
  using CallbackTy = std::function<ReturnTy (ArgsTy...)>;

  template <class InputReaderTy,
            class InputProviderTy>
  class InteractiveMode {
  public:
    using Command = std::string;
    /// @Brief for the given command registers the callback that should be called
    ///
    /// By types of arguments of the given @p callback it infers what parsers must be used
    /// The number of arguments in the @p callback shows how many arguments there  should
    /// be in a @p command.
    template <class CallbackReturnTy, class ...CallbackArgsTy>
    void registerCallback(const Command &command,
                          CallbackTy<CallbackReturnTy, CallbackArgsTy...> &callback) {
      registerCallbackImpl(command, callback, std::index_sequence_for<CallbackArgsTy...>{});
    }

    void run() {
      while(true) {
        try {
          auto input = InputProviderTy::read();
          auto command = InputReaderTy::getCommand(input);

          // infinite loop shouldn't be really infinite
          if (command == exit)
            return;

          auto arguments = InputReaderTy::getArguments(input);
          auto result = callbacks.find(command);

          // trying to call unregistered command
          if (result == callbacks.end())
            throw WrongCommandException(command);

          try {
            auto callback = result->second;
            callback(arguments); // can throw
          } catch(InteractiveModeException &e) {
            // put failed command name and rethrow
            e.setFailedCommand(command);
            throw;
          }
        // if we failed with interactive mode exception
        // just rethrow it
        } catch (InteractiveModeException&) {
          throw;
        // otherwise smth
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
    Command exit = "exit"; /// default exit command is 'exit'
  };
}

#endif /* ROOSTER_INTERACTIVEMODE_H */
