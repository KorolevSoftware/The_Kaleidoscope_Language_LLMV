#pragma once

#include <string>
#include "number_expr_ast.h"

NumberExprAST::NumberExprAST(double value) : value(value) {}

llvm::Value* NumberExprAST::codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) {
	return llvm::ConstantFP::get(context, llvm::APFloat(value));
}