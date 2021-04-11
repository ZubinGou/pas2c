#include "semantic_analyzer.h"
#include "symbol_table.h"
#include <string>
#include <vector>
using namespace std;

SemanticAnalyzer::SemanticAnalyzer(const SyntaxTree& tree){
  this->syntax_tree = tree;   //syntax tree
  this->result = true;
  this->start_analyze();
}

void SemanticAnalyzer::start_analyze(){
  if(this->syntax_tree.find_inferior_node(0,0).type == "programstruct")
    programstruct();
}

string SemanticAnalyzer::get_exp_type(const int& nodeid, const string& symbol_table_name){
  this->controller.current_table = symbol_table_name;
  std::vector<string> result_item  = this->expression(nodeid);
  this->controller.current_table = nullptr;
  if(result_item.empty() == false)
    return result_item[1];
  else{
    return nullptr;
  }

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