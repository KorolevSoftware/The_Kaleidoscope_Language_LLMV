#include <map>

#include "ast_builder.h"
#include "binary_expr_ast.h"
#include "function_ast.h"
#include "number_expr_ast.h"
#include "variable_expr_ast.h"
#include "call_expr_ast.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include <cstdio>

namespace {
	std::unique_ptr<ExprAST> ParseExpression(Tokenizer& tokenizer);
	static std::map<Tokenizer::TokenType, int> BinopPrecedence;

	static int GetTokPrecedence(Tokenizer::Token token) {
		// Make sure it's a declared binop.
		int TokPrec = BinopPrecedence[token.type];
		if (TokPrec <= 0)
			return -1;
		return TokPrec;
	}

	std::unique_ptr<ExprAST> ParseNumberExpr(Tokenizer& tokenizer) {
		double value = std::any_cast<double>(tokenizer.get_token().value);
		return std::make_unique<NumberExprAST>(value);
	}

	std::unique_ptr<ExprAST> ParseIdentifierExpr(Tokenizer& tokenizer) {
		auto token_name = tokenizer.get_token();
		auto token_value_or_bracket = tokenizer.get_token_preview();

		std::string name = std::any_cast<std::string>(token_name.value);


		if (token_value_or_bracket.type != Tokenizer::TokenType::RBOpen) // Simple variable ref.
			return std::make_unique<VariableExprAST>(name);

		// Call function
		std::vector<std::unique_ptr<ExprAST>> Args;
		auto token_variable = tokenizer.get_token();
		while (token_variable.type != Tokenizer::TokenType::RBClose) {
			if (auto Arg = ParseExpression(tokenizer))
				Args.push_back(std::move(Arg));
			else
				return nullptr;
			token_variable = tokenizer.get_token();
			//auto token_arg_delimer = tokenizer.get_token();

			//if (token_arg_delimer.type != Tokenizer::TokenType::Сomma) {
			//	return nullptr;
			//	//return LogError("Expected ')' or ',' in argument list");
			//}
		}

		return std::make_unique<CallExprAST>(name, std::move(Args));

	}

	std::unique_ptr<ExprAST> ParseParenExpr(Tokenizer& tokenizer) {
		auto V = ParseExpression(tokenizer);
		if (!V)
			return nullptr;
		auto token = tokenizer.get_token();
		if (token.type != Tokenizer::TokenType::RBClose)
			return nullptr;
			//return LogError("expected ')'");
		return V;
	}


	/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
	std::unique_ptr<ExprAST> ParsePrimary(Tokenizer& tokenizer) {
		switch (tokenizer.get_token_preview().type) {
		case Tokenizer::TokenType::Identifier:
			return ParseIdentifierExpr(tokenizer);
		case Tokenizer::TokenType::Number:
			return ParseNumberExpr(tokenizer);
		case Tokenizer::TokenType::RBOpen:
			return ParseParenExpr(tokenizer);
		default:
			nullptr;
		}
	}

	static std::unique_ptr<ExprAST> ParseBinOpRHS(Tokenizer& tokenizer, int ExprPrec, std::unique_ptr<ExprAST> LHS) {
		// If this is a binop, find its precedence.
		while (true) {
			int TokPrec = GetTokPrecedence(tokenizer.get_token_preview());

			// If this is a binop that binds at least as tightly as the current binop,
			// consume it, otherwise we are done.
			if (TokPrec < ExprPrec)
				return LHS;

			// Okay, we know this is a binop.
			Tokenizer::TokenType BinOp = tokenizer.get_token().type;

			// Parse the primary expression after the binary operator.
			auto RHS = ParsePrimary(tokenizer);
			if (!RHS)
				return nullptr;

			// If BinOp binds less tightly with RHS than the operator after RHS, let
			// the pending operator take RHS as its LHS.
			int NextPrec = GetTokPrecedence(tokenizer.get_token_preview());
			if (TokPrec < NextPrec) {
				RHS = ParseBinOpRHS(tokenizer, TokPrec + 1, std::move(RHS));
				if (!RHS)
					return nullptr;
			}

			// Merge LHS/RHS.
			LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
		}
	}

	std::unique_ptr<ExprAST> ParseExpression(Tokenizer& tokenizer) {
		auto LHS = ParsePrimary(tokenizer);
		if (!LHS)
			return nullptr;

		return ParseBinOpRHS(tokenizer, 0, std::move(LHS));
	}

	/// prototype
///   ::= id '(' id* ')'
	std::unique_ptr<PrototypeAST> ParsePrototype(Tokenizer& tokenizer) {
		auto token_ident = tokenizer.get_token();
		if (token_ident.type != Tokenizer::TokenType::Identifier)
			return nullptr;
			//return LogErrorP("Expected function name in prototype");

		std::string FnName = std::any_cast<std::string>(token_ident.value);
		auto token_rbo = tokenizer.get_token();

		if (token_rbo.type != Tokenizer::TokenType::RBOpen)
			return nullptr;
			//return LogErrorP("Expected '(' in prototype");

		std::vector<std::string> ArgNames;
		auto token_arg_name = tokenizer.get_token();
		while (token_arg_name.type == Tokenizer::TokenType::Identifier) {
			ArgNames.push_back(std::any_cast<std::string>(token_arg_name.value));
			token_arg_name = tokenizer.get_token();
		}

		if (token_arg_name.type != Tokenizer::TokenType::RBClose)
			return nullptr;
			//return LogErrorP("Expected ')' in prototype");

		// success.
		//tokenizer.get_token(); // eat ')'.

		return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
	}

