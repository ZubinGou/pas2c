#include "parser.h"

using namespace std;  // recommend only use it in .cpp

const string GRAMMAR_FILE = "../include/grammar.json";
string INPUT_FILE = "../example/parser_test.pas";

int main() {
  Parser parser(GRAMMAR_FILE, INPUT_FILE);

  return 0;
}