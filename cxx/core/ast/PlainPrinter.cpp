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
    const clang::SourceManager &manager;
    const clang::PrintingPolicy &policy;
  };

  template <class ExprT>
  inline constexpr auto getNodeName(ExprT *node) ->
  decltype(ASTExprTraits<ExprT>::name) {
    return ASTExprTraits<ExprT>::name;
  }

  template <class ExprT>
  inline std::string getSourceRepr(ExprT *node, ...) {
    llvm::errs() << "Not a single category: " << getNodeName(node) << "\n";
    return "";
  }

  template <class ExprT>
  inline auto getSourceRepr(ExprT *node, PrinterHelper helper) ->
  decltype((void)node->desugar(), std::string()) {
    llvm::errs() << "desugar category: " << getNodeName(node) << "\n";
    QualType desugaredType = node->desugar();
    return desugaredType.getAsString(helper.policy);
  }

  template <class ExprT>
  inline CharSourceRange getDefaultSourceRange(ExprT *node) {
    return CharSourceRange::getTokenRange(node->getSourceRange());
  }

  template <bool hasGetBody, bool hasGetChildren, class ExprT>
  struct getSourceRange {
    static inline CharSourceRange eval(ExprT *node, PrinterHelper helper) {
      return CharSourceRange::getTokenRange(node->getSourceRange());
    }
  };

  template <class ExprT>
  struct getSourceRange<true, true, ExprT> {
    static inline CharSourceRange eval(ExprT *node, PrinterHelper helper) {
      CharSourceRange range = getDefaultSourceRange(node);
      range = clang::Lexer::makeFileCharRange(range, helper.manager, LangOptions());
      SourceLocation nodeBegin = range.getBegin(),
        childBegin = range.getEnd() , current;
      llvm::errs() << "start: " << nodeBegin.printToString(helper.manager)
      << "; end: " << childBegin.printToString(helper.manager) << "\n";

      auto body = node->getBody();
      if (body) {
        current = getDefaultSourceRange(body).getBegin();
        llvm::errs() << "body begin: " <<
        current.printToString(helper.manager) << "\n";
        if (current < childBegin) {
          childBegin = current;
        }
      }
      return CharSourceRange::getCharRange(nodeBegin, childBegin);
    }
  };

  template <class ExprT>
  struct getSourceRange<true, false, ExprT> {
    static inline CharSourceRange eval(ExprT *node, PrinterHelper helper) {
      return getSourceRange<true, true, ExprT>::eval(node, helper);
    }
  };

  template <class ExprT>
  struct getSourceRange<false, true, ExprT> {
    static inline CharSourceRange eval(ExprT *node, PrinterHelper helper) {
      CharSourceRange range = getDefaultSourceRange(node);
      range = clang::Lexer::makeFileCharRange(range, helper.manager, LangOptions());
      SourceLocation nodeBegin = range.getBegin(),
        childBegin = range.getEnd() , current;
      llvm::errs() << "start: " << nodeBegin.printToString(helper.manager)
      << "; end: " << childBegin.printToString(helper.manager) << "\n";
      for (auto child: node->children()) {
        current = getDefaultSourceRange(child).getBegin();
        llvm::errs() << "child begin: " << current.printToString(helper.manager) << "\n";
        if (current < childBegin) {
          childBegin = current;
        }
      }
      return CharSourceRange::getCharRange(nodeBegin, childBegin);
    }
  };

  template <class ExprT>
  inline auto getSourceRepr(ExprT *node, PrinterHelper helper) ->
  decltype((void)node->getSourceRange(), std::string()) {
    constexpr bool hasGetBody = hasGetBodyMethod(node),
               hasGetChildren = hasGetChildrenMethod(node);
    llvm::errs() << "getSourceRange category: " << getNodeName(node) << "\n";
    llvm::errs() << "getBody: " << hasGetBody << "; getChildren: " << hasGetChildren << "\n";
    std::string text = clang::Lexer::getSourceText(
      getSourceRange<hasGetBody, hasGetChildren, ExprT>::eval(node, helper),
      helper.manager, LangOptions(), 0);
    return text;
  }

  template <class ExprT>
  inline void defaultVisit(ExprT *node, PrinterHelper helper) {
    helper.out << "(" << getNodeName(node) << "@@ " <<
      getSourceRepr(node, helper) << "), ";
  }
}

template <class ExprT>
void PlainPrinter::VisitImpl(ExprT *node) {
  defaultVisit(node, {parent.getStream(),
                      context->getSourceManager(),
                      context->getPrintingPolicy()});
}
