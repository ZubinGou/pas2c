#include "semantic_analyzer.h"
#include "symbol_table.h"
#include <string>
#include <vector>
using namespace std;

// initialize the analyzer
SemanticAnalyzer::SemanticAnalyzer(const SyntaxTree& tree){//checked
  this->syntax_tree = tree;   //get the syntax tree
  this->symbol_table_controller = SymbolTableController();  //create the symbol table controller
  this->result = true;// there's no error before the analyse
  this->start_analyze();
}

void SemanticAnalyzer::start_analyze(){//checked
  //the root of the syntax tree must be "programstruct"
  if(this->syntax_tree.find_inferior_node(0,0).type == "programstruct")
    programstruct();
}

// return the type of an expression
string SemanticAnalyzer::get_exp_type(const int& node_id, const string& symbol_table_name){//finished
  this->controller.current_table = symbol_table_name;
  returnList result_item = this->expression(node_id);
  this->controller.current_table = nullptr;
  if(result_item.empty() == false)
    return result_item.type;
  else
    return nullptr;
}

//programstruct -> program_head ; program_body .
void SemanticAnalyzer::programstruct(void){//checked
  int cur_nodeId = this->syntax_tree.find_inferior_node(0, 0).id;//the id of root node
  Node cur_node = this->syntax_tree.find_inferior_node(0, 0);//the root node
  program_head(cur_node.son[0]);//program head is the first son
  if(syntax_tree.node_dic[cur_node.son[1]].type != ";"){// ; is the second son
    this->result = false;
  }
  program_body(cur_node.son[2]);//program body is the third son
  if(syntax_tree.node_dic[cur_node.son[3]].type != "."){// . is the forth son
    this->result = false;
  }
}


//program_head -> program id ( idlist ) | program id
void SemanticAnalyzer::program_head(const int& node_id){// bug alert
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if (cur_node.son_num == 2){
    if (this ->syntax_tree.find_inferior_node(node_id,0).type != "program" ){
      this->result = false;
    }
    if (this ->syntax_tree.find_inferior_node(node_id,1).type != "id" ){
      this->result = false;
    }
    else{
      std::vector<Argument> a;
      this->symbol_table_controller.create_table("main", false, "", a); //initalize the "main" table
    }
  }

  else if ( cur_node.son_num == 5 ){//program_head -> program id ( idlist )
    if (this ->syntax_tree.find_inferior_node(node_id,0).type != "program" ){
      this->result = false;
    }
    if (this ->syntax_tree.find_inferior_node(node_id,1).type != "id" ){
      this->result = false;
    }
    if (this ->syntax_tree.find_inferior_node(node_id,2).type != "(" ){
      this->result = false;
    }
    vector<returnList> agruments = this->idlist(cur_node.son[3]);//goto funtion "idlist" and get the agruements
  
    vector<Argument> params;
    for(auto& it : agruments){//iterate agruements
      Argument temp(it.id_name,nullptr,stoi(it.row));
      params.push_back(temp);
    }
    //"if not create table" in the original code seems to be unnecessary
    if(")" != this->syntax_tree.find_inferior_node(node_id,4).type){
      this->result = false;
    }
  }
  else{
  /* wrong number of inferior nodes
     There must be either 2 or 5 sons */
    this -> result = false;
    cout << "[semantic error] The number of the current inferior node is wrong!" << endl;
  }
}

// program_body-> const_declarations var_declarations subprogram_declarations compound_statement
void SemanticAnalyzer::program_body(const int& node_id){//checked
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if (this ->syntax_tree.find_inferior_node(node_id,0).type == "const_declarations" ){
      this->const_declarations(cur_node.son[0]);
  }
  if (this ->syntax_tree.find_inferior_node(node_id,1).type == "var_declarations" ){
      this->var_declarations(cur_node.son[1]);
  }
  if (this ->syntax_tree.find_inferior_node(node_id,2).type == "subprogram_declarations" ){
      this->subprogram_declarations(cur_node.son[2]);
  }
  if (this ->syntax_tree.find_inferior_node(node_id,3).type == "compound_statement" ){
      this->compound_statement(cur_node.son[3]);
    }
}

