#pragma once

#include <llvm/IR/Verifier.h>
#include "function_ast.h"

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body) : Proto(std::move(Proto)), Body(std::move(Body)) {}

llvm::Function* FunctionAST::codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) {
	// First, check for an existing function from a previous 'extern' declaration.
	llvm::Function* TheFunction = llvm_module.getFunction(Proto->getName());

	if (!TheFunction)
		TheFunction = Proto->codegen(context, builder, llvm_module, variables);

	if (!TheFunction)
		return nullptr;

	// Create a new basic block to start insertion into.
	llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", TheFunction);
	builder.SetInsertPoint(BB);

	// Record the function arguments in the NamedValues map.
	variables.local_variable.clear();

	for (auto& Arg : TheFunction->args())
		variables.local_variable[std::string(Arg.getName())] = &Arg;

	if (llvm::Value* RetVal = Body->codegen(context, builder, llvm_module, variables)) {
		// Finish off the function.
		builder.CreateRet(RetVal);

		// Validate the generated code, checking for consistency.
		verifyFunction(*TheFunction);

		return TheFunction;
	}

	// Error reading body, remove function.
	TheFunction->eraseFromParent();
	return nullptr;
}