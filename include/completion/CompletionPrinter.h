#ifndef LLVM_COMPLETIONPRINTER_H
#define LLVM_COMPLETIONPRINTER_H

#include "clang/Lex/Preprocessor.h"
#include <clang/Sema/CodeCompleteConsumer.h>

/// \brief A code-completion consumer that maintains rooster logic
class CompletionPrinter : public clang::CodeCompleteConsumer {
  llvm::raw_ostream &OS;

  clang::CodeCompletionTUInfo CCTUInfo;

public:
  CompletionPrinter(const clang::CodeCompleteOptions &CodeCompleteOpts,
                    llvm::raw_ostream &OS)
    : CodeCompleteConsumer(CodeCompleteOpts, false), OS(OS),
      CCTUInfo(new clang::GlobalCodeCompletionAllocator) {}

  void ProcessCodeCompleteResults(clang::Sema &S, clang::CodeCompletionContext Context,
                                  clang::CodeCompletionResult *Results,
                                  unsigned NumResults) override;

  void ProcessOverloadCandidates(clang::Sema &S, unsigned CurrentArg,
                                 OverloadCandidate *Candidates,
                                 unsigned NumCandidates) override;

  bool isResultFilteredOut(llvm::StringRef Filter, clang::CodeCompletionResult Results) override;

  clang::CodeCompletionAllocator &getAllocator() override {
    return CCTUInfo.getAllocator();
  }

  clang::CodeCompletionTUInfo &getCodeCompletionTUInfo() override { return CCTUInfo; }
};

#endif /* LLVM_COMPLETIONPRINTER_H */
