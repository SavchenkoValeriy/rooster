#include <completion/CompletionPrinter.h>
#include <clang/Sema/Sema.h>

using namespace llvm;
using namespace clang;

template <unsigned Kind>
const StringRef getNameImpl(CodeCompletionResult Result);

template <>
const StringRef getNameImpl<CodeCompletionResult::RK_Declaration>(CodeCompletionResult Result) {
  auto declaration = Result.Declaration;
  if (!declaration->getIdentifier()) return "";
  return declaration->getIdentifier()->getName();
}

template <>
const StringRef getNameImpl<CodeCompletionResult::RK_Keyword>(CodeCompletionResult Result) {
  return Result.Keyword;
}

template <>
const StringRef getNameImpl<CodeCompletionResult::RK_Macro>(CodeCompletionResult Result) {
  return Result.Macro->getName();
}

template <>
const StringRef getNameImpl<CodeCompletionResult::RK_Pattern>(CodeCompletionResult Result) {
  return Result.Pattern->getAsString();
}

const StringRef getName(CodeCompletionResult Result) {
  switch (Result.Kind) {
  case CodeCompletionResult::RK_Declaration:
    return getNameImpl<CodeCompletionResult::RK_Declaration>(Result);
  case CodeCompletionResult::RK_Keyword:
    return getNameImpl<CodeCompletionResult::RK_Keyword>(Result);
  case CodeCompletionResult::RK_Macro:
    return getNameImpl<CodeCompletionResult::RK_Macro>(Result);
  case CodeCompletionResult::RK_Pattern:
    return getNameImpl<CodeCompletionResult::RK_Pattern>(Result);
  }
  llvm_unreachable("Unknown code completion result Kind.");
}

bool CompletionPrinter::isResultFilteredOut(StringRef Filter,
                                            CodeCompletionResult Result) {
  return getName(Result).startswith(Filter);
}

void
CompletionPrinter::ProcessCodeCompleteResults(Sema &SemaRef,
                                              CodeCompletionContext Context,
                                              CodeCompletionResult *Results,
                                              unsigned NumResults) {
  std::stable_sort(Results, Results + NumResults);

  StringRef Filter = SemaRef.getPreprocessor().getCodeCompletionFilter();

  // Print the results.
  for (unsigned I = 0; I != NumResults; ++I) {
    if(!Filter.empty() && isResultFilteredOut(Filter, Results[I]))
      continue;
    OS << "COMPLETION: ";
    switch (Results[I].Kind) {
    case CodeCompletionResult::RK_Declaration:
      OS << *Results[I].Declaration;
      if (Results[I].Hidden)
        OS << " (Hidden)";
      if (CodeCompletionString *CCS
            = Results[I].CreateCodeCompletionString(SemaRef, Context,
                                                    getAllocator(),
                                                    CCTUInfo,
                                                    includeBriefComments())) {
        OS << " : " << CCS->getAsString();
        if (const char *BriefComment = CCS->getBriefComment())
          OS << " : " << BriefComment;
      }

      OS << '\n';
      break;

    case CodeCompletionResult::RK_Keyword:
      OS << Results[I].Keyword << '\n';
      break;

    case CodeCompletionResult::RK_Macro: {
      OS << Results[I].Macro->getName();
      if (CodeCompletionString *CCS
            = Results[I].CreateCodeCompletionString(SemaRef, Context,
                                                    getAllocator(),
                                                    CCTUInfo,
                                                    includeBriefComments())) {
        OS << " : " << CCS->getAsString();
      }
      OS << '\n';
      break;
    }

    case CodeCompletionResult::RK_Pattern: {
      OS << "Pattern : "
         << Results[I].Pattern->getAsString() << '\n';
      break;
    }
    }
  }
}

static std::string getOverloadAsString(const CodeCompletionString &CCS) {
  std::string Result;
  llvm::raw_string_ostream OS(Result);

  for (auto &C : CCS) {
    switch (C.Kind) {
    case CodeCompletionString::CK_Informative:
    case CodeCompletionString::CK_ResultType:
      OS << "[#" << C.Text << "#]";
      break;

    case CodeCompletionString::CK_CurrentParameter:
      OS << "<#" << C.Text << "#>";
      break;

    default: OS << C.Text; break;
    }
  }
  return OS.str();
}


void
CompletionPrinter::ProcessOverloadCandidates(Sema &SemaRef,
                                             unsigned CurrentArg,
                                             OverloadCandidate *Candidates,
                                             unsigned NumCandidates) {
  for (unsigned I = 0; I != NumCandidates; ++I) {
    if (CodeCompletionString *CCS
          = Candidates[I].CreateSignatureString(CurrentArg, SemaRef,
                                                getAllocator(), CCTUInfo,
                                                includeBriefComments())) {
      OS << "OVERLOAD: " << getOverloadAsString(*CCS) << "\n";
    }
  }
}
