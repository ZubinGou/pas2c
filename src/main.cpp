#include "parser.h"

using namespace std;  // recommend only use it in .cpp

const string GRAMMAR_FILE = "../include/grammar.json";
const string INPUT_FILE = "../example/gcd.pas";

int main() {
  Parser parser(GRAMMAR_FILE, INPUT_FILE);
  SyntaxTree tree = parser.generate_tree();

  return 0;
}