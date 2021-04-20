#include "code_generator.h"

using namespace std;

std::string CodeGenerator::run() {
  programstruct(1);
  add_indent();
  return this->target_code;
}

/*
  target_code manipulate
*/

void target_append(std::string code); // 部分代码到目标代码
void add_indent(); // 为目标代码添加缩进

/* 
  Parser interface
*/

std::vector<int> CodeGenerator::get_son(int node_id){} // return tree[node_id].son

int CodeGenerator::get_father(int node_id){} // return tree[node_id].father

void CodeGenerator::match(int node_id, std::string token){} // match current node and token

std::string CodeGenerator::get_token(int node_id){
  return tree[node_id].type;
}

std::string CodeGenerator::get_str_value(int node_id) {
  return tree[node_id].str_value;
}

double CodeGenerator::get_num_value(int node_id) {
  return tree[node_id].num_value;
}

NumType CodeGenerator::get_num_type(int node_id){
  return tree[node_id].num_type;
}

/* 
  Semantic intrface
*/

bool CodeGenerator::is_addr(std::string id) {
  return table_ctrl.is_addr(id, state_stack.top());
}

bool CodeGenerator::is_func(std::string id) {
  return table_ctrl.is_func(id);
}

std::string CodeGenerator::get_var_type(std::string id) {
  return table_ctrl.get_type(id, state_stack.top());
}

std::string CodeGenerator::get_exp_type(int node_id) {
  return table_anal.get_exp_type(node_id, state_stack.top());
}

std::vector<int> CodeGenerator::get_bound(std::string id) {
  std::vector<Argument> arguments_lists = table_ctrl.get_range(id, state_stack.top());
  std::vector<int> bound;
  for (auto argument : arguments_lists) {
    bound.push_back(argument.period_element.first);
  }
  return bound;
}

std::vector<bool> CodeGenerator::get_args(std::string id) {
  return table_ctrl.get_args(id);
}

/*
  Production
*/

// programstruct -> program_head ; program_body .
void CodeGenerator::programstruct(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();

  if (son_num == 4) {
    target_append("#include<stdio.h>\n");  // TODO:是否更改头文件解析方式？
    program_head(son[0]);
    program_body(son[2]);
    state_stack.pop();
  }
}

// program_head -> program id ( idlist ) | program id
void CodeGenerator::program_head(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();

  if (son_num == 5) {
    // id_list(son[3]);    # 程序头的id_list参数在C语言中不需要
  }
  else if (son_num == 2) {
    
  }
}

// program_body -> const_declarations var_declarations subprogram_declarations compound_statement
void CodeGenerator::program_body(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();

  if (son_num == 4) {
    const_declarations(son[0]);
    var_declarations(son[1]);
    subprogram_declarations(son[2]);
    compound_statement(son[3]);
  }
}

// idlist -> idlist , id | id
void CodeGenerator::idlist(int node_id, vector<int> id_num, string id_type="", bool id_addr=false) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();

  if (son_num == 3) {
    idlist(son[0], id_num, id_type, id_addr);
    
    string value = get_str_value(son[2]);
    if (id_addr)
      value = '*' + value;
    if (id_type == "")  // 声明在变量区
      target_append(", " + value);
    else  // 在参数区
      target_append(", " + id_type + " " + value);
    if (!id_num.empty())
      for (auto num : id_num)
        target_append("[" + to_string(num) + "]");
  }
  else if (son_num == 1) {
    string value = get_str_value(son[0]);
    if (id_addr)
      value = '*' + value;
    target_append(" " + value);
    if (!id_num.empty())
      for (auto num : id_num)
        target_append("[" + to_string(num) + "]");
  }
}

// const_declarations -> const const_declaration ; | e
void CodeGenerator::const_declarations(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    const_declaration(son[1]);
    target_append(";\n");
  }
}

// const_declaration -> const_declaration ; id = const_value | id = const_value 
void CodeGenerator::const_declaration(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 5) {
    const_declaration(son[0]);
    target_append(";");

    vector<string> type_value = const_value(son[4]);
    string id_type = type_value[0];
    string id_value = type_value[1];

    target_append("const " + id_type + " "); // 输出类型，例如 const int
    target_append(get_str_value(son[2]) + " = " + id_value);
  }
  else if (son_num == 3) {
    vector<string> type_value = const_value(son[2]);
    string id_type = type_value[0];
    string id_value = type_value[1];
    target_append("const " + id_type + " "); // 输出类型，例如 const int
    target_append(get_str_value(son[0]) + " = " + id_value);
  }
}

// const_value -> + num | - num | num | 'letter'
std::vector<std::string> CodeGenerator::const_value(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();

  string num_type_str[] = {"None", "int", "float", "bool"};
  std::vector<std::string> type_value;
  if (son_num == 2) {
    string op = get_str_value(son[0]); // + or -
    string num = to_string(get_num_value(son[1])); // 数字，包括整数和浮点数
    NumType num_type = get_num_type(son[1]);
    string id_type = num_type_str[num_type];

    type_value.push_back(id_type);
    type_value.push_back(op + num);
    return type_value;
  }
  else if (son_num == 1) {
    // TODO: 缺bool？
    string num = to_string(get_num_value(son[0]));
    NumType num_type = get_num_type(son[0]);
    string id_type = num_type_str[num_type];

    type_value.push_back(id_type);
    type_value.push_back(num);
    return type_value;
  }
  else if (son_num == 3) {
    string id_type = "char";
    string id_value = get_str_value(son[1]);

    type_value.push_back(id_type);
    type_value.push_back(id_value);
    return type_value;
  }
}

