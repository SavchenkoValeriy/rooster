set(LLVM_LINK_COMPONENTS support)

add_llvm_library(roosterSupport
  CommandLineOptions.cpp
  InteractiveMode.cpp
  JSONInputReader.cpp
  )

set_property(TARGET roosterSupport PROPERTY CXX_STANDARD 14)
