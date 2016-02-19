#include <cxx/support/CommandLineOptions.h>
#include "clang/AST/RecursiveASTVisitor.h"
// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include "core/Ngram.h"
#include "core/NgramCollector.h"

using namespace clang::tooling;
using namespace llvm;
using namespace clang;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory RoosterCategory("rooster options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

static cl::opt<bool> Diagnostics("d", cl::desc("Turn on compiler errors and warnings"),
                                 cl::cat(RoosterCategory));

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

template <typename ExprT>
struct ASTExprTraits;

NgramCollector<Ngram<StringRef, 3>> collector;

enum class ExpressionsTypes {
#define DEFINE_FOR_ALL(CLASS)                                                  \
  ET_##CLASS,
#include "ASTMacroHelpers.h"
  ET_None
};

#define DEFINE_FOR_ALL(CLASS)                                                  \
template<>                                                                     \
struct ASTExprTraits<CLASS> {                                                  \
  static constexpr auto name = #CLASS ;                                        \
  static constexpr int value = static_cast<int>(ExpressionsTypes::ET_##CLASS); \
};
#include "ASTMacroHelpers.h"

class ASTProcessor : public ASTConsumer,
                     public RecursiveASTVisitor<ASTProcessor> {
private:
  using base = RecursiveASTVisitor<ASTProcessor>;
public:
  void HandleTranslationUnit(ASTContext &Ctx) override;

  template <typename ExprT>
  void Visit(ExprT *expr) {
    collector.process(ASTExprTraits<ExprT>::name);
  }

#define DEFINE_FOR_ALL(CLASS)                                                  \
  bool Visit##CLASS(CLASS *S);
#include "ASTMacroHelpers.h"
};

template <>
void ASTProcessor::Visit(TranslationUnitDecl *expr) {
  collector.startNewFile();
  collector.process(ASTExprTraits<TranslationUnitDecl>::name);
}

#define DEFINE_FOR_ALL(CLASS)                                                  \
  bool ASTProcessor::Visit##CLASS(CLASS *S) {                                  \
    Visit(S);                                                                  \
    return true;                                                               \
  }
#include "ASTMacroHelpers.h"

void ASTProcessor::HandleTranslationUnit(ASTContext &Ctx) {
  auto TranslationUnitDecl = Ctx.getTranslationUnitDecl();
  TraverseDecl(TranslationUnitDecl);
}

class ASTProcessorAction : public ASTFrontendAction {
protected:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, StringRef InFile) override;
};

std::unique_ptr<ASTConsumer>
ASTProcessorAction::CreateASTConsumer(CompilerInstance &CI,
                                      StringRef InFile) {
  if (!Diagnostics) {
    CI.getDiagnostics().setClient(new IgnoringDiagConsumer());
  }
  return llvm::make_unique<ASTProcessor>();
}

int main(int argc, const char **argv) {
  CommandLineOptions OptionsParser(argc, argv, RoosterCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  int result = Tool.run(newFrontendActionFactory<ASTProcessorAction>().get());
  llvm::outs() << collector;
  return result;
}
