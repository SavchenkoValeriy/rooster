cmake_minimum_required(VERSION 3.2.2)

project(Rooster
  ${cmake_3_0_PROJ_VERSION}
  ${cmake_3_0_LANGUAGES}
  CXX)

# All includes are in rooster/include directory
set(ROOSTER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Need Clang and LLVM to compile
find_package(Clang REQUIRED CONFIG)

# Add path for custom modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  "${LLVM_CMAKE_DIR}"
  )

# Import LLVM cmake functions for simplier linking
include(AddLLVM)

include(AddRooster)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${ROOSTER_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})

# Google test part
enable_testing()
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GMOCK_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory(lib)
add_subdirectory(utils/unittests)
add_subdirectory(unittests)
