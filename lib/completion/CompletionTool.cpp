#include <completion/CompletionTool.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;
using namespace tooling;

class ASTCollector : public ToolAction {
public:
  bool runInvocation(CompilerInvocation *Invocation, FileManager *Files,
                     std::shared_ptr<PCHContainerOperations> PCHContainerOps,
                     DiagnosticConsumer *DiagConsumer) override {
    std::unique_ptr<ASTUnit> AST = ASTUnit::LoadFromCompilerInvocation(
        Invocation, std::move(PCHContainerOps),
        CompilerInstance::createDiagnostics(&Invocation->getDiagnosticOpts(),
                                            DiagConsumer,
                                            /*ShouldOwnClient=*/false),
        Files);
    if (!AST) return false;

    ASTs[AST->getOriginalSourceFileName()] = std::move(AST);
    return true;
  }
private:
  llvm::DenseMap<llvm::StringRef, std::unique_ptr<ASTUnit> > ASTs;
};

void CompletionTool::init(clang::tooling::CompilationDatabase &database,
                          std::vector<std::string> sources) {
  ClangTool Tool(database, sources);
  Tool.run(collector.get());
}
