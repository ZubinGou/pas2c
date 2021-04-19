#include "parser.h"
#include "code_generator.h"
#include "semantic_analyzer.h"


using namespace std;  // recommend only use it in .cpp

const string GRAMMAR_FILE = "../include/grammar.json";
const string INPUT_FILE = "../example/gcd.pas";

int main() {
  Parser parser(GRAMMAR_FILE, INPUT_FILE);
  SyntaxTree tree = parser.generate_tree();

  SemanticAnalyzer semantic_analyzer(tree);
  semantic_analyzer.start_analyze();

  CodeGenerator code_generator(tree.node_dic, semantic_analyzer.controller)

  return 0;
}