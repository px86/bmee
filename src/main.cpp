#include "bmee.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char **argv) {

  std::string expr;
  std::getline(std::cin, expr);
  while (std::cin) {
    if (!expr.empty()) {
      Parser parser(lex(expr));
      std::cout << evaluate(parser.parse()) << '\n';
    }
    std::getline(std::cin, expr);
  }

  return EXIT_SUCCESS;
}
