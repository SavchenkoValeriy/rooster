add_llvm_executable(rooster
  Rooster.cpp
  )

target_link_libraries(rooster
  roosterSupport
  roosterCoreAST
  roosterCompletion
  clangTooling
  )

set_property(TARGET rooster PROPERTY CXX_STANDARD 14)

install(TARGETS rooster
  RUNTIME DESTINATION bin)
