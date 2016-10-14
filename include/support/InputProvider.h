#ifndef ROOSTER_INPUTPROVIDER_H
#define ROOSTER_INPUTPROVIDER_H

#include <string>

namespace interactive {
  struct InputProvider {
    static std::string read();
  };
}

#endif /* ROOSTER_INPUTPROVIDER_H */
