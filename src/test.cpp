#include "parser.h"
#include "code_generator.h"
#include "semantic_analyzer.h"


using namespace std;  // recommend only use it in .cpp

const string GRAMMAR_FILE = "../include/grammar.json";
const string INPUT_FILE = "../example/gcd.pas";

int main() {
  cout << "\n*** Section 0 ***\n";
  Parser parser(GRAMMAR_FILE, INPUT_FILE);
  SyntaxTree tree = parser.generate_tree();

  cout << "\n*** Section 1 ***\n";
  SemanticAnalyzer semantic_analyzer(tree);
  semantic_analyzer.print_table();

  cout << "\n*** Section 2 ***\n";
  CodeGenerator code_generator(tree.node_dic, semantic_analyzer);
  cout << code_generator.run() << endl;

  return 0;
}