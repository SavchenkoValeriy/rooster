//===- CommandLineOptions.h - options parser for Rooster -*- C++ -*-=====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef ROOSTER_COMMANDLINEOPTIONS_H
#define ROOSTER_COMMANDLINEOPTIONS_H

#include "clang/Tooling/CompilationDatabase.h"
#include "llvm/Support/CommandLine.h"

class CommandLineOptions {
public:
  CommandLineOptions(int &argc, const char **argv,
                     llvm::cl::OptionCategory &Category);
  clang::tooling::CompilationDatabase &getCompilations() const {
    return *Compilations;
  }
  std::vector<std::string> getSourcePathList() const {
    return Compilations->getAllFiles();
  }
  bool isDiagnosticOn() const {
    return DiagnosticOn;
  }

  unsigned getNgramSize() const {
    return NgramSize;
  }

  const llvm::StringRef &getOutput() const {
    return Output;
  }

private:
  std::unique_ptr<clang::tooling::CompilationDatabase> Compilations;
  bool DiagnosticOn;
  unsigned NgramSize;
  llvm::StringRef Output;
};

#endif // ROOSTER_COMMANDLINEOPTIONS_H