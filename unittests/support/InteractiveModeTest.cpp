#include <support/InteractiveMode.h>
#include <support/InputReader.h>
#include <support/InputProvider.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

using namespace interactive;

namespace {
  class InputReaderMockObject {
  public:
    MOCK_METHOD1(getArguments, InputReader::CommandArgsContainer(const std::string &));
    MOCK_METHOD1(getCommand, std::string(const std::string &));
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
  };

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
  }

  virtual void TearDown() override {
    InputReaderMock::clear();
    InputProviderMock::clear();
  }
};

TEST_F(InteractiveModeTest, SimpleCallbackTest) {
  CallbackProvider mock;
  InteractiveModeUnderTest tested;
  CallbackTy<void> simpleCallback = [&mock]() { return mock.simple(); };
  tested.registerCallback("simple", simpleCallback);

  EXPECT_CALL(*InputProviderMock::mock, read())
    .WillRepeatedly(Return(""));
  EXPECT_CALL(*InputReaderMock::mock, getCommand(_))
    .WillOnce(Return("simple"))
    .WillOnce(Return("exit"));
  EXPECT_CALL(*InputReaderMock::mock, getArguments(_));
  EXPECT_CALL(mock, simple())
    .Times(1);

  tested.run();
}
