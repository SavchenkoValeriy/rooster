#include <support/InteractiveMode.h>
#include <support/InputReader.h>
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

  class InputReaderMock : public InputReader {
  public:
    static void init() { mock = std::make_unique<InputReaderMockObject>(); }
    static CommandArgsContainer getArguments(const std::string &);
    static std::string getCommand(const std::string &);
  private:
    static std::unique_ptr<InputReaderMockObject> mock;
  };

  InputReader::CommandArgsContainer InputReaderMock::getArguments(const std::string &input) {
    return mock->getArguments(input);
  }
  std::string InputReaderMock::getCommand(const std::string &input) {
    return mock->getCommand(input);
  }

  std::unique_ptr<InputReaderMockObject> InputReaderMock::mock;
}
