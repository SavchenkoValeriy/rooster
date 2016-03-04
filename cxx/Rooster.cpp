#include <cxx/support/CommandLineOptions.h>
#include <fstream>
#include <cxx/core/ast/StatisticsCollector.h>
#include <cxx/core/ast/PlainPrinter.h>
#include "clang/AST/RecursiveASTVisitor.h"
// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
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

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

static std::fstream *OutputFile;

//template <typename ExprT>
//struct ASTExprTraits;
//
//NgramCollector<Ngram<StringRef, 3>> collector;
//
//enum class ExpressionsTypes {
//#define DEFINE_FOR_ALL(CLASS)                                                  \
//  ET_##CLASS,
//#include "ASTMacroHelpers.h"
//  ET_None
//};
//
//#define DEFINE_FOR_ALL(CLASS)                                                  \
//template<>                                                                     \
//struct ASTExprTraits<CLASS> {                                                  \
//  static constexpr auto name = #CLASS ;                                        \
//  static constexpr int value = static_cast<int>(ExpressionsTypes::ET_##CLASS); \
//};
//#include "ASTMacroHelpers.h"
//
//class ASTProcessor : public ASTConsumer,
//                     public RecursiveASTVisitor<ASTProcessor> {
//private:
//  using base = RecursiveASTVisitor<ASTProcessor>;
//public:
//  void HandleTranslationUnit(ASTContext &Ctx) override;
//
//  template <typename ExprT>
//  void Visit(ExprT *expr) {
//    *OutputFile << ASTExprTraits<ExprT>::name << ", ";
//  //  collector.process(ASTExprTraits<ExprT>::name);
//  }
//
//#define DEFINE_FOR_ALL(CLASS)                                                  \
//  bool Visit##CLASS(CLASS *S);
//#include "ASTMacroHelpers.h"
//};
//
////template <>
////void ASTProcessor::Visit(TranslationUnitDecl *expr) {
//  //collector.startNewFile();
//  //collector.process(ASTExprTraits<TranslationUnitDecl>::name);
////}
//
//#define DEFINE_FOR_ALL(CLASS)                                                  \
//  bool ASTProcessor::Visit##CLASS(CLASS *S) {                                  \
//    Visit(S);                                                                  \
//    return true;                                                               \
//  }
//#include "ASTMacroHelpers.h"
//
//void ASTProcessor::HandleTranslationUnit(ASTContext &Ctx) {
//  auto TranslationUnitDecl = Ctx.getTranslationUnitDecl();
//  TraverseDecl(TranslationUnitDecl);
//}
//
//class ASTProcessorAction : public ASTFrontendAction {
//public:
//  ASTProcessorAction(bool Diagnostics) : Diagnostics(Diagnostics) { }
//
//protected:
//  std::unique_ptr<ASTConsumer>
//  CreateASTConsumer(CompilerInstance &CI, StringRef InFile) override;
//
//private:
//  bool Diagnostics;
//};
//
//class RoosterActionFactory : public FrontendActionFactory {
//public:
//  RoosterActionFactory(CommandLineOptions &options) : options(options) { }
//  FrontendAction *create() override {
//    return new ASTProcessorAction(options.isDiagnosticOn());
//  }
//private:
//  CommandLineOptions &options;
//};
//
//std::unique_ptr<ASTConsumer>
//ASTProcessorAction::CreateASTConsumer(CompilerInstance &CI,
//                                      StringRef InFile) {
//  if (!Diagnostics) {
//    CI.getDiagnostics().setClient(new IgnoringDiagConsumer());
//  }
//  return llvm::make_unique<ASTProcessor>();
//}
//
int main(int argc, const char **argv) {
  CommandLineOptions OptionsParser(argc, argv, RoosterCategory);
  StatisticsCollector<PlainPrinterAction> RoosterCollector;
  RoosterCollector.init(OptionsParser.getOutput());
  RoosterCollector.collect(OptionsParser.getCompilations(),
                           OptionsParser.getSourcePathList());
//  std::fstream output;
//  output.open(OptionsParser.getOutput(), std::fstream::out);
//  OutputFile = &output;
//  ClangTool Tool(OptionsParser.getCompilations(),
//                 OptionsParser.getSourcePathList());
//  RoosterActionFactory Factory(OptionsParser);
//  int result = Tool.run(&Factory);
//  //llvm::outs() << collector;
//  output.close();
  return 0;
}
