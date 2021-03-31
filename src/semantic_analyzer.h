#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "syntax_tree.h"
#include "symbol_table.h"
#include <string>

class SemanticAnalyzer
{
private:
  SyntaxTree syntax_tree;
  bool result = true;

public:
  SemanticAnalyzer(const SyntaxTree& );
  void start_analyze();
  void get_exp_type(const int& , const std::string& );
  // 下面是一堆产生式函数的声明
};

#endif