#pragma once

#include <any>
#include <list>
#include <string_view>
#include <vector>

struct Tokenizer {
	enum class TokenType {
		Eof,
		Def,
		If,
		Else,
		Then,
		Plus,
		Minus,
		Div,
		Mult,
		Less,
		Great,
		Сomma,
		RBOpen,
		RBClose,
		Extern,
		Identifier,
		Number
	};

	struct Token {
		TokenType type;
		std::any value;
	};

	Tokenizer(std::string from_parse);

	Token get_token_preview();
	Token get_token();
	Token get_token_next();
private:
	int index = 0;
	std::vector<Token> tokens;
	std::vector<Token>::iterator begin;
};
