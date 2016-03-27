#include "PlainPrinter.h"
#include "ASTExprTraits.h"

#include <clang/Lex/Lexer.h>
#include <llvm/Support/Debug.h>

#include <boost/hana/fwd/eval_if.hpp>
#include <boost/hana/optional.hpp>

#define DEBUG_TYPE "plain-printer"

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

namespace { // start of anonymous namespace
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
    DEBUG(llvm::errs() << "Not a single category: " << getNodeName(node) << "\n");
    return "";
  }

  template <class ExprT>
  inline auto getSourceRepr(ExprT *node, PrinterHelper helper) ->
  decltype((void)node->desugar(), std::string()) {
    DEBUG(llvm::errs() << "desugar category: " << getNodeName(node) << "\n");

    QualType desugaredType = node->desugar();
    return desugaredType.getAsString(helper.policy);
  }

  template <class ExprT>
  inline CharSourceRange getDefaultSourceRange(ExprT *node) {
    return CharSourceRange::getTokenRange(node->getSourceRange());
  }

  template <class ExprT>
  inline CharSourceRange getSourceRangeWithoutBody(ExprT *node, PrinterHelper helper) {
    DEBUG(llvm::errs() << "Has getBody: " << getNodeName(node) << "\n");

    CharSourceRange range = getDefaultSourceRange(node);
    range = clang::Lexer::makeFileCharRange(range, helper.manager, LangOptions());
    SourceLocation nodeBegin = range.getBegin(),
      childBegin = range.getEnd() ,
      current;

    DEBUG(llvm::errs() << "start: " << nodeBegin.printToString(helper.manager)
          << "; end: " << childBegin.printToString(helper.manager) << "\n");

    auto body = node->getBody();
    if (body) {
      current = getDefaultSourceRange(body).getBegin();

      DEBUG(llvm::errs() << "body begin: " <<
            current.printToString(helper.manager) << "\n");

      if (current < childBegin) {
        childBegin = current;
      }
    } else {
      DEBUG(llvm::errs() << "Has no body: " << getNodeName(node) << "\n");

      auto getLocation = hana::sfinae(
        [](auto *obj) -> decltype(obj->getLocation()) {
          return obj->getLocation();
        });
      childBegin = getLocation(node).value_or(childBegin);
    }

    return CharSourceRange::getCharRange(nodeBegin, childBegin);
  }

  template <class ExprT>
  inline CharSourceRange getSourceRangeWithoutChildren(ExprT *node,
                                                       PrinterHelper helper) {
    DEBUG(llvm::errs() << "Has children: " << getNodeName(node) << "\n");

    CharSourceRange range = getDefaultSourceRange(node);
    range = clang::Lexer::makeFileCharRange(range, helper.manager, LangOptions());
    SourceLocation nodeBegin = range.getBegin(),
      childBegin = range.getEnd() , current;

    DEBUG(llvm::errs() << "start: " << nodeBegin.printToString(helper.manager)
          << "; end: " << childBegin.printToString(helper.manager) << "\n");

    for (auto child: node->children()) {
      current = getDefaultSourceRange(child).getBegin();
      DEBUG(llvm::errs() << "child begin: " << current.printToString(helper.manager) << "\n");
      if (current < childBegin) {
        childBegin = current;
      }
    }

    return CharSourceRange::getCharRange(nodeBegin, childBegin);
  }

  auto getSourceRangeIfHasGetBodyMethod =
    hana::sfinae([](auto *p, PrinterHelper helper) ->
    decltype((decltype(getSourceRangeWithoutBody(p, helper)))p->getBody()) {
      return getSourceRangeWithoutBody(p, helper);
    });

  auto getSourceRangeIfHasChildrenMethod =
    hana::sfinae([](auto *p, PrinterHelper helper) ->
    decltype((decltype(getSourceRangeWithoutChildren(p, helper)))p->children()) {
      return getSourceRangeWithoutChildren(p, helper);
    });


  template <class ExprT>
  inline CharSourceRange getSourceRange(ExprT *node, PrinterHelper helper) {
    return getGenericValue(hana::make_tuple(node, helper),
                           getDefaultSourceRange(node),
                           getSourceRangeIfHasGetBodyMethod,
                           getSourceRangeIfHasChildrenMethod);
  }

  template <class ExprT>
  inline auto getSourceRepr(ExprT *node, PrinterHelper helper) ->
  decltype((void)node->getSourceRange(), std::string()) {
    constexpr bool hasGetBody = hasGetBodyMethod(node),
               hasGetChildren = hasGetChildrenMethod(node);
    DEBUG(llvm::errs() << "getSourceRange category: " << getNodeName(node) << "\n");
    DEBUG(llvm::errs() << "getBody: " << hasGetBody << "; getChildren: " << hasGetChildren << "\n");
    std::string text = clang::Lexer::getSourceText(
      getSourceRange(node, helper),
      helper.manager, LangOptions(), 0);
    return text;
  }

  template <class ExprT>
  inline void defaultVisit(ExprT *node, PrinterHelper helper) {
    helper.out << "(" << getNodeName(node) << "@@ " <<
      getSourceRepr(node, helper) << "), ";
  }
} // end of anonymous namespace

template <class ExprT>
void PlainPrinter::VisitImpl(ExprT *node) {
  defaultVisit(node, {parent.getStream(),
                      context->getSourceManager(),
                      context->getPrintingPolicy()});
}
