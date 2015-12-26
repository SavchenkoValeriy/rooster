#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/iterator_range.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/Hashing.h>

template <class UnitTy, unsigned N = 3>
class Ngram {
private:
  using StorageType = llvm::SmallVector<UnitTy, N>;
  StorageType elements;
  using MyType = Ngram<UnitTy, N>;
  void initFromPrevious(MyType *previous) {
    if (previous) {
      elements = StorageType(std::next(previous->begin()),
                             previous->end());
    } else {
      elements = StorageType(N-1);
    }
  }
public:
  using UnitType = UnitTy;
  using iterator = typename StorageType::iterator;
  using const_iterator = typename StorageType::const_iterator;
  Ngram(MyType *previous, const UnitType &currentUnit) {
    initFromPrevious(previous);
    elements.push_back(currentUnit);
  }
  Ngram(MyType *previous, const UnitType &&currentUnit) {
    initFromPrevious(previous);
    elements.emplace_back(currentUnit);
  }
  Ngram() {
    elements = StorageType(N);
  }
  iterator begin() { return elements.begin(); }
  iterator end() { return elements.end(); }
  const_iterator begin() const { return elements.begin(); }
  const_iterator end() const { return elements.end(); }
  static constexpr unsigned getSize() { return N; }
  friend struct llvm::DenseMapInfo<MyType>;
};

template <class UnitTy>
class Ngram <UnitTy, 0>;

template <class UnitTy, unsigned N>
llvm::raw_ostream &operator << (llvm::raw_ostream &stream,
                           const Ngram<UnitTy, N> &printed) {
  stream << "(" << llvm::join(printed.begin(), printed.end(), ", ") << ")";
  return stream;
}

template <class UnitTy, unsigned N>
llvm::raw_ostream &operator << (llvm::raw_ostream &stream,
                           const Ngram<UnitTy, N> *printed) {
  return stream << *printed;
}

namespace llvm {
  template <class UnitTy, unsigned N>
  struct DenseMapInfo<Ngram<UnitTy, N>> {
    using MyType = Ngram<UnitTy, N>;
    static inline MyType getEmptyKey() {
      return MyType(nullptr, DenseMapInfo<UnitTy>::getEmptyKey());
    }
    static inline MyType getTombstoneKey() {
      return MyType(nullptr, DenseMapInfo<UnitTy>::getTombstoneKey());
    }
    static unsigned getHashValue(const MyType &Val) {
      return llvm::hash_combine_range(Val.begin(), Val.end());
    }
    static bool isEqual(const MyType &LHS, const MyType &RHS) {
      for (auto lit = LHS.begin(), rit = RHS.begin(),
          le = LHS.end(), re = RHS.end(); lit != le && rit != re; ++lit, ++rit) {
        if (*lit != *rit) return false;
      }
      return true;
    }
  };
}
