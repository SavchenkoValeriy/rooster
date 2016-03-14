#include "PlainPrinter.h"
#include "ASTExprTraits.h"

#include <clang/Lex/Lexer.h>


PlainPrinterAction::PlainPrinterAction(const llvm::StringRef &filename) :
  ASTAction<PlainPrinterAction>(),
  OutputFile(std::make_shared<std::fstream>()) {
  OutputFile->open(filename, std::fstream::out);
}

PlainPrinterAction::~PlainPrinterAction() {
  OutputFile->close();
}

std::fstream &PlainPrinterAction::getStream() {
  return *OutputFile;
}

PlainPrinterAction::PlainPrinterAction(const PlainPrinterAction &action) :
  ASTAction<PlainPrinterAction>(), OutputFile(action.OutputFile) { }

template <>
void PlainPrinter::Visit(FunctionDecl *func) {
  parent.getStream() << ", $\n";
  VisitImpl(func);
}

template <>
void PlainPrinter::Visit(TranslationUnitDecl *d) { }

#define DEFINE_FOR_ALL(CLASS)                                                  \
template void PlainPrinter::VisitImpl<CLASS>(CLASS *);
#include "ASTMacroHelpers.h"

namespace {
  struct PrinterHelper {
    std::fstream &out;
    clang::SourceManager &manager;
  };

  inline std::string getSourceRepr(...) {
    return "";
  }

  template <class ExprT>
  inline auto getSourceRepr(ExprT *node, PrinterHelper helper) ->
  decltype((void)node->getSourceRange(), std::string()) {
    // (T, U) => "T,,"
    std::string text = clang::Lexer::getSourceText(
      CharSourceRange::getTokenRange(node->getSourceRange()), helper.manager,
                                     LangOptions(), 0);
    if (text.size() != 0 && text.at(text.size()-1) == ',')
      return clang::Lexer::getSourceText(
        CharSourceRange::getCharRange(node->getSourceRange()), helper.manager,
                                      LangOptions(), 0);
    return text;
  }

  template <class ExprT>
  inline constexpr auto getNodeName(ExprT *node) ->
  decltype(ASTExprTraits<ExprT>::name) {
    return ASTExprTraits<ExprT>::name;
  }

  template <class ExprT>
  inline void defaultVisit(ExprT *node, PrinterHelper helper) {
    helper.out << "(" << getNodeName(node) << "@@ " <<
      getSourceRepr(node, helper) << "), ";
  }
}

template <class ExprT>
void PlainPrinter::VisitImpl(ExprT *node) {
  defaultVisit(node, {parent.getStream(), *sourceManager});
}