// var_declarations -> var var_declaration ; | e
void CodeGenerator::var_declarations(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    var_declaration(son[1]);
    target_append(";\n");
  }
}

// var_declaration -> var_declaration ; idlist : type | idlist : type
void CodeGenerator::var_declaration(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 5) {  // basic_type
    var_declaration(son[0]);
    target_append(";\n");

    pair<vector<string>, vector<int> > type_nums = type(son[4]);
    vector<string> id_type = type_nums.first;
    vector<int> id_num = type_nums.second;
    if (id_type[0] == "0"){  // 基本类型
      target_append(id_type[1]);  // int float bool char
      vector<int> none;
      idlist(son[2], none);
    }
    else {  // 数组类型
      target_append(id_type[1]);  // int float bool char
      idlist(son[2], id_num);
    }
  }
  else if (son_num == 3) {  // array [ period ] of basic_type
    // idlist : type
    pair<vector<string>, vector<int> > type_nums = type(son[2]);
    vector<string> id_type = type_nums.first;
    vector<int> id_num = type_nums.second;
    if (id_type[0] == "0") {  // 基本类型
      target_append(id_type[1]);  // int float bool char
      vector<int> none;
      idlist(son[0], none);
    }
    else {  // 数组类型
      target_append(id_type[1]);  // int float bool char
      idlist(son[0], id_num);  // 需要传递数组的大小
    }
  }
}

// type -> basic_type | array [ period ] of basic_type
pair<vector<string>, vector<int> > CodeGenerator::type(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  pair<vector<string>, vector<int> > ans;
  if (son_num == 1) {  // basic_type
    string id_type = basic_type(son[0]);
    ans.first.push_back("0");
    ans.first.push_back(id_type);
    return ans;
  }
  else if (son_num == 6) {  // array [ period ] of basic_type
    string id_type = basic_type(son[5]);  // 类型 int float bool char
    ans.first.push_back("1");
    ans.first.push_back(id_type);
    ans.second = period(son[2]);
    return ans;
  }
}

// basic_type -> integer | real | boolean | char
std::string CodeGenerator::basic_type(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 1) {
    map<string, string> type_pas2c;
    type_pas2c["integer"] = "int";
    type_pas2c["real"] = "float";
    type_pas2c["boolean"] = "bool";
    type_pas2c["char"] = "char";
    
    string var_type = get_str_value(son[0]);
    string res = "";
    if (type_pas2c.find(var_type) != type_pas2c.end())
      res = type_pas2c[var_type];
    return res; 
  }
}

// period -> period , digits .. digits | digits .. digits
std::vector<int> CodeGenerator::period(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    std::vector<int> nums;
    int left = get_num_value(son[0]);
    int right = get_num_value(son[2]);
    nums.push_back(right-left+1);
    return nums;  // 1..10 代表10个元素
  }
  else if (son_num == 5) {
    std::vector<int> nums = period(son[0]);
    int left = get_num_value(son[2]);
    int right = get_num_value(son[4]);
    nums.push_back(right-left+1);
    return nums;
  }
}

// subprogram_declarations -> subprogram_declarations subprogram ; | e
void CodeGenerator::subprogram_declarations(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    subprogram_declarations(son[0]);
    subprogram(son[1]);
  }
}

// subprogram -> subprogram_head ; subprogram_body
void CodeGenerator::subprogram(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    subprogram_head(son[0]);
    subprogram_body(son[2]);
    state_stack.pop();
  }
}

// subprogram_head -> procedure id formal_parameter | function id formal_parameter : basic_type 
void CodeGenerator::subprogram_head(int node_id) {  // 函数头 Pascal-S的过程为没有返回值的函数
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {  // procedure
    target_append("void");  // void
    string value = get_str_value(son[1]);
    state_stack.push(value);
    target_append(" " + value);  // 函数名 gcd

    formal_parameter(son[2]);
  }
  else if (son_num == 5) {  // function
    string id_type = basic_type(son[4]);  // int float bool char
    target_append(id_type);  // int
    string id_value = get_str_value(son[1]);
    state_stack.push(id_value);
    target_append(" " + id_value);  // 函数名 gcd

    formal_parameter(son[2]);  // 参数
  }
}

// formal_parameter -> ( parameter_list ) | e
void CodeGenerator::formal_parameter(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    target_append("(");
    parameter_list(son[1]);
    target_append(")");
  }
  else if (son_num == 1) {
    target_append("()");
  }
}

// parameter_list -> parameter_list ; parameter | parameter
void CodeGenerator::parameter_list(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    parameter_list(son[0]);
    target_append(", ");  // 逗号分隔符
    parameter(son[2]);
  }
  else if (son_num == 1) {
    parameter(son[0]);
  }
}

// parameter -> var_parameter | value_parameter
void CodeGenerator::parameter(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 1) {
    string token = get_token(son[0]);
    if (token == "var_parameter")  // 传地址
        var_parameter(son[0]);
    else  // 传值
        value_parameter(son[0]);
  }
}
// var_parameter -> var value_parameter
void CodeGenerator::var_parameter(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 2) {
    value_parameter(son[1]);
  }
}
// value_parameter -> idlist : basic_type
void CodeGenerator::value_parameter(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    string id_type = basic_type(son[2]);  // 基本类型 如 int
    target_append(id_type);  // int

    int parent = get_father(node_id);
    string token = get_token(parent);
    vector<int> none;
    if (token == "var_parameter")
        idlist(son[0], none, id_type, true);  // 参数 id,id,id
    else
        idlist(son[0], none, id_type);
  }
}