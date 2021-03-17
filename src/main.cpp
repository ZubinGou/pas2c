#include "parser.h"

using namespace std;  // recommend only use it in .cpp

const string GRAMMAR_FILE = "../include/expression.json";

int main() {
  Parser parser(GRAMMAR_FILE);

  return 0;
}