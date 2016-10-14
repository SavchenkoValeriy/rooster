#include <support/InteractiveMode.h>
#include <gtest/gtest.h>

namespace interactive {
  class InputReaderMock : public InputReader {
    static CommandArgsContainer getArguments(const std::string &);
    static std::string getCommand(const std::string &);
  };
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
