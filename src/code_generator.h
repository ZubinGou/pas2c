#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stack>

#include "semantic_analyzer.h"
#include "syntax_tree.h"

/*
struct Node {
  int father;             // 父节点的编号
  int son_num;            // 子结点的个数
  int id;                 // 节点编号
  std::string type;       // 结点代表的符号类型
  double num_value;       // 节点携带的数值
  std::string str_value;  // 节点携带的字符串值
  bool is_terminal;       // 当前节点是否为终结符
  int line;                // 如果是终结符，终结符的行号
  int col;                // 如果是终结符，终结符的列号
  std::vector<int> son;   // 子节点的编号集
};
*/

class CodeGenerator {
 public:
  CodeGenerator(const std::vector<Node>& syntax_tree, const SemanticAnalyzer& semantic_analyzer) {
    this->tree = syntax_tree;
    this->table_anal = semantic_analyzer;
    this->table_ctrl = semantic_analyzer.controller;
  }
  
  std::string run(); // 返回 target_code

 private:
  std::vector<Node> tree; // 语法树的根节点
  SemanticAnalyzer table_anal;
  SymbolTableController table_ctrl; // 符号表控制器
  std::stack<std::string> state_stack; // 状态栈，标记作用域
  std::string target_code; // 目标代码

  // target_code manipulate
  void target_append(std::string code); // 部分代码到目标代码
  void add_indent(); // 为目标代码添加缩进

  // Parser interface
  std::vector<int> get_son(int node_id); // return tree[node_id].son
  int get_father(int node_id); // return tree[node_id].father
  void match(int node_id, std::string token); // match current node and token
  std::string get_token(int node_id);
  std::string get_str_value(int node_id);
  double get_num_value(int node_id);

  // Semantic intrface
  bool is_addr(std::string id);
  bool is_func(std::string id);
  std::string get_var_type(std::string id);
  std::string get_exp_type(int node_id);
  std::vector<int> get_bound(std::string id);
  std::vector<bool> get_args(std::string id);

  // Production
  /*
  programstruct -> program_head ; program_body .
  program_head -> program id ( idlist ) | program id
  program_body -> const_declarations var_declarations subprogram_declarations compound_statement
  idlist -> idlist , id | id
  const_declarations -> const const_declaration ; | e
  const_declaration -> const_declaration ; id = const_value | id = const_value 
  const_value -> + num | - num | num | 'letter'
  var_declarations -> var var_declaration ; | e
  var_declaration -> var_declaration ; idlist : type | idlist : type
  type -> basic_type | array [ period ] of basic_type
  basic_type -> integer | real | boolean | char
  period -> period , digits .. digits | digits .. digits
  subprogram_declarations -> subprogram_declarations subprogram ; | e
  subprogram -> subprogram_head ; subprogram_body
  subprogram_head -> procedure id formal_parameter | function id formal_parameter : basic_type 
  formal_parameter -> ( parameter_list ) | e
  parameter_list -> parameter_list ; parameter | parameter
  parameter -> var_parameter | value_parameter
  var_parameter -> var value_parameter
  value_parameter -> idlist : basic_type
  */
  void programstruct(int node_id);
  void program_head(int node_id);
  void program_body(int node_id);
  void idlist(int node_id, vector<int> id_num, string id_type="", bool id_addr=false);
  void const_declarations(int node_id);
  void const_declaration(int node_id);
  void const_value(int node_id);
  void var_declarations(int node_id);
  void var_declaration(int node_id);
  void type(int node_id);
  std::string basic_type(int node_id);
  int period(int node_id);
  void subprogram_declarations(int node_id);
  void subprogram(int node_id);
  void subprogram_head(int node_id);
  void formal_parameter(int node_id);
  void parameter_list(int node_id);
  void parameter(int node_id);
  void var_parameter(int node_id);
  void value_parameter(int node_id);

  /*
  subprogram_body -> const_declarations var_declarations compound_statement
  compound_statement -> begin statement_list end
  statement_list -> statement_list ; statement | statement
  statement -> variable assignop expression | procedure_call | compound_statement | if expression then statement else_part | for id assignop expression to expression do statement | read ( variable_list ) | write ( expression_list ) | e
  variable_list -> variable_list , variable | variable
  variable -> id id_varpart
  id_varpart -> [ expression_list ] | e
  procedure_call -> id | id ( expression_list )
  else_part -> else statement | e
  expression_list -> expression_list , expression | expression
  expression -> simple_expression relop simple_expression | simple_expression
  simple_expression -> simple_expression addop term | term
  term -> term mulop factor | factor
  factor -> num | variable | id ( expression_list ) | ( expression ) | not factor | uminus factor
  */
  void subprogram_body(int node_id);
  void compound_statement(int node_id);

  // debug
  void print_target_code(); // 打印目标代码
};

#endif