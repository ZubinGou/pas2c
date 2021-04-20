#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "syntax_tree.h"
#include "symbol_table.h"
#include <string>
#include <vector>
#include <iostream>

struct returnList{
  returnList(std::string id_name, std::string type, std::string row, 
  std::string column, std::string value_type):
        id_name(id_name), 
        type(type), 
        row(row), 
        column(column), 
        value_type(value_type){};
  returnList():id_name(""), type(""), row(""), column(""), value_type(""){};
  std::string id_name;
  std::string type;
  std::string row;
  std::string column;
  std::string value_type;
  bool empty(){
    if(id_name == "" && type == "" && row == "" && column == "" && value_type == "")
      return true;
    else  
      return false;
  }
};

class SemanticAnalyzer
{
private:
  SyntaxTree syntax_tree;
  SymbolTableController symbol_table_controller;
  bool result = true;

public:
  SemanticAnalyzer(const SyntaxTree& );
  SemanticAnalyzer(){};
  SymbolTable symbol_table(); 
  SymbolTableController controller;
  void start_analyze();
  std::string get_exp_type(const int& , const std::string& );
  // 下面是一堆产生式函数的声明

  // programstruct -> program_head ; program_body
  void programstruct(void);
  // program_head -> program id ( idlist ) | program id
  void program_head(const int& );
  // program_body-> const_declarations var_declarations subprogram_declarations compound_statement
  void program_body(const int& );
  // idlist -> idlist, id | id
  std::vector<returnList> idlist(const int& );

  // basic_type -> integer | real | boolean | char
  std::string basic_type(const int& );
  
  // parameter_list -> parameter_list ; parameter | parameter
  std::vector<Argument> parameter_list(const int& );
  // parameter -> var_parameter | value_parameter
  std::vector<Argument> parameter(const int& );
  // var_parameter -> var value_parameter
  std::vector<Argument> var_parameter(const int& );
  // value_parameter -> idlist : simple_type
  std::vector<Argument> value_parameter(const int& );
  // compound_statement -> begin statement_list end
  void compound_statement(const int& );
  // statement_list -> statement_list ; statement | statement
  void statement_list(const int& );
  /* statement -> variable assignop expression | procedure_call | compound_statement
                 | if expression then statement else_part | while expression do statement
                 | for id assignop expression to expression do statement
                 | read ( variable_list ) | write ( expression_list ) | None */
  void statement(const int& );
  // variable_list -> variable_list , variable | variable
  std::vector<returnList> variable_list(const int& );
  // variable -> id id_varpart
  returnList variable(const int& );
  // id_varpart -> [ expression_list ] | None
  void id_varpart(const int& );
  // procedure_call -> id | id ( expression_list )
  void procedure_call(const int& );
  // else_part -> else statement | None
  void else_part(const int& );
  // expression_list -> expression_list , expression | expression
  std::vector<returnList> expression_list(const int& );
  // expression -> simple_expression relop simple_expression | simple_expression
  returnList expression(const int& );
  // simple_expression -> simple_expression addop term | term
  returnList simple_expression(const int& );
  // term -> term mulop factor | factor
  // mulop的集合为{'*', '/', 'div', 'mod', 'and'}
  returnList term(const int& );
  // factor -> num | variable | id ( expression_list ) | ( expression ) | not factor | uminus factor
  returnList factor(const int& );
};

#endif