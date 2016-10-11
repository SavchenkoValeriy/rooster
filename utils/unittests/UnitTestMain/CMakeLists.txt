add_llvm_library(gtest_main
  TestMain.cpp

  LINK_LIBS
  ${GTEST_BOTH_LIBRARIES}

  LINK_COMPONENTS
  Support # Depends on llvm::cl
  )

set_property(TARGET gtest_main PROPERTY CXX_STANDARD 14)
