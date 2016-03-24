add_clang_library(roosterCoreAST
  StatisticsCollector.cpp
  PlainPrinter.cpp
  ASTAction.h
  FileAction.h
  ASTExprTraits.h
)

set_property(TARGET roosterCoreAST PROPERTY CXX_STANDARD 14)

