#include "semantic_analyzer.h"
#include "symbol_table.h"
#include <string>
#include <vector>
using namespace std;

SemanticAnalyzer::SemanticAnalyzer(const SyntaxTree& tree){
  this->syntax_tree = tree;   //syntax tree
  this->symbol_table_controller = SymbolTableController();  //symbol table controller
  this->result = true;
  this->start_analyze();
}

void SemanticAnalyzer::start_analyze(){
  if(this->syntax_tree.find_inferior_node(0,0).type == "programstruct")
    programstruct();
}

string SemanticAnalyzer::get_exp_type(const int& nodeid, const string& symbol_table_name){
  this->controller.current_table = symbol_table_name;
  returnList result_item = this->expression(nodeid);
  this->controller.current_table = nullptr;
  if(result_item.empty() == false)
    return result_item.type;
  else
    return nullptr;
}

void SemanticAnalyzer::programstruct(void){
  int cur_nodeId = this->syntax_tree.find_inferior_node(0, 0).id;
  Node cur_node = this->syntax_tree.find_inferior_node(0, 0);
  program_head(cur_node.son[0]);
  if(syntax_tree.node_dic[cur_node.son[1]].type != ";"){
    this->result = false;
  }
  program_body(cur_node.son[2]);
  if(syntax_tree.node_dic[cur_node.son[3]].type != ";"){
    this->result = false;
  }
}

void SemanticAnalyzer::program_head(const int& node_id){

}

void SemanticAnalyzer::program_body(const int& node_id){

}

vector<returnList> SemanticAnalyzer::idlist(const int& id){

}

string SemanticAnalyzer::basic_type(const int& ){

}



vector<Argument> SemanticAnalyzer::parameter_list(const int& node_id){
  vector<Argument> parameters;
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if (cur_node.son_num == 1) {
    vector<Argument> sub_parameters = parameter(cur_node.son[0]);
    if(!sub_parameters.empty()){
      for(auto& it : sub_parameters){
        parameters.push_back(it);
      }
    }
  }
  else if (cur_node.son_num == 3) {
    vector<Argument> sub_parameters = parameter_list(cur_node.son[0]);
    if(!sub_parameters.empty()){
      for(auto& it : sub_parameters){
        parameters.push_back(it);
      }
    }
    vector<Argument> sub_parameters = parameter(cur_node.son[2]);
    if(!sub_parameters.empty()){
      for(auto& it : sub_parameters){
        parameters.push_back(it);
      }
    }
  }
  else {
    this->result = false;
    cout << "[semantic error] error on son number of current node." << endl;
  }
  return parameters;
}

vector<Argument> SemanticAnalyzer::parameter(const int& node_id){
  vector<Argument> parameter_result;
  Node son_node = this->syntax_tree.find_inferior_node(node_id, 0);
  if (son_node.type == "var_parameter"){
    for(auto& it : this->var_parameter(node_id)){
      parameter_result.push_back(it);
    }
  }
  else if (son_node.type == "value_parameter"){
    for(auto& it : this->value_parameter(node_id)){
      parameter_result.push_back(it);
    }
  }
  return parameter_result;
}

vector<Argument> SemanticAnalyzer::var_parameter(const int& node_id){
  vector<Argument> parameters;
  parameters = this->value_parameter(this->syntax_tree.find_inferior_node(node_id, 1).id);
  for(auto& it : parameters){
    it.pass_value = true;
  }
  return parameters;
}

vector<Argument> SemanticAnalyzer::value_parameter(const int& node_id){
  vector<Argument> parameters;
  Node cur_node = this->syntax_tree.node_dic[node_id];
  vector<returnList> id_list = this->idlist(cur_node.son[0]);
  string type_id = this->basic_type(cur_node.son[2]);
  for(auto& it : id_list){
    Argument sub_parameter = Argument(it.id_name, type_id, stoi(it.row));
    parameters.push_back(sub_parameter);
  }
  return parameters;
}

void SemanticAnalyzer::compound_statement(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if (this->syntax_tree.find_inferior_node(node_id, 0).type != "begin"){
  }
  else if (this->syntax_tree.find_inferior_node(node_id, 2).type != "end") {
    cout << "[semantic error] error on current node token" << endl;
  }
  else{
    this->statement_list(cur_node.son[1]);
  }
}

void SemanticAnalyzer::statement_list(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 1){
    this->statement(cur_node.son[0]);
  }
  else if(cur_node.son_num == 3){
    this->statement_list(cur_node.son[0]);
    this->statement(cur_node.son[2]);
  }
  else{
    result = false;
    cout << "[semantic error] error on son number of current node." << endl;
  }
}

