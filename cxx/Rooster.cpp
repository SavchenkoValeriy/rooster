#include "clang/AST/RecursiveASTVisitor.h"
// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"

using namespace clang::tooling;
using namespace llvm;
using namespace clang;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

template <typename ExprT>
struct ASTExprTraits;


enum class ExpressionsTypes {
#define STMT(CLASS, PARENT)                                                    \
  ET_##CLASS,
#include "clang/AST/StmtNodes.inc"
#undef STMT
#define DECL(CLASS, BASE)                                                      \
  ET_##CLASS##Decl,
#include "clang/AST/DeclNodes.inc"
#undef DECL
  ET_None
};

#define STMT(CLASS, PARENT)                                                    \
template<>                                                                     \
struct ASTExprTraits<CLASS> {                                                  \
  static constexpr auto name = #CLASS ;                                        \
  enum {value = static_cast<int>(ExpressionsTypes::ET_##CLASS)};               \
};
#include "clang/AST/StmtNodes.inc"
#undef STMT
#define DECL(CLASS, BASE)                                                      \
template<>                                                                     \
struct ASTExprTraits<CLASS##Decl> {                                            \
  static constexpr auto name = #CLASS "Decl" ;                                 \
  enum {value = static_cast<int>(ExpressionsTypes::ET_##CLASS##Decl)};         \
};
#include "clang/AST/DeclNodes.inc"
#undef DECL

class ASTProcessor : public ASTConsumer,
                     public RecursiveASTVisitor<ASTProcessor> {
private:
  using base = RecursiveASTVisitor<ASTProcessor>;
public:
  void HandleTranslationUnit(ASTContext &Ctx) override;

  template <typename ExprT>
  void Visit(ExprT *expr) {
    llvm::outs() << ASTExprTraits<ExprT>::name << "\n";
  }


  #define STMT(CLASS, PARENT)                                                  \
  bool Visit##CLASS(CLASS *S) {                                                \
    Visit(S);                                                                  \
    return true;                                                               \
  }
  #include "clang/AST/StmtNodes.inc"

  #define DECL(CLASS, BASE)                                                    \
  bool Visit##CLASS##Decl(CLASS##Decl *D) {                                    \
    Visit(D);                                                                  \
    return true;                                                               \
  }
  #include "clang/AST/DeclNodes.inc"
};

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
  return llvm::make_unique<ASTProcessor>();
}

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  return Tool.run(newFrontendActionFactory<ASTProcessorAction>().get());
}
