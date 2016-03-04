#include "PlainPrinter.h"

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
  DefaultVisit(func);
}