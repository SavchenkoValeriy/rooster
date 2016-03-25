#ifndef LLVM_ASTEXPRTRAITS_H
#define LLVM_ASTEXPRTRAITS_H

#include <boost/hana/type.hpp>

namespace hana = boost::hana;

template <typename ExprT>
struct ASTExprTraits;

enum class ExpressionsTypes {
#define DEFINE_FOR_ALL(CLASS)                                                  \
  ET_##CLASS,
#include "ASTMacroHelpers.h"
  ET_None
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
