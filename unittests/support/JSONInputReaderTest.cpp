#include <support/JSONInputReader.h>
#include <gtest/gtest.h>

using namespace interactive;

TEST(JSONInputReaderTests, SimpleTest) {
  auto request = "[\"custom\"]";
  auto command = JSONInputReader::getCommand(request);
  ASSERT_EQ(command, "custom");
  auto args = JSONInputReader::getArguments(request);
  ASSERT_EQ(args.size(), 0);
}

TEST(JSONInputReaderTests, TestWithIntegers) {
  auto request = "[\"action\", 1, 2, 3, 4, 5]";
  auto args = JSONInputReader::getArguments(request);
  ASSERT_EQ(args.size(), 5);
}
