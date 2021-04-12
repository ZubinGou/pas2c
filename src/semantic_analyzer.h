#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "syntax_tree.h"
#include "symbol_table.h"
#include <string>
#include <vector>

class SemanticAnalyzer
{
private:
  SyntaxTree syntax_tree;
  bool result = true;

public:
  SemanticAnalyzer(const SyntaxTree& );
  SymbolTable symbol_table(); 
  SymbolTableController controller;
  void start_analyze();
  std::string get_exp_type(const int& , const std::string& );
  // 下面是一堆产生式函数的声明

   /*  programstruct -> program_head ; program_body  */
  void programstruct(void);

  void program_head(const int& );

  void program_body(const int& );

// 返回[expression, return_type, None, None]或[node, type, 行号, 列号]或[array, element_type, 行号，列号]
  std::vector<std::string> expression(const int& );

};

#endif