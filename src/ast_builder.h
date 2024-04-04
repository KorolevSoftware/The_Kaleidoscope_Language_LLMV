#pragma once
#include "base_expr_ast.h"
#include "tokenizer.h"
#include <vector>
#include "function_ast.h"
#include "KaleidoscopeJIT.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"

class AST {
public:
	AST();
	void build(Tokenizer& self);


private:
	std::unique_ptr<llvm::FunctionPassManager> TheFPM;
	std::unique_ptr<llvm::LoopAnalysisManager> TheLAM;
	std::unique_ptr<llvm::FunctionAnalysisManager> TheFAM;
	std::unique_ptr<llvm::CGSCCAnalysisManager> TheCGAM;
	std::unique_ptr<llvm::ModuleAnalysisManager> TheMAM;
	std::unique_ptr<llvm::PassInstrumentationCallbacks> ThePIC;
	std::unique_ptr<llvm::StandardInstrumentations> TheSI;

	std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit;
	std::unique_ptr<llvm::Module> llvm_module;
	std::unique_ptr<llvm::LLVMContext> context;
	std::unique_ptr<llvm::IRBuilder<>> builder;
	std::vector<std::unique_ptr<FunctionAST>> root;
};