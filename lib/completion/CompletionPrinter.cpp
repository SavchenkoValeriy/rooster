#include <completion/CompletionPrinter.h>
#include <support/StringExtras.h>
#include <boost/lexical_cast.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/algorithm/string/join.hpp>
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

bool isCompletionAvailable(CodeCompletionResult Result) {
  return Result.Availability == CXAvailability_Available ||
    Result.Availability == CXAvailability_Deprecated;
}

bool CompletionPrinter::isResultFilteredOut(StringRef Filter,
                                            CodeCompletionResult Result) {
  return !isCompletionAvailable(Result) && getName(Result).startswith(Filter);
}

class CompletionCandidate {
public:
  template <class T>
  using PlaceholdersTy = llvm::SmallVector<T, 4>;
  struct Postcompletion {
    std::string PlaceholdersText;
    PlaceholdersTy<unsigned> Positions;
  };
  void setTypedText(const char *value) { addStringValue(TypedText, value); }
  void setResultType(const char *value) { addStringValue(ResultType, value); }
  void setBriefComment(const char *value) { addStringValue(BriefComment, value); }
  void addArgumentToSignature(const char *value) {
    auto sizeBefore = PostcompletionInfo.PlaceholdersText.size();
    addToSignature(value);
    auto sizeAfter = PostcompletionInfo.PlaceholdersText.size();
    if (sizeAfter != sizeBefore)
      addPlaceholderRange(sizeBefore, sizeAfter);
  }
  void addTypedTextToSignature(const char *value) {
    addToSignature(value);
  }
  void addToPostcompletion(const char* value) {
    addStringValue(PostcompletionInfo.PlaceholdersText, value);
  }
  void setPriority(unsigned value) { Priority = value; }
  void addPlaceholderRange(unsigned begin, unsigned end) {
    PostcompletionInfo.Positions.push_back(begin);
    PostcompletionInfo.Positions.push_back(end);
  }

  const std::string &getTypedText() { return TypedText; }
  const std::string &getResultType() { return ResultType; }
  const std::string &getBriefComment() { return BriefComment; }
  const std::string &getSignature() { return Signature; }
  const Postcompletion &getPostcompletion() { return PostcompletionInfo; }
  unsigned getPriority() { return Priority; }
  unsigned getAnnotation() { return TypedText.size(); }
private:
  void addToSignature(const char *value) {
    addStringValue(Signature, value);
    if (TypedTextSet) addToPostcompletion(value);
    TypedTextSet = true;
  }
  void addStringValue(std::string &dst, const char *src) {
    if (src) dst += src;
  }

  bool TypedTextSet = false;
  std::string TypedText      = "",
              ResultType     = "",
              BriefComment   = "",
              Signature      = "";
  unsigned Priority = 0, Annotation = 0;
  Postcompletion PostcompletionInfo;
};

void print(llvm::raw_ostream &OS, const std::string &value) {
  OS << "\"" << value << "\"" << " ";
}

void print(llvm::raw_ostream &OS, unsigned value) {
  OS << value << " ";
}

namespace print {
  auto adapt(const std::string &value) {
    return wrap(value);
  }

  auto adapt(unsigned value) {
    return boost::lexical_cast<std::string>(value);
  }

  template <class Ty>
  auto adapt(const CompletionCandidate::PlaceholdersTy<Ty> &value) {
      CompletionCandidate::PlaceholdersTy<std::string> adaptedValue;
      boost::range::transform(value, std::back_inserter(adaptedValue),
                              [](auto x) { return adapt(x); });
      return boost::algorithm::join(adaptedValue, " ");
  }

  void print(llvm::raw_ostream &OS, const CompletionCandidate::Postcompletion &arg) {
    OS << "(" << adapt(arg.PlaceholdersText);
    auto positions = adapt(arg.Positions);
    OS << (!positions.empty() ? " " : "") << positions << ")";
  }

  template <class ProcessedArgTy, class ...ArgsTy>
  void print(llvm::raw_ostream &OS, ProcessedArgTy arg, ArgsTy... args) {
    OS << adapt(arg) << " ";
    print(OS, args...);
  }
}
llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, CompletionCandidate &candidate) {
  print::print(OS, candidate.getTypedText(),
               candidate.getPriority(),
               candidate.getResultType(),
               candidate.getBriefComment(),
               candidate.getSignature(),
               candidate.getAnnotation(),
               candidate.getPostcompletion());
  return OS;
}

void
CompletionPrinter::ProcessCodeCompleteResults(Sema &SemaRef,
                                              CodeCompletionContext Context,
                                              CodeCompletionResult *Results,
                                              unsigned NumResults) {
  std::stable_sort(Results, Results + NumResults);

  // Print the results.
  OS << "(\n";
  for (unsigned I = 0; I != NumResults; ++I) {
    OS << "(";
    CompletionCandidate candidate;
    switch (Results[I].Kind) {
    case CodeCompletionResult::RK_Declaration:
      if (Results[I].Hidden)
        OS << " (Hidden)";
      if (CodeCompletionString *CCS
            = Results[I].CreateCodeCompletionString(SemaRef, Context,
                                                    getAllocator(),
                                                    CCTUInfo,
                                                    includeBriefComments())) {
        candidate.setTypedText(CCS->getTypedText());
        candidate.setPriority(CCS->getPriority());
        candidate.setBriefComment(CCS->getBriefComment());
        for (auto chunk : *CCS) {
          switch (chunk.Kind) {
          case CodeCompletionString::CK_ResultType:
            candidate.setResultType(chunk.Text);
            break;
          case CodeCompletionString::CK_Optional:
            break;
          case CodeCompletionString::CK_Placeholder:
            candidate.addArgumentToSignature(chunk.Text);
            break;
          default:
            candidate.addTypedTextToSignature(chunk.Text);
          }
        }
      }
      OS << candidate;
      break;

    case CodeCompletionResult::RK_Keyword:
      OS << Results[I].Keyword;
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
      break;
    }

    case CodeCompletionResult::RK_Pattern: {
      OS << "Pattern : "
         << Results[I].Pattern->getAsString();
      break;
    }
    }
    OS << ")\n";
  }
  OS << ")\n";
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
