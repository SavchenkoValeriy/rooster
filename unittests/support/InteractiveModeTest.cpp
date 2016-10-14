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
    static CommandArgsContainer getArguments(const std::string &);
    static std::string getCommand(const std::string &);
  private:
    static std::unique_ptr<InputReaderMockObject> mock;
  };

  class InputProviderMock : public InputProvider {
  public:
    static void init() { mock = std::make_unique<InputProviderMockObject>(); }
    std::string read() { return mock->read(); }
  private:
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
}
