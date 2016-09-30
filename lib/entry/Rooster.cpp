#include <support/CommandLineOptions.h>
#include <completion/CompletionTool.h>

using namespace llvm;

static cl::OptionCategory RoosterCompletionCategory("Rooster completion tool options");

static cl::opt<bool> interactive("i", cl::desc("Run in interactive mode"),
                                 cl::cat(RoosterCompletionCategory));
static cl::opt<std::string> fileToComplete("f", cl::desc("File to run completion for"),
                                         cl::cat(RoosterCompletionCategory));
static cl::opt<unsigned> line("l", cl::desc("Line of completion"),
                              cl::cat(RoosterCompletionCategory));
static cl::opt<unsigned> column("c", cl::desc("Column of completion"),
                                cl::cat(RoosterCompletionCategory));

static cl::extrahelp MoreHelp("\nMore help text...\n");

int main(int argc, const char **argv) {
  CommandLineOptions OptionsParser(argc, argv, RoosterCompletionCategory);
  CompletionTool Rooster(std::move(OptionsParser.getCompilations()),
                         OptionsParser.getSourcePathList());
  Rooster.setPrintDiagnostics(OptionsParser.isDiagnosticOn());
  Rooster.completeAt(fileToComplete, line, column);
  return 0;
}