// idlist -> idlist, id | id
vector<returnList> SemanticAnalyzer::idlist(const int& node_id){//not finished
  vector<returnList> arguments;
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 1){
    if(this->syntax_tree.find_inferior_node(node_id,0).type == "id"){
      Node node_child = this->syntax_tree.find_inferior_node(node_id,0);
      //returnList current = (node_child.str_value,node_child.type,node_child.line,node_child.col,);
      /*
      std::string id_name, 
      std::string type, 
      std::string row,
      std::string column, 
      std::string value_type
      */
      //(node_child.str_value, node_child.type,node_child.line, node_child.col,)
    }
  }
  else if(cur_node.son_num == 3){

  }
  else{
    this -> result = false;
    std::cout<<"[semantic error] The number of the current inferior node is wrong!"<<endl;
  }
  return arguments;
}



// const_declarations -> const const_declaration ; | None
void SemanticAnalyzer::const_declarations(const int& node_id){//finished
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 2){
    Node node_child = this->syntax_tree.find_inferior_node(node_id,0);
    if(node_child.type=="const"){
      this->const_declaration(cur_node.son[1]);
    }
    else{
      cout<<"[semantic error] The number of the current inferior node is wrong!"<<endl;
    }
  }
}

//const_declaration -> const_declaration ; id relop const_value |  id relop const_value
void SemanticAnalyzer::const_declaration(const int& node_id){// not finished
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 3){

  }
  else if(cur_node.son_num == 5){

  }
  else{
    cout<<"[semantic error] The number of the current inferior node is wrong!" << endl;
  }
}


vector<string> SemanticAnalyzer::const_value(const int& node_id){//not finished
  string value_const;
  string type_const;

  if(this->syntax_tree.find_inferior_node(node_id,0).type == "addop"){

  }

  else if(this->syntax_tree.find_inferior_node(node_id,0).type == "num"){

  }
  else if(this->syntax_tree.find_inferior_node(node_id,0).type == "\'" && this->syntax_tree.find_inferior_node(node_id,2).type == "\'"){
    Node node_child = syntax_tree.find_inferior_node(node_id,1);
    value_const = node_child.str_value;
    if (1){
      type_const = "char";
    }
  }
  else{
    this->result = false;
    cout<<"[semantic error] The number of the current inferior node is wrong!"<<endl;
  }
  vector<string> tuple;
  tuple.push_back(value_const);
  tuple.push_back(type_const);
  return tuple;
}

void SemanticAnalyzer::var_declarations(const int& node_id){//finished
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 3){
    Node node_child = this->syntax_tree.find_inferior_node(node_id,0);
    if(node_child.type == "var"){
      this->var_declarations(cur_node.son[1]);
    }
    else{
      cout<<"[semantic error] The token of current node is wrong!"<<endl;
    }
  }
  else if(cur_node.son_num != 1){
    this->result = false;
    cout<<"[semantic error] The token of the son of the current node is wrong!"<<endl;
  }
}

void SemanticAnalyzer::var_declaration(const int& node_id){//not finished
  Node cur_node =this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 3){

  }
  else if(cur_node.son_num == 5){

  }
  else{
    this->result = false;
    cout<<"[semantic error] the number of the son of the current node is wrong!"<<endl;
  }
}

//type -> basic_type | array [ period ] of basic_type
void SemanticAnalyzer::type(const int nodeID){//not finished
  Result_type result_type;
  Node cur_node = this->syntax_tree.node_dic[nodeID];
  if(cur_node.son_num == 1){
    result_type.type = cur_node.son[0];
  }
  else if(cur_node.son_num == 6){
    vector<returnList> period_array = this->period(cur_node.son[2]);
    string type_array = this->basic_type(cur_node.son[5]);
    int size_array = 0;
    
  }
}

string SemanticAnalyzer::basic_type(const int& nodeID){//finished
  Node node_child = this->syntax_tree.find_inferior_node(nodeID,0);
  if(node_child.type == "integer" || node_child.type == "real" || node_child.type == "boolean" || node_child.type =="char"){
    return node_child.type;
  }
  else{
    cout<<"[semantic error] The token of the current node is wrong!"<<endl;
  }
}


vector<returnList> SemanticAnalyzer::period(const int& node_id){
  
}

