#include <support/CommandLineOptions.h>
#include <fstream>
#include <completion/CompletionTool.h>

using namespace clang::tooling;
using namespace llvm;
using namespace clang;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory RoosterCategory("rooster options");

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

static std::fstream *OutputFile;

int main(int argc, const char **argv) {
  CommandLineOptions OptionsParser(argc, argv, RoosterCategory);
  CompletionTool Rooster(std::move(OptionsParser.getCompilations()),
                         OptionsParser.getSourcePathList());
  Rooster.setPrintDiagnostics(OptionsParser.isDiagnosticOn());
  Rooster.completeAt("/space/vsavchenko/source/rooster/lib/entry/Rooster.cpp", 21, 40);
  Rooster.completeAt("/space/vsavchenko/source/rooster/lib/entry/Rooster.cpp", 21, 40);
  Rooster.completeAt("/space/vsavchenko/source/rooster/lib/entry/Rooster.cpp", 21, 40);
  Rooster.completeAt("/space/vsavchenko/source/rooster/lib/entry/Rooster.cpp", 21, 40);
  Rooster.completeAt("/space/vsavchenko/source/rooster/lib/entry/Rooster.cpp", 21, 40);
  return 0;
}
