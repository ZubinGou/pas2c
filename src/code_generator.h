#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "syntax_tree.h"
#include "semantic_analyzer.h"

class CodeGenerator {
 public:
  CodeGenerator(SyntaxTree syntax_tree, SemanticAnalyzer semantic_analyzer){
    this->syntax_tree = syntax_tree;
    this->symbol_table = semantic_analyzer.symbol_table();
  }
  SyntaxTree syntax_tree;
  SymbolTable symbol_table;
  std::stack<std::string> state_stack;
  std::string target_code;
  
 private:
  SyntaxTree syntax_tree;
  
  void output_buffer();
  void print_code();

  // 每个产生式对应的函数
  void start_generate();

};

#endif