void SemanticAnalyzer::subprogram_declarations(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num==3){
    subprogram_declarations(cur_node.son[0]);
    subprogram(cur_node.son[1]);
  }
  else if(cur_node.son_num!=1){
    this->result=false;
    cout<<"[semantic error] error on son number of current node."<<endl;
  }
}



void SemanticAnalyzer::subprogram(const int& node_id){
  //checked
  Node cur_node = this->syntax_tree.node_dic[node_id];
  subprogram_head(cur_node.son[0]);
  subprogram_body(cur_node.son[2]);
  this->symbol_table_controller.relocate_table();
}

void SemanticAnalyzer::subprogram_head(const int& node_id){//checked
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num==3){
    Node node_child = this->syntax_tree.find_inferior_node(node_id,1);
    string subprogram_name = node_child.str_value;
    vector<Argument> parameters = formal_parameter(cur_node.son[2]);
    this->symbol_table_controller.create_table(subprogram_name,false,"",parameters);
  }
  else if(cur_node.son_num==5){
    Node node_child = this->syntax_tree.find_inferior_node(node_id,1);
    string subprogram_name = node_child.str_value;
    string return_type = basic_type(cur_node.son[4]);
    vector<Argument> parameters = formal_parameter(cur_node.son[2]);
    this->symbol_table_controller.create_table(subprogram_name,false,return_type,parameters);
  }
  else{
    this->result = false;
    cout<<"[semantic error] error on son number of current node"<<endl;
  }

}

void SemanticAnalyzer::subprogram_body(const int& node_id){//checked
  Node cur_node = this->syntax_tree.node_dic[node_id];
  const_declarations(cur_node.son[0]);
  var_declarations(cur_node.son[1]);
  compound_statement(cur_node.son[2]);
}

vector<Argument> SemanticAnalyzer::formal_parameter(const int& node_id){// finished
  vector<Argument> parameters;
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 1){}
  else if(cur_node.son_num == 3){
    parameters = parameter_list(cur_node.son[1]);
  }
  else{
    cout<<"[semantic error] error on son number of current node"<<endl;
  }
  return parameters;
}

//--------------------------------------------------------------------------------
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
    sub_parameters = parameter(cur_node.son[2]);
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
    if(var_list.empty() == false){  // maybe wrong here, I feel strange but not sure
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
  returnList var;
  Node cur_node = this->syntax_tree.node_dic[node_id];
  Node son_id_node = this->syntax_tree.find_inferior_node(node_id, 0);
  Node son_varpart_ndoe = this->syntax_tree.find_inferior_node(node_id, 1);
  SymbolTableElement item = this->symbol_table_controller.search_table(son_id_node.str_value, this->symbol_table_controller.current_table);

  if(item.empty() == false){
    item.use.push_back(son_id_node.line);
    this->id_varpart(cur_node.son[1]);
    if(item.element_type == "array"){
      if(son_varpart_ndoe.son_num == 3){
        var = returnList(item.name, "array", to_string(son_id_node.line), to_string(son_id_node.col), item.value_type);
      }
      else{
        this->result = false;
        cout << "[semantic error] row:" << son_id_node.line << " col:" << son_id_node.col << " cannot operate on array name." << endl;
      }
    }
    else{
        var = returnList(item.name, item.value_type, to_string(son_id_node.line), to_string(son_id_node.col), "");
    }
  }
  else{
    this->result = false;
    cout << "[semantic error] row:" << son_id_node.line << " col:" << son_id_node.col << ", id " << son_id_node.str_value << " is not defined." << endl;
  }

  return var;
}

