#include <clang/Tooling/Tooling.h>
#include "StatisticsCollector.h"
#include "PlainPrinter.h"

using namespace clang::tooling;

template <class Action>
void StatisticsCollector<Action>::collect(CompilationDatabase &database,
                                          std::vector<std::string> sources) {
  ClangTool Tool(database, {sources[0]});
  Tool.run(ActionObject.get());
}

template class StatisticsCollector<PlainPrinterAction>;
