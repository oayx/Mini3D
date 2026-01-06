#pragma once

#include <iostream>
#include <vector>
#include <cctype>
#include <optional>
#include <memory>
#include <stdexcept>

#include "LexerDefined.h"
#include "StringUtils.h"

namespace shaderlab {
	class ShaderLexer {
	public:
		ShaderLexer(const std::string& fileName, const std::string& input)
			: _fileName(fileName), _input(input), _position(0), _line(1), _column(1) {}

		std::vector<Token> tokenize() {
			std::vector<Token> tokens;
			while (_position < _input.size()) {
				const char current = peek();

				if (std::isspace(current)) {
					consumeWhitespace();
				}
				else if (current == '/' && peekNext() == '/') {
					consumeLineComment();
				}
				else if (current == '/' && peekNext() == '*') {
					consumeBlockComment();
				}
				else if (current == '"') {
					tokens.push_back(consumeString());
				}
				else if (std::isalpha(current) || current == '_' || ((current == '2' || current == '3') && peekNext() == 'D')) {//2D/3D texture
					tokens.push_back(consumeIdentifierOrKeyword());
				}
				else if ((std::isdigit(current) || current == '-' || current == '+')) {
					tokens.push_back(consumeNumber());
				}
				else if ((current == '.' && std::isdigit(peekNext()))) {//.01
					tokens.push_back(consumeNumber());
				}
				else if (isSymbol(current)) {
					tokens.push_back(consumeSymbol());
				}
				else {
					tokens.push_back({ TokenType::Unknown, std::string(1, current), _line, _column });
					advance();
				}
			}

			tokens.push_back({ TokenType::EndOfFile, "", _line, _column });
			return tokens;
		}

	private:
		const std::string _fileName;
		const std::string _input;
		size_t _position;
		size_t _line;
		size_t _column;

		char peek() const noexcept {
			return _position < _input.size() ? _input[_position] : '\0';
		}

		char peekNext() const noexcept {
			return _position + 1 < _input.size() ? _input[_position + 1] : '\0';
		}

		void advance()noexcept {
			if (_position < _input.size()) {
				if (_input[_position] == '\n') {
					_line++;
					_column = 1;
				}
				else {
					_column++;
				}
				_position++;
			}
		}

		void consumeWhitespace() noexcept {
			while (_position < _input.size() && std::isspace(peek())) {
				advance();
			}
		}

		void consumeLineComment() noexcept {
			advance(); // consume /
			advance(); // consume /

			while (_position < _input.size() && peek() != '\n') {
				advance();
			}
		}

		void consumeBlockComment() noexcept {
			advance(); // consume /
			advance(); // consume *

			while (_position < _input.size() && !(peek() == '*' && peekNext() == '/')) {
				advance();
			}

			if (_position < _input.size()) {
				advance(); // consume *
				advance(); // consume /
			}
		}

		Token consumeString() {
			const size_t startLine = _line;
			const size_t startColumn = _column;

			advance(); // consume opening "
			std::string value;

			while (_position < _input.size() && peek() != '"') {
				if (peek() == '\\') {
					advance(); // consume escape
					if (_position < _input.size()) {
						value += _input[_position];
						advance();
					}
				}
				else {
					value += peek();
					advance();
				}
			}

			if (peek() == '"') {
				advance(); // consume closing "
			}

			return { TokenType::StringLiteral, value, startLine, startColumn };
		}

		Token consumeNumber() {
			const size_t startLine = _line;
			const size_t startColumn = _column;
			std::string value;

			bool hasDecimal = false;
			bool hasExponent = false;

			if (peek() == '+' || peek() == '-') {
				value += peek();
				advance();
			}

			while (_position < _input.size()) {
				const char c = peek();
				if (std::isdigit(c)) {
					value += c;
					advance();
				}
				else if (c == '.' && !hasDecimal && !hasExponent) {
					hasDecimal = true;
					value += c;
					advance();
				}
				else if ((c == 'e' || c == 'E') && !hasExponent) {
					hasExponent = true;
					value += c;
					advance();
					if (peek() == '+' || peek() == '-') {
						value += peek();
						advance();
					}
				}
				else {
					break;
				}
			}

			return { TokenType::NumberLiteral, value, startLine, startColumn };
		}

		Token consumeIdentifierOrKeyword() {
			const size_t startLine = _line;
			const size_t startColumn = _column;
			std::string value;

			while (_position < _input.size() && (std::isalnum(peek()) || peek() == '_')) {
				value += peek();
				advance();
			}

			if (StringUtils::EqualIgnoreCase(value, "CGPROGRAM")) {
				return consumeShaderBlock(TokenType::CGProgram);
			}
			else if (StringUtils::EqualIgnoreCase(value, "HLSLPROGRAM")) {
				return consumeShaderBlock(TokenType::HLSLProgram);
			}
			else if (StringUtils::EqualIgnoreCase(value, "GLSLPROGRAM")) {
				return consumeShaderBlock(TokenType::GLSLProgram);
			}
			if (StringUtils::EqualIgnoreCase(value, "CGINCLUDE")) {
				return consumeShaderBlock(TokenType::CGInclude);
			}
			else if (StringUtils::EqualIgnoreCase(value, "HLSLINCLUDE")) {
				return consumeShaderBlock(TokenType::HLSLInclude);
			}
			else if (StringUtils::EqualIgnoreCase(value, "GLSLINCLUDE")) {
				return consumeShaderBlock(TokenType::GLSLInclude);
			}

			auto it = Keyword2Tokens.find(value);
			if (it != Keyword2Tokens.end()) {
				return { it->second, value, startLine, startColumn };
			}

			return { TokenType::Identifier, value, startLine, startColumn };
		}

		Token consumeShaderBlock(TokenType type) {
			const size_t startLine = _line;
			const size_t startColumn = _column;
			std::string value;

			std::string endMarker = "ENDCG";
			switch (type)
			{
			case TokenType::CGProgram:
			case TokenType::CGInclude:
				endMarker = "ENDCG";
				break;
			case TokenType::HLSLProgram:
			case TokenType::HLSLInclude:
				endMarker = "ENDHLSL";
				break;
			case TokenType::GLSLProgram:
			case TokenType::GLSLInclude:
				endMarker = "ENDGLSL";
				break;
			default:break;
			}

			advance(); // consume \n
			while (_position < _input.size()) {
				if (peek() == 'E' && _position + endMarker.size() <= _input.size()) {
					std::string potentialEnd = _input.substr(_position, endMarker.size());
					if (potentialEnd == endMarker) {
						_position += endMarker.size();
						break;
					}
				}
				value += peek();
				advance();
			}

			return { type, value, startLine, startColumn };
		}

		Token consumeSymbol() {
			const size_t startLine = _line;
			const size_t startColumn = _column;
			const char c = peek();
			advance();

			static const std::unordered_map<char, TokenType> symbols = {
				{'{', TokenType::SymbolLBrace},
				{'}', TokenType::SymbolRBrace},
				{'[', TokenType::SymbolLBracket},
				{']', TokenType::SymbolRBracket},
				{'(', TokenType::SymbolLParent},
				{')', TokenType::SymbolRParent},
				{'=', TokenType::SymbolEqual},
				{',', TokenType::SymbolComma},
				{';', TokenType::SymbolSemicolon},
				{':', TokenType::SymbolColon},
			};

			return { symbols.at(c), std::string(1, c), startLine, startColumn };
		}

		bool isSymbol(char c) const {
			static const std::string symbols = "{}[]=,;:()";
			return symbols.find(c) != std::string::npos;
		}
	};
}