#ifndef LLVM_COMPLETIONTOOL_H
#define LLVM_COMPLETIONTOOL_H

#include <clang/Tooling/CompilationDatabase.h>

class ASTCollector;

class CompletionTool {
public:
  void init(clang::tooling::CompilationDatabase &database,
            std::vector<std::string> sources);
  void completeAt(std::string &file, unsigned line, unsigned column);
private:
  std::unique_ptr<ASTCollector> collector;
};

#endif /* LLVM_COMPLETIONTOOL_H */