	/// definition ::= 'def' prototype expression
	std::unique_ptr<FunctionAST> ParseDefinition(Tokenizer& tokenizer) {
		tokenizer.get_token(); // eat def.
		auto Proto = ParsePrototype(tokenizer);
		if (!Proto)
			return nullptr;

		if (auto E = ParseExpression(tokenizer))
			return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
		return nullptr;
	}

	/// toplevelexpr ::= expression
	std::unique_ptr<FunctionAST> ParseTopLevelExpr(Tokenizer& tokenizer) {
		if (auto E = ParseExpression(tokenizer)) {
			// Make an anonymous proto.
			auto Proto = std::make_unique<PrototypeAST>("__anon_expr",
				std::vector<std::string>());
			return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
		}
		return nullptr;
	}

	/// external ::= 'extern' prototype
	std::unique_ptr<PrototypeAST> ParseExtern(Tokenizer& tokenizer) {
		tokenizer.get_token();
		return ParsePrototype(tokenizer);
	}


	//===----------------------------------------------------------------------===//
	// Top-Level parsing
	//===----------------------------------------------------------------------===//

	static void HandleDefinition(Tokenizer tokenizer) {
		if (ParseDefinition(tokenizer)) {
			fprintf(stderr, "Parsed a function definition.\n");
		} else {
			// Skip token for error recovery.
			tokenizer.get_token();
		}
	}

	static void HandleExtern(Tokenizer tokenizer) {
		if (ParseExtern(tokenizer)) {
			fprintf(stderr, "Parsed an extern\n");
		} else {
			// Skip token for error recovery.
			tokenizer.get_token();
		}
	}

	static void HandleTopLevelExpression(Tokenizer tokenizer) {
		// Evaluate a top-level expression into an anonymous function.
		if (ParseTopLevelExpr(tokenizer)) {
			fprintf(stderr, "Parsed a top-level expr\n");
		} else {
			// Skip token for error recovery.
			tokenizer.get_token();
		}
	}
}

static llvm::ExitOnError ExitOnErr;

AST::AST() {
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();
	context = std::make_unique<llvm::LLVMContext>();

	llvm_module = std::make_unique<llvm::Module>("my cool jit", *context);
	jit = ExitOnErr(llvm::orc::KaleidoscopeJIT::Create());
	builder = std::make_unique<llvm::IRBuilder<>>(*context);

	llvm_module->setDataLayout(jit->getDataLayout());
	// Create new pass and analysis managers.
	TheFPM = std::make_unique<llvm::FunctionPassManager>();
	TheLAM = std::make_unique<llvm::LoopAnalysisManager>();
	TheFAM = std::make_unique<llvm::FunctionAnalysisManager>();
	TheCGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
	TheMAM = std::make_unique<llvm::ModuleAnalysisManager>();
	ThePIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
	TheSI = std::make_unique<llvm::StandardInstrumentations>(*context, /*DebugLogging*/ true);
	TheSI->registerCallbacks(*ThePIC, TheMAM.get());

	// Add transform passes.
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	TheFPM->addPass(llvm::InstCombinePass());
	// Reassociate expressions.
	TheFPM->addPass(llvm::ReassociatePass());
	// Eliminate Common SubExpressions.
	TheFPM->addPass(llvm::GVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	TheFPM->addPass(llvm::SimplifyCFGPass());

	// Register analysis passes used in these transform passes.
	llvm::PassBuilder PB;
	PB.registerModuleAnalyses(*TheMAM);
	PB.registerFunctionAnalyses(*TheFAM);
	PB.crossRegisterProxies(*TheLAM, *TheFAM, *TheCGAM, *TheMAM);
}

void AST::build(Tokenizer& self) {
	BinopPrecedence[Tokenizer::TokenType::Less] = 10;
	BinopPrecedence[Tokenizer::TokenType::Great] = 10;
	BinopPrecedence[Tokenizer::TokenType::Plus] = 20;
	BinopPrecedence[Tokenizer::TokenType::Minus] = 20;
	BinopPrecedence[Tokenizer::TokenType::Mult] = 40;
	BinopPrecedence[Tokenizer::TokenType::Div] = 40;

	bool process = true;
	std::vector<std::unique_ptr<PrototypeAST>> root_protoyps;

	while (process) {
		Tokenizer::Token token_preview = self.get_token_preview();

		switch (token_preview.type) {
		case Tokenizer::TokenType::Def :
			root.emplace_back(ParseDefinition(self));
			break;

		case Tokenizer::TokenType::Extern:
			root_protoyps.emplace_back(ParseExtern(self));

			break;

		case Tokenizer::TokenType::Eof :
			process = false;
			break;
		}

	}
	Enviroment env;

	for (auto& functions : root_protoyps) {
		functions->codegen(*context, *builder, *llvm_module, env);
	}

	for (auto& functions : root) {
		functions->codegen(*context, *builder, *llvm_module, env);
	}
	llvm_module->print(llvm::errs(), nullptr);

	auto RT = jit->getMainJITDylib().createResourceTracker();
	auto TSM = llvm::orc::ThreadSafeModule(std::move(llvm_module), std::move(context));
	jit->addModule(std::move(TSM), RT);
	auto ExprSymbol = ExitOnErr(jit->lookup("main"));
	double (*FP)() = ExprSymbol.getAddress().toPtr<double (*)()>();
	fprintf(stderr, "Evaluated to %f\n", FP());
	ExitOnErr(RT->remove());
}
