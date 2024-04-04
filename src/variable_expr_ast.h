#pragma once

#include <string>
#include "base_expr_ast.h"

class VariableExprAST : public ExprAST {
	std::string Name;

public:
	VariableExprAST(const std::string& Name);
	llvm::Value* codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) override;
};