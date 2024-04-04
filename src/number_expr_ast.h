#pragma once

#include "base_expr_ast.h"

class NumberExprAST : public ExprAST {
	double value;

public:
	NumberExprAST(double value);

	llvm::Value* codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) override;
};