void SemanticAnalyzer::id_varpart(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 3){
    vector<returnList> exp_list = this->expression_list(cur_node.son[1]);
    if(exp_list.empty() == false){
      for(auto& exp : exp_list){
        if(exp.type != "integer"){
          this->result = false;
          cout << "[semantic error] row: " << this->syntax_tree.find_inferior_node(node_id, 0).line << ", array index should be integer." << endl;
        }
      }
    }
    else if(cur_node.son_num != 1){
      this->result = false;
      cout << "[semantic error] error on son number of current node." << endl;
    }
  }
}
// procedure_call -> id | id ( expression_list )
void SemanticAnalyzer::procedure_call(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 1){
    Node son_node = this->syntax_tree.find_inferior_node(node_id, 0);
    SymbolTableElement item = this->symbol_table_controller.search_table(son_node.str_value, this->symbol_table_controller.current_table);
    if(item.empty()){
      item.use.push_back(son_node.line);
      if(item.element_type == "procedure" || item.element_type == "function"){
        if(item.arguments_lists.size() != 0){
          this->result = false;
          cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", procedure/function need parameters." << endl;
        }
      }
      else{
        this->result = false;
        cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", Cannot be called as a procedure/function." << endl;
      }
    }
    else{
      this->result = false;
      cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", " << son_node.str_value << " is not defined." << endl;
    }
  }
  else if(cur_node.son_num == 4){
    Node son_node = this->syntax_tree.find_inferior_node(node_id, 0);
    SymbolTableElement item = this->symbol_table_controller.search_table(son_node.str_value, this->symbol_table_controller.current_table);

    if(item.empty() == false){
      item.use.push_back(son_node.line);
      if(item.element_type == "procedure" || item.element_type == "function"){
        if(item.arguments_lists.size() != 0){
          vector<returnList> exp_list = this->expression_list(cur_node.son[2]);
          if(exp_list.empty() == false){
            vector<string> args;
            for(auto& exp : exp_list){
              args.push_back(exp.type);
            }
            string return_type = this->symbol_table_controller.check_parameters(son_node.str_value, args);
            if(return_type == ""){
              this->result = false;
              cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", parameters do not match arguments." << endl; 
            }
          }
          else{
            this->result = false;
            cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", this procedure/function needs parameters." << endl; 
          }
        }
        else{
          this->result = false;
          cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", this procedure/function does not need parameters." << endl; 
        }
      }
      else{
        this->result = false;
        cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", " << son_node.str_value << "cannot use as procedure/function." << endl; 
      }
    }
    else{
      this->result = false;
      cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", " << son_node.str_value << " is not defined" << endl; 
    }
  }
  else{
    this->result = false;
    cout << "[semantic error] error on son number of current node." << endl;
  }
}
  // else_part -> else statement | None
void SemanticAnalyzer::else_part(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  if(cur_node.son_num == 2){
    this->statement(cur_node.son[1]);
  }
}
  // expression_list -> expression_list , expression | expression
vector<returnList> SemanticAnalyzer::expression_list(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  vector<returnList> exp_list;
  if(cur_node.son_num == 1){
    returnList exp = this->expression(cur_node.son[0]);
    if(exp.empty() == false){
      exp_list.push_back(exp);
    }
  }
  else if(cur_node.son_num == 3){
    exp_list = this->expression_list(cur_node.son[0]);
    returnList exp = this->expression(cur_node.son[2]);
    if(exp.empty() == false){
      exp_list.push_back(exp);
    }
  }
  else{
    this->result = false;
    cout << "[semantic error] error on son number of current node." << endl;
  }
  return exp_list;
}
  // expression -> simple_expression relop simple_expression | simple_expression

returnList SemanticAnalyzer::expression(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  returnList exp;

  if(cur_node.son_num == 1){
    exp = this->simple_expression(cur_node.son[0]);
  }
  else if(cur_node.son_num == 3){
    returnList exp1 = this->simple_expression(cur_node.son[0]);
    returnList exp2 = this->simple_expression(cur_node.son[2]);
    Node son_relop_node = this->syntax_tree.find_inferior_node(node_id, 1);

    if(exp1.empty() == false && exp2.empty() == false){
      if(exp1.type == exp2.type){
        if(exp1.type == "integer" || exp1.type == "real"){
          exp = returnList("expression", "boolean", to_string(son_relop_node.line), to_string(son_relop_node.col), "");
        }
        else{
          this->result = false;
          cout << "[semantic error] row: " << son_relop_node.line << ", this type of data cannot compare." << endl;
        }
      }
      else{
        this->result = false;
        cout << "[semantic error] row: " << son_relop_node.line << ", different type of data cannot compare." << endl;
      }
    }
  }
  return exp;
}
  // simple_expression -> simple_expression addop term | term
