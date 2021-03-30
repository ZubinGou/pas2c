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
  SemanticAnalyzer(SyntaxTree tree);
  void start_analyze();
  void get_exp_type(int nodeid, std::string symbol_table_name);
  // 下面是一堆产生式函数的声明
};

#endif