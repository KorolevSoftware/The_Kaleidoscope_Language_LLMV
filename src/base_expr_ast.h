#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "enviroment.h"

class ExprAST {
public:
	virtual ~ExprAST() = default;
	virtual llvm::Value* codegen(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& llvm_module, Enviroment& variables) = 0;
};