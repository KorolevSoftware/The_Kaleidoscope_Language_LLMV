#pragma once

#include <string>
#include "prototype_ast.h"

PrototypeAST::PrototypeAST(const std::string& Name, std::vector<std::string> Args) : Name(Name), Args(std::move(Args)) {}

const std::string& PrototypeAST::getName() const { return Name; }

llvm::Function* PrototypeAST::codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) {
	std::vector<llvm::Type*> Doubles(Args.size(), llvm::Type::getDoubleTy(context));
	llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(context), Doubles, false);
	llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, llvm_module);

	// Set names for all arguments.
	unsigned Idx = 0;
	for (auto& Arg : F->args())
		Arg.setName(Args[Idx++]);

	return F;
}