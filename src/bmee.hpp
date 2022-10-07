#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class Token;
class Parser;

class Operation;

class UnaryPlus;
class UnaryMinus;
class Addition;
class Subtraction;
class Multiplication;
class Division;
class Modulo; // TODO
class Number;

auto lex(const std::string &src) -> std::vector<Token>;
auto evaluate(const std::unique_ptr<Operation> &opr) -> double;

class Token {
public:
  enum class Type {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    LEFT_PAREN,
    RIGHT_PAREN,
  } type;

  size_t position;
  double value;

  Token() = delete;
  Token(Token::Type tok_type, size_t pos, double val = 0)
      : type(tok_type), position(pos), value(val) {}
};

// Helper function
auto token_type_str(Token::Type t) -> const char *;

class Parser {
public:
  auto parse() -> std::unique_ptr<Operation>;

  Parser() = delete;
  Parser(const std::vector<Token> &tokens) : m_tokens(tokens) {}
  Parser(std::vector<Token> &&tokens) : m_tokens(std::move(tokens)) {}

private:
  auto parse_expression() -> Operation *;
  auto parse_term() -> Operation *;
  auto parse_factor() -> Operation *;

  auto consume() -> const Token & { return m_tokens.at(m_pos++); }
  auto peek() const -> const Token & { return m_tokens.at(m_pos); };

  auto empty() const -> bool { return m_pos >= m_tokens.size(); }
  auto expect(Token::Type type) -> bool;

  std::vector<Token> m_tokens;
  size_t m_pos = 0;
};

class Operation {
public:
  virtual double evaluate() = 0;
  virtual ~Operation() = default;
};

class Addition : public Operation {
public:
  double evaluate() { return left->evaluate() + right->evaluate(); };
  ~Addition() { delete left; delete right; }
  Operation *left;
  Operation *right;
};

class Subtraction : public Operation {
public:
  double evaluate() { return left->evaluate() - right->evaluate(); };
  ~Subtraction() { delete left; delete right; }
  Operation *left;
  Operation *right;
};

class Multiplication : public Operation {
public:
  double evaluate() { return left->evaluate() * right->evaluate(); };
  ~Multiplication() { delete left; delete right; }
  Operation *left;
  Operation *right;
};

class Division : public Operation {
public:
  double evaluate() { return left->evaluate() / right->evaluate(); };
  ~Division() { delete left; delete right; }
  Operation *left;
  Operation *right;
};

class Modulo : public Operation {
public:
  double evaluate() {
    return (std::uint64_t)left->evaluate() % (std::uint64_t)right->evaluate();
  };
  ~Modulo() { delete left; delete right; }
  Operation *left;
  Operation *right;
};

class UnaryPlus : public Operation {
public:
  double evaluate() { return right->evaluate(); }
  ~UnaryPlus() { delete right; }
  Operation *right;
};

class UnaryMinus : public Operation {
public:
  double evaluate() { return -1 * right->evaluate(); }
  ~UnaryMinus() { delete right; }
  Operation *right;
};

class Number : public Operation {
public:
  double evaluate() { return value; }
  ~Number() = default;
  double value;
};
