#include "tokenizer.h"
#include <set>
#include <ranges>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string_view>
#include <regex>
#include <vector>
#include <execution>
#include <charconv>
#include <functional>

namespace { // private
	std::vector<std::string> split(const std::string& str, std::string_view pattern) {
		const auto r = std::regex(pattern.data());
		return std::vector<std::string> {
			std::sregex_token_iterator(str.begin(), str.end(), r, -1),
				std::sregex_token_iterator()
		};
	}

	Tokenizer::Token word_to_token(Tokenizer::TokenType type, std::string_view word, std::string_view input) {
		if (word == input) {
			return Tokenizer::Token{ .type = type };
		}
		return Tokenizer::Token{ .type = Tokenizer::TokenType::Eof };
	}

	Tokenizer::Token word_to_number(std::string_view input) {
		double value;
		if (std::from_chars(input.data(), input.data() + input.size(), value).ec != std::errc::invalid_argument) {
			return Tokenizer::Token{ .type = Tokenizer::TokenType::Number, .value = value };
		}
		return Tokenizer::Token{ .type = Tokenizer::TokenType::Eof };
	}

	Tokenizer::Token word_to_ident(std::string_view input) {
		return Tokenizer::Token{ .type = Tokenizer::TokenType::Identifier, .value = std::string(input) };
	}


	struct TokenPair {
		Tokenizer::TokenType type;
		std::function< Tokenizer::Token(std::string_view input) > function;
	};

	std::vector<TokenPair> parsers = {
	   { Tokenizer::TokenType::Def, std::bind(word_to_token, Tokenizer::TokenType::Def, "def", std::placeholders::_1) },
	   { Tokenizer::TokenType::Extern, std::bind(word_to_token, Tokenizer::TokenType::Extern, "extern", std::placeholders::_1) },
	   { Tokenizer::TokenType::RBOpen, std::bind(word_to_token, Tokenizer::TokenType::RBOpen, "(", std::placeholders::_1) },
	   { Tokenizer::TokenType::RBClose, std::bind(word_to_token, Tokenizer::TokenType::RBClose, ")", std::placeholders::_1) },
	   { Tokenizer::TokenType::Plus, std::bind(word_to_token, Tokenizer::TokenType::Plus, "+", std::placeholders::_1) },
	   { Tokenizer::TokenType::Minus, std::bind(word_to_token, Tokenizer::TokenType::Minus, "-", std::placeholders::_1) },
	   { Tokenizer::TokenType::Div, std::bind(word_to_token, Tokenizer::TokenType::Div, "/", std::placeholders::_1) },
	   { Tokenizer::TokenType::If, std::bind(word_to_token, Tokenizer::TokenType::If, "if", std::placeholders::_1) },
	   { Tokenizer::TokenType::Great, std::bind(word_to_token, Tokenizer::TokenType::Great, ">", std::placeholders::_1) },
	   { Tokenizer::TokenType::Less, std::bind(word_to_token, Tokenizer::TokenType::Less, "<", std::placeholders::_1) },
	   { Tokenizer::TokenType::Then, std::bind(word_to_token, Tokenizer::TokenType::Then, "then", std::placeholders::_1) },
	   { Tokenizer::TokenType::Else, std::bind(word_to_token, Tokenizer::TokenType::Else, "else", std::placeholders::_1) },
	   { Tokenizer::TokenType::Сomma, std::bind(word_to_token, Tokenizer::TokenType::Сomma, ",", std::placeholders::_1) },
	   { Tokenizer::TokenType::Number, word_to_number },
	   { Tokenizer::TokenType::Identifier, word_to_ident },
	};
}


Tokenizer::Tokenizer(std::string from_parse) {
	// Phase 1 text clean
	std::string ready_from_split = std::regex_replace(from_parse, std::regex(R"([()+\-*])"), " $& "); // add space 
	std::vector words = split(ready_from_split, R"(\s+)");// space char one and more

	// Phase 2 text to tokens
	tokens.resize(words.size());
	std::transform(std::execution::par, words.begin(), words.end(), tokens.begin(), // make tokens from words
		[](const std::string& input)-> Token {
			for (auto& parser : parsers) {
				Token temp = parser.function(input);
				if (temp.type != TokenType::Eof) {
					return temp;
				}
			}
			return Token { .type = TokenType::Eof };
		}
	);

	begin = tokens.begin();
}


Tokenizer::Token Tokenizer::get_token_preview() {
	if (begin != tokens.end())
		return *begin;

	return { TokenType::Eof };
}

Tokenizer::Token Tokenizer::get_token() {
	index++;
	if (begin != tokens.end())
		return *begin++;

	return { TokenType::Eof };
}

Tokenizer::Token Tokenizer::get_token_next() {
	if (begin + 1 != tokens.end())
		return *(begin + 1);

	return { TokenType::Eof };
}
