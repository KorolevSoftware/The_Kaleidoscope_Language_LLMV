#pragma once
#include "base_expr_ast.h"

#include "tokenizer.h"
class BinaryExprAST : public ExprAST {
	Tokenizer::TokenType op_type;
	std::unique_ptr<ExprAST> LHS, RHS;

public:
	BinaryExprAST(Tokenizer::TokenType op_type, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS);
	llvm::Value* codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) override;
};