// maybe a lot of bug here, logic in here is really complicate
void SemanticAnalyzer::statement(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];

  // statement -> procedure_call | compound_statement | None
  if(cur_node.son_num == 1){
    Node son_node = syntax_tree.find_inferior_node(node_id, 0);
    if(son_node.type == "procedure_call"){
      this->procedure_call(son_node.id);
    }
    else if(son_node.type == "compound_statement"){
      this->compound_statement(son_node.id);
    }
    else if(son_node.type != ""){
      cout << "[semantic error] error on current node token." << endl;
    }
  }

  // statement -> variable assignop expression
  else if(cur_node.son_num == 3){
    Node son_node_var = this->syntax_tree.find_inferior_node(node_id, 0);
    Node son_node_exp = this->syntax_tree.find_inferior_node(node_id, 2);
    returnList var = this->variable(son_node_var.id);
    returnList return_type = this->expression(son_node_exp.id);
    if(var.empty() || return_type.empty())
      return;
    
    SymbolTableElement result_item = this->symbol_table_controller.search_table(var.id_name, this->symbol_table_controller.current_table);

    if(result_item.empty() == false){
      if(var.type == "array"){
        var.type = var.value_type;  // e.g. int, real, char...
      }
      if(var.type != return_type.type){
        if(var.type == "real" && return_type.type == "integer"){
          this->result = false;
          cout << "[semantic error] row:" << var.row << " col:" << var.column << ", pass int value to real variable." << endl;
        }
        else if(var.type == "integer" && return_type.type == "real"){
          this->result = false;
          cout << "[semantic error] row:" << var.row << " col:" << var.column << ", pass real value to int variable." << endl;
        }
        else{
          this->result = false;
          cout << "[semantic error] row:" << var.row << " col:" << var.column << ", cannot assign value, type does not match." << endl;
        }
      }
    }
    else{
      this->result = false;
      cout << "[semantic error] row:" << var.row << " col:" << var.column << ", variable " << var.id_name << " is not defined." << endl;
    }
  }

  // statement -> while expression do statement | read ( variable_list ) | write ( expression_list )
  else if(cur_node.son_num == 4){
    Node son_node = syntax_tree.find_inferior_node(node_id, 0);
    if(son_node.type == "while"){
      returnList return_type = this->expression(cur_node.son[1]);
      if(return_type.empty() == false){
        if(return_type.type == "boolean")
          this->statement(cur_node.son[3]);
        else{
          this->result = false;
          cout << "[semantic error] row:" << this->syntax_tree.find_inferior_node(node_id, 0).line << ", the condition after the while should be a boolean expression" << endl;
        }
      }
    }
    else if(son_node.type == "read"){
      vector<returnList> var_list = this->variable_list(cur_node.son[3]);
      SymbolTableElement result_item;
      for(auto& var : var_list){
        result_item = this->symbol_table_controller.search_table(var.id_name, this->symbol_table_controller.current_table);
        if(result_item.empty() == true){
          this->result = false;
          cout << "[semantic error] row:" << var.row << " col:" << var.column << ", variable " << var.id_name << " is not defined." << endl;
        }
      }
    }
    else if(son_node.type == "write"){ // todo: complete this action
      vector<returnList> exp_list = this->expression_list(cur_node.son[3]);
    }
    else{
      cout << "[semantic error] error on current node token." << endl;
    }
  }

  // statement -> if expression then statement else_part
  else if(cur_node.son_num == 5){
    returnList return_type = this->expression(cur_node.son[1]);
    if(return_type.empty() == false){
      result = false;
      cout << "[semantic error] row:" << cur_node.line << ", expression cannot be empty." << endl;
    }
    else if(return_type.type != "boolean"){
      result = false;
      cout << "[semantic error] row:" << cur_node.line << ", the condition after if should be a boolean expression." << endl;
    }
    this->statement(cur_node.son[3]);
    this->else_part(cur_node.son[4]);
  }

  // statement -> for id assignop expression to expression do statement
  else if(cur_node.son_num == 8){
    Node son_node = this->syntax_tree.find_inferior_node(node_id, 1);
    returnList return_type_exp1 = this->expression(cur_node.son[3]);
    returnList return_type_exp2 = this->expression(cur_node.son[5]);

    // 存疑son_node.str_value
    SymbolTableElement result_item = this->symbol_table_controller.search_table(son_node.str_value, this->symbol_table_controller.current_table);
    if(result_item.empty() == true){
      result = false;
      cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", id " << son_node.str_value << " is not defined." << endl;
    }
    if(result_item.empty() == false && return_type_exp1.empty() == false && return_type_exp2.empty() == false){
      result_item.use.push_back(son_node.line);
      if(result_item.value_type == "integer" && return_type_exp1.type == "integer" && return_type_exp2.type == "integer")
        this->statement(cur_node.son[7]);
      else{
        this->result = false;
        cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", the type of the iteration variable in the for statement can only be int" << endl;
      }
    }
    else{
      this->result = false;
      cout << "[semantic error] row:" << cur_node.line << ", the statement is incomplete." << endl;
    }

  }

  else
    return;
}

vector<returnList> SemanticAnalyzer::variable_list(const int& node_id){
  vector<returnList> var_list;
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 1){
    var_list.push_back(this->variable(cur_node.son[0]));
  }
  else if(cur_node.son_num == 3){
    if(var_list.empty() == false){
      for(auto& var : this->variable_list(cur_node.son[0])){
        var_list.push_back(var);
      }
    }
    var_list.push_back(this->variable(cur_node.son[2]));
  }
  else{
    this->result = false;
    cout << "[semantic error] error on son number of current node." << endl;
  }
  return var_list;
}

returnList SemanticAnalyzer::variable(const int& node_id){

}

void SemanticAnalyzer::id_varpart(const int& node_id){

}
// procedure_call -> id | id ( expression_list )
void SemanticAnalyzer::procedure_call(const int& node_id){

}
  // else_part -> else statement | None
void SemanticAnalyzer::else_part(const int& node_id){

}
  // expression_list -> expression_list , expression | expression
vector<returnList> SemanticAnalyzer::expression_list(const int& node_id){

}
  // expression -> simple_expression relop simple_expression | simple_expression
returnList SemanticAnalyzer::expression(const int& node_id){

}
  // simple_expression -> simple_expression addop term | term
returnList SemanticAnalyzer::simple_expression(const int& node_id){

}
  // term -> term mulop factor | factor
// mulop的集合为{'*', '/', 'div', 'mod', 'and'}
returnList SemanticAnalyzer::term(const int& node_id){

}
  // factor -> num | variable | id ( expression_list ) | ( expression ) | not factor | uminus factor
returnList SemanticAnalyzer::factor(const int& node_id){
  
}