returnList SemanticAnalyzer::simple_expression(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  returnList sim_exp;

  if(cur_node.son_num == 1){
    sim_exp = this->term(cur_node.son[0]);
  }
  else if(cur_node.son_num == 3){
    returnList sim_exp_1 = this->simple_expression(cur_node.son[0]);
    returnList term = this->term(cur_node.son[2]);
    Node son_addop_node = this->syntax_tree.find_inferior_node(node_id, 1);

    if(sim_exp_1.empty() == false && term.empty() == false){
      if(sim_exp_1.type == term.type){  // exp type = term type
        if(son_addop_node.str_value == "or"){ // op is or
          if(sim_exp_1.type == "integer" || sim_exp_1.type == "boolean"){
            sim_exp = returnList("expression", sim_exp_1.type, to_string(son_addop_node.line), to_string(son_addop_node.col), "");
          }
          else{
            this->result = false;
            cout << "[semantic error] row: " << son_addop_node.line << ", type of expression does not match for operation or." << endl; 
          }
        }
        else{ // op is addop
          if(sim_exp_1.type == "integer" || sim_exp_1.type == "boolean"){
            sim_exp = returnList("expression", sim_exp_1.type, to_string(son_addop_node.line), to_string(son_addop_node.col), "");
          }
          else{
            this->result = false;
            cout << "[semantic error] row: " << son_addop_node.line << ", type of expression does not match for operation addop." << endl; 
          }
        }
      }
      else{ // exp type != term type
        if(son_addop_node.str_value == "or"){ // op is or
          this->result = false;
          cout << "[semantic error] row: " << son_addop_node.line << ", type of expression does not match for operation or." << endl; 
        }
        else{ // op is addop
          if((sim_exp_1.type == "integer" && term.type == "real") 
                    || (sim_exp_1.type == "real" && term.type == "integer"))
            sim_exp = returnList("expression", "real", to_string(son_addop_node.line), 
                                  to_string(son_addop_node.col), "");
          else{
            this->result = false;
            cout << "[semantic error] row: " << son_addop_node.line << ", type of expression does not match for operation addop." << endl; 
          }
        }
      }
    }
  }  
  else{
    this->result = false;
    cout << "[semantic error] error on son number of current node." << endl;
  }
  return sim_exp;
}
  // term -> term mulop factor | factor
  // mulop的集合为{'*', '/', 'div', 'mod', 'and'}
returnList SemanticAnalyzer::term(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  returnList term;

  if(cur_node.son_num == 1){
    term = this->factor(cur_node.son[0]);
  }
  else if(cur_node.son_num == 3){
    returnList sub_term = this->term(cur_node.son[0]);
    returnList sub_factor = this->factor(cur_node.son[2]);
    Node son_mulop_node = this->syntax_tree.find_inferior_node(node_id, 1);

    if(sub_term.empty() == false && sub_factor.empty() == false){
      if(sub_term.type == sub_factor.type){
        if(sub_term.type == "boolean"){
          if(son_mulop_node.str_value == "and")
            term = returnList("expression", "boolean", to_string(son_mulop_node.line), to_string(son_mulop_node.col), "");
          else{
            this->result = false;
            cout << "[semantic error] row: " << son_mulop_node.line << ", type of expression does not match for operation mulop." << endl; 
          }
        }
        else if (sub_term.type == "integer")
        {
          if(son_mulop_node.str_value == "and" || son_mulop_node.str_value == "div" || son_mulop_node.str_value == "*" || son_mulop_node.str_value == "mod")
            term = returnList("expression", "integer", to_string(son_mulop_node.line), to_string(son_mulop_node.col), "");
          else if(son_mulop_node.str_value == "/"){
            term = returnList("expression", "real", to_string(son_mulop_node.line), to_string(son_mulop_node.col), "");         
          }
          else{
            this->result = false;
            cout << "[semantic error] row: " << son_mulop_node.line << ", type of expression does not match for operation mulop." << endl; 
          }
        }
        else if (sub_term.type == "real")
        {
          if(son_mulop_node.str_value == "*" || son_mulop_node.str_value == "/")
            term = returnList("expression", "real", to_string(son_mulop_node.line), to_string(son_mulop_node.col), "");
          else{
            this->result = false;
            cout << "[semantic error] row: " << son_mulop_node.line << ", type of expression does not match for operation mulop." << endl; 
          }
        }
        else{
          this->result = false;
          cout << "[semantic error] row: " << son_mulop_node.line << ", type of expression does not match for operation mulop." << endl; 
        }
      }
      else{ // sub_term.type != sub_factor.type
        if((sub_term.type == "integer" && sub_factor.type == "real") 
              || (sub_term.type == "real" && sub_factor.type == "integer"))
          term = returnList("expression", "real", to_string(son_mulop_node.line), to_string(son_mulop_node.col), "");
        else{
          this->result = false;
          cout << "[semantic error] row: " << son_mulop_node.line << ", type of expression does not match for operation mulop." << endl; 
        }
      }
    }
  }
  else{
    this->result = false;
    cout << "[semantic error] error on son number of current node." << endl;
  }
  return term;
}

