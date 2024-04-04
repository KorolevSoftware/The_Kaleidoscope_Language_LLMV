#pragma once

#include <string>
#include "base_expr_ast.h"

class CallExprAST : public ExprAST {
	std::string Callee;
	std::vector<std::unique_ptr<ExprAST>> Args;

public:
	CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args);
	llvm::Value* codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) override;
};