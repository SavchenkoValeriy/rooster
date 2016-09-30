add_llvm_library(roosterCompletion
  CompletionTool.cpp
  CompletionPrinter.cpp)

target_link_libraries(roosterCompletion
  clangTooling
  )

set_property(TARGET roosterCompletion PROPERTY CXX_STANDARD 14)
