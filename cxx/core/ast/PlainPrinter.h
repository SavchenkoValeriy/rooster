#ifndef LLVM_PLAINPRINTER_H
#define LLVM_PLAINPRINTER_H

#include <fstream>
#include "FileAction.h"
#include "ASTAction.h"
#include "ASTExprTraits.h"

class PlainPrinterAction;

class PlainPrinter : public FileAction<PlainPrinter> {
public:
  using Parent = PlainPrinterAction;
  PlainPrinter(Parent &parent) : FileAction<PlainPrinter>(), parent(parent) { }
  template <class ExprT>
  void Visit(ExprT *node);
private:
  Parent &parent;
  template <class ExprT>
  void DefaultVisit(ExprT *node);
};

class PlainPrinterAction : public ASTAction<PlainPrinterAction> {
private:
  std::shared_ptr<std::fstream> OutputFile;
public:
  using Child = PlainPrinter;
  PlainPrinterAction(const llvm::StringRef &filename);
  PlainPrinterAction(const PlainPrinterAction&);
  std::fstream &getStream();
  ~PlainPrinterAction();
};

template <class ExprT>
void PlainPrinter::DefaultVisit(ExprT *node) {
  parent.getStream() << ASTExprTraits<ExprT>::name << ", ";
}

template <class ExprT>
void PlainPrinter::Visit(ExprT *node) {
  DefaultVisit(node);
}

#endif //LLVM_PLAINPRINTER_H
