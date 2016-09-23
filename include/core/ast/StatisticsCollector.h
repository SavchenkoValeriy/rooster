#ifndef LLVM_STATISTICSCOLLECTOR_H
#define LLVM_STATISTICSCOLLECTOR_H

#include <clang/Tooling/CompilationDatabase.h>

template <class Action>
class StatisticsCollector {
private:
  std::unique_ptr<Action> ActionObject;
public:
  template <typename... Args>
  void init(Args&&... args) {
    ActionObject.reset(new Action(std::forward<Args>(args)...));
//    ActionObject = llvm::make_unique<Action>(std::forward<Args>(args)...);
  }
  void collect(clang::tooling::CompilationDatabase &database,
               std::vector<std::string> sources);
};


#endif //LLVM_STATISTICSCOLLECTOR_H
