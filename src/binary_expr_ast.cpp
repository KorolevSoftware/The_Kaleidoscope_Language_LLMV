#include "binary_expr_ast.h"



BinaryExprAST::BinaryExprAST(Tokenizer::TokenType op_type, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
	: op_type(op_type), LHS(std::move(LHS)), RHS(std::move(RHS)) {}


llvm::Value* BinaryExprAST::codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) {
	llvm::Value* L = LHS->codegen(context, builder, llvm_module, variables);
	llvm::Value* R = RHS->codegen(context, builder, llvm_module, variables);
	if (!L || !R)
		return nullptr;

	switch (op_type) {
	case Tokenizer::TokenType::Plus:
		return builder.CreateFAdd(L, R, "addtmp");
	case Tokenizer::TokenType::Minus:
		return builder.CreateFSub(L, R, "subtmp");
	case Tokenizer::TokenType::Mult:
		return builder.CreateFMul(L, R, "multmp");
	case Tokenizer::TokenType::Less:
		L = builder.CreateFCmpULT(L, R, "cmptmp");
		// Convert bool 0/1 to double 0.0 or 1.0
		return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context), "booltmp");
	default:
		return nullptr;
	}
}