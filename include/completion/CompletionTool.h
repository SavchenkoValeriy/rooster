#ifndef LLVM_COMPLETIONTOOL_H
#define LLVM_COMPLETIONTOOL_H

#include <clang/Tooling/CompilationDatabase.h>

class ASTCollector;

class CompletionTool {
public:
  void init(clang::tooling::CompilationDatabase &database,
            std::vector<std::string> sources);
  void completeAt(const std::string &file, unsigned line, unsigned column);
  CompletionTool();
  CompletionTool(const CompletionTool&) = delete;
  CompletionTool(const CompletionTool&&) = delete;
  ~CompletionTool();
private:
  std::unique_ptr<ASTCollector> collector;
};

#endif /* LLVM_COMPLETIONTOOL_H */
