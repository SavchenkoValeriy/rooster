set(SupportSources
  InteractiveModeTest.cpp
  JSONInputReaderTest.cpp
  )

add_rooster_unittest(SupportTests ${SupportSources})
target_link_libraries(SupportTests roosterSupport)
