add_subdirectory(ast)

add_library(roosterCore
  Ngram.h
  NgramCollector.h
  Dummy.cpp
)

target_link_libraries(roosterCore
  roosterCoreAST)