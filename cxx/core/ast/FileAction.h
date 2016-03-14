#ifndef LLVM_FILEACTION_H
#define LLVM_FILEACTION_H

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendActions.h"

using namespace clang;

template <class Derived>
class FileAction : public ASTConsumer,
  public RecursiveASTVisitor<FileAction<Derived> > {
private:
  using base = RecursiveASTVisitor<FileAction<Derived> >;
  Derived &getAsDerived() { return *static_cast<Derived *>(this); }
protected:
  SourceManager *sourceManager;
public:
  void HandleTranslationUnit(ASTContext &Ctx) override;

#define DEFINE_FOR_ALL(CLASS)                                                  \
  bool Visit##CLASS(CLASS *S);
#include "ASTMacroHelpers.h"
};

#define DEFINE_FOR_ALL(CLASS)                                                  \
  template<class Derived>                                                      \
  bool FileAction<Derived>::Visit##CLASS(CLASS *S) {                           \
    getAsDerived().Visit(S);                                                     \
    return true;                                                               \
  }
#include "ASTMacroHelpers.h"

template<class Derived>
void FileAction<Derived>::HandleTranslationUnit(ASTContext &Ctx) {
  auto TranslationUnitDecl = Ctx.getTranslationUnitDecl();
  sourceManager = &Ctx.getSourceManager();
  base::TraverseDecl(TranslationUnitDecl);
}

#endif //LLVM_FILEACTION_H
