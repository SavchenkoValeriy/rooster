#include "CommandLineOptions.h"
#include <clang/Tooling/ArgumentsAdjusters.h>
#include <llvm/ADT/STLExtras.h>

using namespace clang::tooling;
using namespace llvm;

namespace {
  class ArgumentsAdjustingCompilations : public CompilationDatabase {
  public:
    ArgumentsAdjustingCompilations(
      std::unique_ptr<CompilationDatabase> Compilations)
      : Compilations(std::move(Compilations)) {}

    void appendArgumentsAdjuster(ArgumentsAdjuster Adjuster) {
      Adjusters.push_back(Adjuster);
    }

    std::vector<CompileCommand>
    getCompileCommands(StringRef FilePath) const override {
      return adjustCommands(Compilations->getCompileCommands(FilePath));
    }

    std::vector<std::string> getAllFiles() const override {
      return Compilations->getAllFiles();
    }

    std::vector<CompileCommand> getAllCompileCommands() const override {
      return adjustCommands(Compilations->getAllCompileCommands());
    }

  private:
    std::unique_ptr<CompilationDatabase> Compilations;
    std::vector<ArgumentsAdjuster> Adjusters;

    std::vector<CompileCommand>
    adjustCommands(std::vector<CompileCommand> Commands) const {
      for (CompileCommand &Command : Commands)
        for (const auto &Adjuster : Adjusters)
          Command.CommandLine = Adjuster(Command.CommandLine, Command.Filename);
      return Commands;
    }
  };
}

CommandLineOptions::CommandLineOptions(int &argc, const char **argv,
                                       cl::OptionCategory &Category) {
  static cl::opt<bool> Help("h", cl::desc("Alias for -help"), cl::Hidden);

  static cl::opt<std::string> BuildPath(cl::Positional, cl::desc("Build path"),
                                        cl::Required, cl::cat(Category));

  static cl::list<std::string> ArgsAfter(
    "extra-arg",
    cl::desc("Additional argument to append to the compiler command line"),
    cl::cat(Category));

  static cl::list<std::string> ArgsBefore(
    "extra-arg-before",
    cl::desc("Additional argument to prepend to the compiler command line"),
    cl::cat(Category));

  static cl::opt<bool> Diagnostics("d", cl::desc("Turn on compiler errors and warnings"),
                                   cl::cat(Category));

  static cl::opt<unsigned> N("n", cl::desc("Ngram size (0,10]"),
                             cl::init(3), cl::cat(Category));

  cl::HideUnrelatedOptions(Category);

  Compilations.reset(FixedCompilationDatabase::loadFromCommandLine(argc,
                                                                   argv));
  cl::ParseCommandLineOptions(argc, argv, nullptr);

  if (!Compilations) {
    std::string ErrorMessage;
    Compilations =
      CompilationDatabase::autoDetectFromDirectory(BuildPath, ErrorMessage);
    if (!Compilations)
      llvm::report_fatal_error(ErrorMessage);
  }
  auto AdjustingCompilations =
    llvm::make_unique<ArgumentsAdjustingCompilations>(
      std::move(Compilations));
  AdjustingCompilations->appendArgumentsAdjuster(
    getInsertArgumentAdjuster(ArgsBefore, ArgumentInsertPosition::BEGIN));
  AdjustingCompilations->appendArgumentsAdjuster(
    getInsertArgumentAdjuster(ArgsAfter, ArgumentInsertPosition::END));
  Compilations = std::move(AdjustingCompilations);

  DiagnosticOn = Diagnostics;
  NgramSize = N;
}
