#ifndef LLVM_COMPLETIONTOOL_H
#define LLVM_COMPLETIONTOOL_H

#include <clang/Tooling/CompilationDatabase.h>

class ASTCollector;

class CompletionTool {
public:
  void init();
  void completeAt(const std::string &file, unsigned line, unsigned column);
  void setPrintDiagnostics(bool doPrint);
  bool getPrintDiagnostics();
  CompletionTool(clang::tooling::CompilationDatabase &&database,
                 std::vector<std::string> &&sources);
  CompletionTool(const CompletionTool&) = delete;
  CompletionTool(const CompletionTool&&) = delete;
  ~CompletionTool();
private:
  std::unique_ptr<ASTCollector> collector;
  clang::tooling::CompilationDatabase &database;
  std::vector<std::string> sources;
  bool printDiagnostics;
};

#endif /* LLVM_COMPLETIONTOOL_H */
