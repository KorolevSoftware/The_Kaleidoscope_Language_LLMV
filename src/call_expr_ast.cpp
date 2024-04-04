#pragma once

#include <string>
#include "call_expr_ast.h"


CallExprAST::CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args)
	: Callee(Callee), Args(std::move(Args)) {}

llvm::Value* CallExprAST::codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) {
	llvm::Function* CalleeF = llvm_module.getFunction(Callee);
	if (!CalleeF)
		return nullptr;
		//return LogErrorV("Unknown function referenced");

	// If argument mismatch error.
	if (CalleeF->arg_size() != Args.size())
		return nullptr;
		//return LogErrorV("Incorrect # arguments passed");

	std::vector<llvm::Value*> ArgsV;
	for (unsigned i = 0, e = Args.size(); i != e; ++i) {
		ArgsV.push_back(Args[i]->codegen(context, builder, llvm_module, variables));
		if (!ArgsV.back())
			return nullptr;
	}

	return builder.CreateCall(CalleeF, ArgsV, "calltmp");
}
