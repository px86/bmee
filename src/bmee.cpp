#include "bmee.hpp"
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <iostream>
#include <memory>

auto lex(const std::string &src) -> std::vector<Token>
{

#define emit_token(type, pos) tokens.push_back(Token(type, pos))

  std::vector<Token> tokens;
  for (size_t i = 0; i < src.size(); ++i) {
    switch (src.at(i)) {
    case ' ':
    case '\t':
    case '\n': continue;
    case '+': emit_token(Token::Type::PLUS, i); break;
    case '-': emit_token(Token::Type::MINUS, i); break;
    case '*': emit_token(Token::Type::MULTIPLY, i); break;
    case '/': emit_token(Token::Type::DIVIDE, i); break;
    case '%': emit_token(Token::Type::MODULO, i); break;
    case '(': emit_token(Token::Type::LEFT_PAREN, i); break;
    case ')': emit_token(Token::Type::RIGHT_PAREN, i); break;
    default:
      {
	if (isdigit(src.at(i))) {
	  size_t start = i, span = 1;
	  for (size_t pos = start + span;
	       pos < src.size() && (isdigit(src.at(pos)) || src.at(pos) == '.');
	       ++span, ++pos
	       );
	  tokens.push_back(Token(Token::Type::NUMBER, start,
				 std::stod(src.substr(start, span))));
	  i += span - 1;
	} else {
	  std::cerr << "Unknow token at position: " << i << std::endl;
	  std::exit(EXIT_FAILURE);
	}
      }
    }
  }
#undef emit_token

  return tokens;
}

auto token_type_str(Token::Type t) -> const char *
{
  switch (t) {
  case Token::Type::NUMBER: return "NUMBER";
  case Token::Type::PLUS: return "PLUS";
  case Token::Type::MINUS: return "MINUS";
  case Token::Type::MULTIPLY: return "MULTIPLY";
  case Token::Type::DIVIDE: return "DIVIDE";
  case Token::Type::MODULO: return "MODULO";
  case Token::Type::LEFT_PAREN: return "LEFT_PAREN";
  case Token::Type::RIGHT_PAREN: return "RIGHT_PAREN";
  }
  return "UNKNOWN_TOKEN";
}

auto Parser::parse_expression() -> Operation *
{
  auto tok = peek();
  switch (tok.type) {
  case Token::Type::PLUS:
    {
      consume();
      auto expr = parse_expression();
      UnaryPlus *unaryplus = new UnaryPlus();
      unaryplus->right = expr;
      return unaryplus;
    }
  case Token::Type::MINUS:
    {
      consume();
      auto expr = parse_expression();
      UnaryMinus *unaryminus = new UnaryMinus();
      unaryminus->right = expr;
      return unaryminus;
    }
  default:
    {
      auto term = parse_term();
      if (empty()) return term;

      switch (peek().type) {
      case Token::Type::PLUS:
	{
	  consume();
	  Addition *addition = new Addition();
	  addition->left = term;
	  addition->right = parse_expression();
	  return addition;
	}
      case Token::Type::MINUS:
	{
	  consume();
	  Subtraction *subtraction = new Subtraction();
	  subtraction->left = term;
	  subtraction->right = parse_expression();
	  return subtraction;
	}
      default: return term;
      }
    }
  }
}

auto Parser::parse_term() -> Operation *
{
  auto factor = parse_factor();
  if (empty()) return factor;
  auto tok = peek();
  switch (tok.type) {
  case Token::Type::MULTIPLY:
    {
      consume();
      Multiplication *multiplication = new Multiplication();
      multiplication->left = factor;
      multiplication->right = parse_expression();
      return multiplication;
    }
  case Token::Type::DIVIDE:
    {
      consume();
      Division *division = new Division();
      division->left = factor;
      division->right = parse_expression();
      return division;
    }
  default: return factor;
  }
}

auto Parser::parse_factor() -> Operation *
{
  auto tok = peek();
  switch (tok.type) {
  case Token::Type::LEFT_PAREN:
    {
      consume();
      auto expr = parse_expression();
      if (expect(Token::Type::RIGHT_PAREN))
	consume();
      return expr;
    }
  case Token::Type::NUMBER:
    {
      Number *num = new Number();
      num->value = consume().value;
      return num;
    }
  default:
    {
      std::cerr << "Factor: Unexpected token encountered!\n";
      std::exit(EXIT_FAILURE);
    }
  }
}

auto Parser::parse() -> std::unique_ptr<Operation>
{
  auto expr = parse_expression();
  if (!empty()) {
    std::cerr << "Unexpected token at: " << peek().position << std::endl;
    std::exit(EXIT_FAILURE);
  }
  return std::unique_ptr<Operation>(expr);
}

auto evaluate(const std::unique_ptr<Operation> &opr) -> double
{
  if (opr) return opr->evaluate();
  else {
    std::cerr << "Trying to evaluate null object." << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

auto Parser::expect(Token::Type type) -> bool
{
  if (peek().type != type) {
    std::cerr << "Expected " << token_type_str(type) << ", found "
              << token_type_str(peek().type) << " at " << peek().position
              << std::endl;
    std::exit(EXIT_FAILURE);
  }
  return true;
}
