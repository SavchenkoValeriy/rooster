#ifndef LLVM_ASTEXPRTRAITS_H
#define LLVM_ASTEXPRTRAITS_H

#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/unpack.hpp>

namespace hana = boost::hana;

template <typename ExprT>
struct ASTExprTraits;

enum class ExpressionsTypes {
#define DEFINE_FOR_ALL(CLASS)                                                  \
  ET_##CLASS,
#include "ASTMacroHelpers.h"
  ET_None
};

template <class RetTy, class FuncTy, class ...ArgTypes>
auto getGenericValue(hana::tuple<ArgTypes...> args,
                     RetTy &&defaultValue, FuncTy &&func) {
  return hana::unpack(args, func).value_or(defaultValue);
};

template <class RetTy, class FuncTy, class ...ArgTypes, class ...FuncTypes>
auto getGenericValue(hana::tuple<ArgTypes...> args, RetTy &&defaultValue,
                     FuncTy &&func, FuncTypes && ...funcs) {
  return hana::unpack(args, func).
    value_or(getGenericValue(args, defaultValue, funcs...));
};

auto hasGetBodyMethod =
  hana::is_valid([](auto *p) -> decltype((void)p->getBody()) {});
auto hasGetChildrenMethod =
  hana::is_valid([](auto *p) -> decltype((void)p->children()) {});
auto hasGetLocationMethod =
  hana::is_valid([](auto *p) -> decltype((void)p->getLocation()) {});

#define DEFINE_FOR_ALL(CLASS)                                                  \
template<>                                                                     \
struct ASTExprTraits<CLASS> {                                                  \
  static constexpr auto name = #CLASS ;                                        \
  static constexpr int value = static_cast<int>(ExpressionsTypes::ET_##CLASS); \
};
#include "ASTMacroHelpers.h"


#endif //LLVM_ASTEXPRTRAITS_H
