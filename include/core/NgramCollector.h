#include <iostream>
#include <vector>
#include <llvm/ADT/DenseSet.h>
#include <llvm/Support/raw_ostream.h>

template <class Ngram>
class NgramCollector;

template <class Ngram>
llvm::raw_ostream &operator << (llvm::raw_ostream &stream,
                           const NgramCollector<Ngram> &collector) {
  using Stats = std::pair<Ngram, size_t>;
  std::vector<Stats> sorted(collector.ngrams.begin(), collector.ngrams.end());
  std::sort(sorted.begin(), sorted.end(), [](const Stats &a, const Stats &b) {
    return a.second > b.second;
  });
  stream << "Ngrams (with N = " << Ngram::getSize() << ")\n";
  for (auto unit: sorted) {
    stream << unit.first << " : " << unit.second << "\n";
  }
  return stream;
}

template <class Ngram>
class NgramCollector {
private:
  using StorageType = llvm::DenseMap<Ngram, size_t>;
  using ResultType = std::pair<typename StorageType::iterator, bool>;
  StorageType ngrams;
  Ngram *currentNgram = nullptr;
  static bool isNewlyAppended(const ResultType &result) {
    return result.second;
  }
  static void increaseCounter(const ResultType &result) {
    ++(result.first->second);
  }
  static Ngram *getPointerToAppended(const ResultType &result) {
    return &result.first->first;
  }
  void add(Ngram &&newNgram) {
    auto result = ngrams.insert(std::make_pair(newNgram, 1));
    if (!isNewlyAppended(result)) {
      increaseCounter(result);
    }
    currentNgram = getPointerToAppended(result);
  }
public:
  void process(const typename Ngram::UnitType &element) {
    add(Ngram(currentNgram, element));
  }
  void startNewFile() {
    currentNgram = nullptr;
  }
  friend llvm::raw_ostream &operator << <> (llvm::raw_ostream &stream,
                                       const NgramCollector<Ngram> &collector);
};