// factor -> num | variable | id ( expression_list ) | ( expression ) | not factor | uminus factor
returnList SemanticAnalyzer::factor(const int& node_id){
  Node cur_node = this->syntax_tree.node_dic[node_id];
  returnList factor;

  if(cur_node.son_num == 1){
    Node son_node = this->syntax_tree.find_inferior_node(node_id, 0);
    if(son_node.type == "num"){
      string son_type;
      switch(son_node.num_type){
        case 1:
          son_type = "integer";
          break;
        case 2:
          son_type = "real";
          break;
        case 3:
          son_type = "boolean";
          break;
        default:
          this->result = false;
          cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", the type is not basic type of Pascal." << endl;
          return factor;
      }
      factor = returnList("expression", son_type, to_string(son_node.line), to_string(son_node.col), "");
    }
    else if(son_node.type == "variable"){
      returnList var = this->variable(son_node.id);
      if(var.empty() == false){
        if(var.type == "array")
          factor = returnList(var.id_name, var.value_type, var.row, var.column, "");
        else
          factor = returnList(var.id_name, var.type, var.row, var.column, "");  
      }
    }
  }

  else if(cur_node.son_num == 2){
    Node son_node = this->syntax_tree.find_inferior_node(node_id, 0);
    if(son_node.type == "not"){
      returnList sub_factor = this->factor(cur_node.son[1]);
      if(sub_factor.empty() == false){
        if(sub_factor.type == "boolean" || sub_factor.type == "integer")
          factor = returnList("expression", sub_factor.type, to_string(son_node.line), to_string(son_node.col), "");
        else{
          this->result = false;
          cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", only boolean and integer can use not." << endl;
        }
      }
    }
    else if(son_node.type == "uminus"){
      returnList sub_factor = this->factor(cur_node.son[1]);
      if(sub_factor.empty() == false){
        if(sub_factor.type == "real" || sub_factor.type == "integer")
          factor = returnList("expression", sub_factor.type, to_string(son_node.line), to_string(son_node.col), "");
        else{
          this->result = false;
          cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", only real and integer can use uminus." << endl;
        }
      }
    }
  }

  else if(cur_node.son_num == 3){
    returnList exp = this->expression(cur_node.son[1]);
    if(exp.empty() == false)
      factor = returnList("expression", exp.type, exp.row, exp.column, "");
  }

  else if(cur_node.son_num == 4){
    Node son_node = this->syntax_tree.find_inferior_node(node_id, 0);
    SymbolTableElement item = this->symbol_table_controller.search_table(son_node.str_value, this->symbol_table_controller.current_table);

    if(item.empty() == false){
      item.use.push_back(son_node.line);
      if(item.element_type == "function" || item.element_type == "procedure"){
        vector<returnList> exp_list = this->expression_list(cur_node.son[2]);
        if(exp_list.empty() == false){
          vector<string> args;
          for(auto& exp : exp_list){
            args.push_back(exp.type);
          }
          string return_type = this->symbol_table_controller.check_parameters(son_node.type, args);
          if(return_type == ""){
            this->result = false;
            cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", parameters do not match." << endl;
          }
          else
            factor = returnList("expression", return_type, to_string(son_node.line), to_string(son_node.col), "");
        }
      }
      else{
        this->result = false;
        cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", this is neither a function nor a procedure." << endl;
      }
    }
    else{
      this->result = false;
      cout << "[semantic error] row:" << son_node.line << " col:" << son_node.col << ", is not defined." << endl;
    }
  } 
  else{
    this->result = false;
    cout << "[semantic error] error on son number of current node." << endl;
  }

  return factor;
}