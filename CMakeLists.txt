cmake_minimum_required(VERSION 3.4.3)

project(Rooster
  ${cmake_3_0_PROJ_VERSION}
  ${cmake_3_0_LANGUAGES}
  CXX)

set(ROOSTER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

find_package(Clang REQUIRED CONFIG)

# Add path for custom modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${LLVM_CMAKE_DIR}"
  )

include(AddLLVM)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${ROOSTER_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})
message("${LLVM_COMPILE_FLAGS}")

add_subdirectory(lib)
