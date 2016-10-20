set(LLVM_LINK_COMPONENTS support)

add_rooster_library(roosterSupport
  CommandLineOptions.cpp
  JSONInputReader.cpp
  ConsoleInputProvider.cpp
  )

set_property(TARGET roosterSupport PROPERTY CXX_STANDARD 14)
