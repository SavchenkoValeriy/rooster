#include <support/CommandLineOptions.h>
#include <support/InteractiveMode.h>
#include <support/JSONInputReader.h>
#include <support/ConsoleInputProvider.h>
#include <completion/CompletionTool.h>
#include <llvm/Support/CommandLine.h>

using namespace llvm;

static cl::OptionCategory RoosterCompletionCategory("Rooster completion tool options");

static cl::opt<bool> isInteractive("i", cl::desc("Run in interactive mode"),
                                 cl::cat(RoosterCompletionCategory));
static cl::opt<std::string> fileToComplete("f", cl::desc("File to run completion for"),
                                         cl::cat(RoosterCompletionCategory));
static cl::opt<unsigned> line("l", cl::desc("Line of completion"),
                              cl::cat(RoosterCompletionCategory));
static cl::opt<unsigned> column("c", cl::desc("Column of completion"),
                                cl::cat(RoosterCompletionCategory));

static cl::extrahelp MoreHelp("\nMore help text...\n");

using namespace interactive;

struct RoosterServerCommands {
  enum class Command : char {
    complete,
    exit
  };

  Command value;
};

using InteractiveServer = InteractiveMode<JSONInputReader,
                                          ConsoleInputProvider>;

int main(int argc, const char **argv) {
  CommandLineOptions OptionsParser(argc, argv, RoosterCompletionCategory);
  CompletionTool Rooster(std::move(OptionsParser.getCompilations()),
                         OptionsParser.getSourcePathList());
  Rooster.setPrintDiagnostics(OptionsParser.isDiagnosticOn());
  if (isInteractive) {
    InteractiveServer server;
    CallbackTy<void, std::string, unsigned, unsigned> completionCallback;
    completionCallback =
      [&Rooster](std::string file, unsigned line, unsigned column) {
      return Rooster.completeAt(file, line, column);
    };
    server.registerCallback("complete",
                            completionCallback);
    server.run();
  } else {
    Rooster.completeAt(fileToComplete, line, column);
  }
  return 0;
}
