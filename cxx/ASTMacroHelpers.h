#ifndef DEFINE_FOR_ALL
#  define DEFINE_FOR_ALL(CLASS)
#endif

#define ABSTRACT_STMT(CLASS)
#define STMT(CLASS, PARENT) DEFINE_FOR_ALL(CLASS)
#include "clang/AST/StmtNodes.inc"

#define ABSTRACT_DECL(CLASS)
#define DECL(CLASS, BASE) DEFINE_FOR_ALL(CLASS##Decl)
#include "clang/AST/DeclNodes.inc"

#undef DEFINE_FOR_ALL