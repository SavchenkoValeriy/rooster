#ifndef ROOSTER_STRINGEXTRAS_H
#define ROOSTER_STRINGEXTRAS_H

#include <string>

struct WrappedString {
  const std::string &OriginalString;
  const char *OpeningWrap,
             *ClosingWrap;
};

WrappedString wrap(const std::string &value,
                   const char *OpeningWrap = "\"",
                   const char *ClosingWrap = nullptr) {
  return WrappedString{value, OpeningWrap, ClosingWrap ? ClosingWrap : OpeningWrap};
}

template <class OStreamTy>
OStreamTy &operator<<(OStreamTy &OS, const WrappedString &value) {
  return OS << value.OpeningWrap << value.OriginalString << value.ClosingWrap;
}

#endif /* ROOSTER_STRINGEXTRAS_H */
