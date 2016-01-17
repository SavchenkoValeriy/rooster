set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_subdirectory(cxx)

add_clang_executable(rooster
  cxx/Rooster.cpp
  )
target_link_libraries(rooster
  clangTooling
  clangBasic
  clangASTMatchers
  )
  
install(TARGETS rooster
  RUNTIME DESTINATION bin)