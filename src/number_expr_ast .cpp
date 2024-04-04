#pragma once


#include "base_expr_ast.h"

class NumberExprAST : public ExprAST {
	double Val;

public:
	NumberExprAST(double Val) : Val(Val) {}

	Value* codegen() override;
};