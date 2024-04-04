#pragma once
#include "variable_expr_ast.h"

VariableExprAST::VariableExprAST(const std::string& Name):Name(Name) {}

llvm::Value* VariableExprAST::codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) {
	llvm::Value* local_variable = variables.local_variable[Name];
	if (local_variable) {
		return local_variable;
	}

	llvm::Value* global_variable = variables.global_variable[Name];

	if (global_variable) {
		return global_variable;
	}

	return nullptr;
}