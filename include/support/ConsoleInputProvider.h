#ifndef ROOSTER_CONSOLEINPUTPROVIDER_H
#define ROOSTER_CONSOLEINPUTPROVIDER_H

#include <support/InputProvider.h>

namespace interactive {
  class ConsoleInputProvider : public InputProvider {
  public:
    static std::string read();
  };
}

#endif /* ROOSTER_CONSOLEINPUTPROVIDER_H */
