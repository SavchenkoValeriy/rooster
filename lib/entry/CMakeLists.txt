add_rooster_executable(rooster
  Rooster.cpp
  )

target_link_libraries(rooster
  roosterSupport
  roosterCompletion
  )

install(TARGETS rooster
  RUNTIME DESTINATION bin)
