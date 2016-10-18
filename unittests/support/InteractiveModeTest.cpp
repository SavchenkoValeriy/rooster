#include <support/InteractiveMode.h>
#include <support/InputReader.h>
#include <support/InputProvider.h>
#include <boost/lexical_cast.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

using namespace interactive;

namespace {
  class InputReaderMockObject {
  public:
    MOCK_METHOD1(getArguments, InputReader::CommandArgsContainer(const std::string &));
    MOCK_METHOD1(getCommand, std::string(const std::string &));
    std::string parseString(const std::string &arg) {
      return arg;
    }
    int parseInt(const std::string &arg) {
      return boost::lexical_cast<int>(arg);
    }
  };

  class InputProviderMockObject {
  public:
    MOCK_METHOD0(read, std::string());
  };

  class InputReaderMock : public InputReader {
  public:
    static void init() { mock = std::make_unique<InputReaderMockObject>(); }
    static void clear() { mock = nullptr;  }
    static CommandArgsContainer getArguments(const std::string &);
    static std::string getCommand(const std::string &);
    static std::unique_ptr<InputReaderMockObject> mock;

    template <typename T>
    static T parse(const std::string &arg);
  };

  template <>
  std::string InputReaderMock::parse(const std::string &arg) {
    return mock->parseString(arg);
  }
  template <>
  int InputReaderMock::parse(const std::string &arg) {
    return mock->parseInt(arg);
  }


  class InputProviderMock : public InputProvider {
  public:
    static void init() { mock = std::make_unique<InputProviderMockObject>(); }
    static void clear() { mock = nullptr;  }
    static std::string read() { return mock->read(); }
    static std::unique_ptr<InputProviderMockObject> mock;
  };

  InputReader::CommandArgsContainer InputReaderMock::getArguments(const std::string &input) {
    return mock->getArguments(input);
  }
  std::string InputReaderMock::getCommand(const std::string &input) {
    return mock->getCommand(input);
  }

  std::unique_ptr<InputReaderMockObject> InputReaderMock::mock;
  std::unique_ptr<InputProviderMockObject> InputProviderMock::mock;

  class CallbackProvider {
  public:
    MOCK_METHOD0(simple, void());
    MOCK_METHOD3(threeStrings, void(const std::string &,
                                    const std::string &,
                                    const std::string &));
  };
}

using ::testing::Return;
using ::testing::_;

using InteractiveModeUnderTest = InteractiveMode<InputReaderMock, InputProviderMock>;

class InteractiveModeTest : public ::testing::Test {
 protected:
  virtual void SetUp() override {
    InputReaderMock::init();
    InputProviderMock::init();
    EXPECT_CALL(*InputProviderMock::mock, read())
        .WillRepeatedly(Return(""));
  }

  virtual void TearDown() override {
    InputReaderMock::clear();
    InputProviderMock::clear();
    tested.clear();
  }

  InteractiveModeUnderTest tested;
};

TEST_F(InteractiveModeTest, SimpleCallbackTest) {
  CallbackProvider mock;
  CallbackTy<void> simpleCallback = [&mock]() { return mock.simple(); };
  tested.registerCallback("simple", simpleCallback);

  EXPECT_CALL(*InputReaderMock::mock, getCommand(_))
    .WillOnce(Return("simple"))
    .WillOnce(Return("exit"));
  EXPECT_CALL(*InputReaderMock::mock, getArguments(_));
  EXPECT_CALL(mock, simple())
    .Times(1);

  tested.run();
}

TEST_F(InteractiveModeTest, StringCallbackTest) {
  CallbackProvider mock;
  CallbackTy<void, std::string, std::string, std::string> stringsCallback =
    [&mock](std::string a, std::string b, std::string c) {
      return mock.threeStrings(a, b, c);
    };
  tested.registerCallback("threeStrings", stringsCallback);

  EXPECT_CALL(*InputReaderMock::mock, getCommand(""))
      .WillOnce(Return("threeStrings"))
      .WillOnce(Return("threeStrings"))
      .WillOnce(Return("exit"));
  EXPECT_CALL(*InputReaderMock::mock, getArguments(""))
      .WillOnce(Return<InputReader::CommandArgsContainer>({"la", "la", "la"}))
      .WillOnce(Return<InputReader::CommandArgsContainer>({"a", "b", "c"}));
  EXPECT_CALL(mock, threeStrings("la", "la", "la"));
  EXPECT_CALL(mock, threeStrings("a", "b", "c"));

  tested.run();
}
