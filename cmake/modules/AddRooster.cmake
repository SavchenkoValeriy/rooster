# Generic support for adding a unittest.
function(add_unittest test_suite test_name)
  add_llvm_executable(${test_name} ${ARGN})
  set(outdir ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
  set_output_directory(${test_name} BINARY_DIR ${outdir} LIBRARY_DIR ${outdir})
  target_link_libraries(${test_name} gtest_main)
  target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES})
  target_link_libraries(${test_name} ${GMOCK_BOTH_LIBRARIES})
  target_link_libraries(${test_name} ${CMAKE_THREAD_LIBS_INIT})
  add_test(${test_suite} ${test_name})
  add_dependencies(${test_suite} ${test_name})
  get_target_property(test_suite_folder ${test_suite} FOLDER)
  set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 14)
  if (NOT ${test_suite_folder} STREQUAL "NOTFOUND")
    set_property(TARGET ${test_name} PROPERTY FOLDER "${test_suite_folder}")
  endif ()
endfunction()
