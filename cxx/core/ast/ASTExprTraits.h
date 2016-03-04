#ifndef LLVM_ASTEXPRTRAITS_H
#define LLVM_ASTEXPRTRAITS_H

template <typename ExprT>
struct ASTExprTraits;

enum class ExpressionsTypes {
#define DEFINE_FOR_ALL(CLASS)                                                  \
  ET_##CLASS,
#include "ASTMacroHelpers.h"
  ET_None
};

#define DEFINE_FOR_ALL(CLASS)                                                  \
template<>                                                                     \
struct ASTExprTraits<CLASS> {                                                  \
  static constexpr auto name = #CLASS ;                                        \
  static constexpr int value = static_cast<int>(ExpressionsTypes::ET_##CLASS); \
};
#include "ASTMacroHelpers.h"


#endif //LLVM_ASTEXPRTRAITS_H
