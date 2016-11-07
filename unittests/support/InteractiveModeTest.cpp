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
  template <>
  unsigned InputReaderMock::parse(const std::string &arg) {
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

  class ComplexObject {
  public:
    int index;
    std::string name;
  };

  bool operator== (const ComplexObject &rhs, const ComplexObject &lhs) {
    return rhs.index == lhs.index && rhs.name == lhs.name;
  }
  bool operator!= (const ComplexObject &rhs, const ComplexObject &lhs) {
    return !(rhs == lhs);
  }

  class CallbackProvider {
  public:
    MOCK_METHOD0(simple, void());
    MOCK_METHOD3(threeStrings, void(const std::string &,
                                    const std::string &,
                                    const std::string &));
    MOCK_METHOD2(diffFirst, void(const std::string &, int));
    MOCK_METHOD3(diffSecond, void(int, int, const std::string &));
    MOCK_METHOD1(diffThird, void(unsigned));
    MOCK_METHOD1(complexProcessor, void(ComplexObject));
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

TEST_F(InteractiveModeTest, DifferentArgsTest) {
  CallbackProvider mock;
  CallbackTy<void, std::string, int> first = [&mock](std::string a, int b) {
    return mock.diffFirst(a, b);
  };
  CallbackTy<void, int, int, std::string> second = [&mock](int a, int b, std::string c) {
    return mock.diffSecond(a, b, c);
  };
  CallbackTy<void, unsigned> third = [&mock](int a) {
    return mock.diffThird(a);
  };
  tested.registerCallback("first", first);
  tested.registerCallback("second", second);
  tested.registerCallback("third", third);

  EXPECT_CALL(*InputReaderMock::mock, getCommand(""))
    .WillOnce(Return("first"))
    .WillOnce(Return("second"))
    .WillOnce(Return("second"))
    .WillOnce(Return("third"))
    .WillOnce(Return("first"))
    .WillOnce(Return("third"))
    .WillOnce(Return("exit"));
  EXPECT_CALL(*InputReaderMock::mock, getArguments(""))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"No", "1"}))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"2", "3", "Yes"}))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"42", "15", "Linux"}))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"72"}))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"Hello", "9000"}))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"1841"}));
  EXPECT_CALL(mock, diffFirst("No", 1));
  EXPECT_CALL(mock, diffFirst("Hello", 9000));
  EXPECT_CALL(mock, diffSecond(2, 3, "Yes"));
  EXPECT_CALL(mock, diffSecond(42, 15, "Linux"));
  EXPECT_CALL(mock, diffThird(72));
  EXPECT_CALL(mock, diffThird(1841));

  tested.run();
}

TEST_F(InteractiveModeTest, WrongNumberOfArgumentsTest) {
  CallbackProvider mock;
  CallbackTy<void, int> action = [&mock](int a) {
    return mock.diffThird(a);
  };
  tested.registerCallback("do", action);

  EXPECT_CALL(*InputReaderMock::mock, getCommand(""))
    .WillOnce(Return("do"))
    .WillOnce(Return("do"))
    .WillOnce(Return("do"))
    .WillOnce(Return("exit"));
  EXPECT_CALL(*InputReaderMock::mock, getArguments(""))
    .WillOnce(Return<InputReader::CommandArgsContainer>({}))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"No"}))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"2", "3"}));

  EXPECT_CALL(mock, diffThird(_)).Times(0);

  try {
    tested.run();
  } catch (WrongNumberOfArgumentsException &e) {
    ASSERT_EQ(e.getExpectedNumber(), 1);
    ASSERT_EQ(e.getProvidedNumber(), 0);
  }

  try {
    tested.run();
  } catch (ParseException &e) {
    ASSERT_EQ(e.getPosition(), 0);
  }

  try {
    tested.run();
  } catch (WrongNumberOfArgumentsException &e) {
    ASSERT_EQ(e.getExpectedNumber(), 1);
    ASSERT_EQ(e.getProvidedNumber(), 2);
  }

  tested.run();
}

TEST_F(InteractiveModeTest, WrongCommandTest) {
  CallbackProvider mock;
  EXPECT_CALL(*InputReaderMock::mock, getCommand(""))
    .WillOnce(Return("make-magic"));
  EXPECT_CALL(*InputReaderMock::mock, getArguments(""))
    .WillOnce(Return<InputReader::CommandArgsContainer>({}));

  try {
    tested.run();
  } catch (WrongCommandException &e) {
    ASSERT_EQ(e.getCommand(), "make-magic");
  }
}

namespace {
  template <>
  ComplexObject InputReaderMock::parse(const std::string &arg) {
    int returnIndex = -1;
    if (arg == "emacs") returnIndex = 42;
    if (arg == "vim") returnIndex = 15;
    if (arg == "nano") returnIndex = 4;
    return {returnIndex, arg};
  }
}

TEST_F(InteractiveModeTest, CustomParserTest) {
  CallbackProvider mock;
  CallbackTy<void, ComplexObject> action = [&mock](ComplexObject obj) {
    return mock.complexProcessor(obj);
  };

  tested.registerCallback("do-complex", action);

  EXPECT_CALL(*InputReaderMock::mock, getCommand(""))
    .WillOnce(Return("do-complex"))
    .WillOnce(Return("do-complex"))
    .WillOnce(Return("do-complex"))
    .WillOnce(Return("exit"));
  EXPECT_CALL(*InputReaderMock::mock, getArguments(""))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"emacs"}))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"vim"}))
    .WillOnce(Return<InputReader::CommandArgsContainer>({"nano"}));

  EXPECT_CALL(mock, complexProcessor(ComplexObject{42, "emacs"}));
  EXPECT_CALL(mock, complexProcessor(ComplexObject{15, "vim"}));
  EXPECT_CALL(mock, complexProcessor(ComplexObject{4, "nano"}));

  tested.run();
}
