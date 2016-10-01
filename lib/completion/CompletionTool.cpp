#include <completion/CompletionTool.h>
#include <completion/CompletionPrinter.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Sema/CodeCompleteConsumer.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;
using namespace tooling;

class ASTCollector : public ToolAction {
public:
  bool runInvocation(CompilerInvocation *Invocation, FileManager *Files,
                     std::shared_ptr<PCHContainerOperations> PCHContainerOps,
                     DiagnosticConsumer *DiagConsumer) override {
    auto diagnostics = CompilerInstance::createDiagnostics(
        &Invocation->getDiagnosticOpts(), DiagConsumer,
        /*ShouldOwnClient=*/false);
    if (!printDiagnostics)
      diagnostics->setClient(new IgnoringDiagConsumer());
    Invocation->getFrontendOpts().SkipFunctionBodies = 1;
    std::unique_ptr<ASTUnit> AST = ASTUnit::LoadFromCompilerInvocation(
        Invocation, std::move(PCHContainerOps),
        diagnostics,
        Files);
    if (!AST) return false;
    llvm::errs() << "Processed: " << AST->getOriginalSourceFileName() << "\n";

    ASTs[AST->getOriginalSourceFileName()] = std::move(AST);
    return true;
  }

  ASTUnit *getASTUnit(const llvm::StringRef &file) {
    return ASTs[file].get();
  }

  void setPrintDiagnostics(bool doPrint) {
    printDiagnostics = doPrint;
  }

  bool getPrintDiagnostics() {
    return printDiagnostics;
  }

  ASTCollector() : ASTs(), printDiagnostics(false) {}
private:
  llvm::DenseMap<llvm::StringRef, std::unique_ptr<ASTUnit> > ASTs;
  bool printDiagnostics;
};

void CompletionTool::completeAt(const std::string &file,
                                unsigned int line, unsigned int column) {
  ASTUnit *AST = collector->getASTUnit(file);
  if (!AST) {
    ClangTool Tool(database, {file});
    Tool.run(collector.get());
  }
  AST = collector->getASTUnit(file);
  CodeCompleteOptions options;
  options.IncludeBriefComments = 1;
  options.IncludeCodePatterns  = 1;
  options.IncludeGlobals       = 1;
  options.IncludeMacros        = 1;
  llvm::SmallVector<StoredDiagnostic, 0> StoredDiagnostics{};
  auto PCHContainerOps = std::make_shared<PCHContainerOperations>();
  llvm::SmallVector<const llvm::MemoryBuffer *, 0> OwnedBuffers{};
  CompletionPrinter completionPrinter(options, llvm::errs());
  AST->CodeComplete(file, line, column, {}, options.IncludeMacros, options.IncludeCodePatterns, options.IncludeBriefComments, completionPrinter, PCHContainerOps, AST->getDiagnostics(), const_cast<LangOptions&>(AST->getLangOpts()), AST->getSourceManager(), AST->getFileManager(), StoredDiagnostics, OwnedBuffers);
}

void CompletionTool::setPrintDiagnostics(bool doPrint) {
  collector->setPrintDiagnostics(doPrint);
}

bool CompletionTool::getPrintDiagnostics() {
  return collector->getPrintDiagnostics();
}

CompletionTool::CompletionTool(CompilationDatabase &&database,
                               std::vector<std::string> &&sources) :
  collector(llvm::make_unique<ASTCollector>()), database(database), sources(sources) { }
CompletionTool::~CompletionTool() = default;
