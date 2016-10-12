#include <support/JSONInputReader.h>
#include <gtest/gtest.h>
#include <boost/hana/size.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/tuple.hpp>

using namespace interactive;
using namespace boost;

TEST(JSONInputReaderTests, SimpleTest) {
  auto request = "[\"custom\"]";
  auto command = JSONInputReader::getCommand(request);
  ASSERT_EQ(command, "custom");
  auto args = JSONInputReader::getArguments(request);
  ASSERT_EQ(args.size(), 0);
}

template <class T, class U>
void compare(const T &actual, const U &expected) {
  ASSERT_EQ(actual.template get<U>(), expected);
}

template <class T>
void compare(const T &actual, const char *expected) {
  std::string expectedString = expected;
  compare(actual, expectedString);
}

template <class T, class ...Elements>
inline void compare(const T &actual, hana::tuple<Elements...> &&expected) {
  auto ai = actual.begin(), ae = actual.end();
  ASSERT_EQ(actual.size(), static_cast<size_t>(hana::size(expected)));
  hana::for_each(expected, [&ai, &ae](auto &x) {
      compare(*ai++, x);
    });
}

TEST(JSONInputReaderTests, TestWithIntegers) {
  auto request = "[\"action\", 1, 2, 3, 4, 5]";
  auto args = JSONInputReader::getArguments(request);
  compare(args, hana::make_tuple(1, 2, 3, 4, 5));
}

TEST(JSONInputReaderTests, TestWithStrings) {
  auto request = "[\"crawl\", \"la\", \"la-la\", \"la-la-la\"]";
  auto args = JSONInputReader::getArguments(request);
  compare(args, hana::make_tuple("la", "la-la", "la-la-la"));
}

TEST(JSONInputReaderTests, TestMixed) {
  auto request = "[\"born\", 16, \"March\", 1989]";
  auto args = JSONInputReader::getArguments(request);
  compare(args, hana::make_tuple(16, "March", 1989));
}
