#ifndef LLVM_ASTACTION_H
#define LLVM_ASTACTION_H

#include "clang/Tooling/Tooling.h"

template <class Derive>
class ASTAction : public clang::ASTFrontendAction,
                  public clang::tooling::FrontendActionFactory {

public:
  virtual clang::FrontendAction *create() override {
    return new Derive(getDerive());
  }

  Derive &getDerive() {
    return *static_cast<Derive *>(this);
  }

private:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &CI, StringRef InFile) override {
    return llvm::make_unique<typename Derive::Child>(getDerive());
  };
};


#endif //LLVM_ASTACTION_H
