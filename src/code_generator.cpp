#include "code_generator.h"

#include "assert.h"

using namespace std;

string join_vec(vector<string> vec, string sep) {
  string res;
  if (vec.size() > 0) res += vec[0];
  for (int i = 1; i < vec.size(); i++) res += sep + vec[i];
  return res;
}

std::string CodeGenerator::run() {
  programstruct(1);
  add_indent();
  return this->target_code;
}

/*
  target_code manipulate
*/
void CodeGenerator::target_append(std::string code) {  // 部分代码到目标代码
  this->target_code += code;
}

void CodeGenerator::add_indent() {  // 为目标代码添加缩进
  stringstream sstream(target_code);
  target_code = "";
  int space_len = 0;
  string line;
  string content;
  while (sstream) {
    getline(sstream, line);
    if (line == "") continue;
    content = "";
    for (int i = 0; i < space_len; i++) content += "    ";
    content += line;

    if (line[0] == '{' || line[line.size() - 1] == '{')
      space_len += 1;
    else if (line[0] == '}' || line[line.size() - 1] == '}') {
      space_len -= 1;
      content = "";
      for (int i = 0; i < space_len; i++) content += "    ";
      content += line;
    }
    target_code += content + "\n";
  }
}

/*
 Debug
 */
void CodeGenerator::print_target_code() { cout << this->target_code; }

/*
  Parser interface
*/

std::vector<int> CodeGenerator::get_son(int node_id) {
  return this->tree[node_id].son;
}

int CodeGenerator::get_father(int node_id) {
  return this->tree[node_id].father;
}

void CodeGenerator::match(int node_id, std::string token) {
  assert(this->tree[node_id].type == token);
}  // match current node and token

std::string CodeGenerator::get_token(int node_id) { return tree[node_id].type; }

std::string CodeGenerator::get_str_value(int node_id) {
  return tree[node_id].str_value;
}

double CodeGenerator::get_num_value(int node_id) {
  return tree[node_id].num_value;
}

NumType CodeGenerator::get_num_type(int node_id) {
  return tree[node_id].num_type;
}

/*
  Semantic intrface
*/

bool CodeGenerator::is_addr(std::string id) {
  return table_ctrl.is_addr(id, state_stack.top());
}

bool CodeGenerator::is_func(std::string id) { return table_ctrl.is_func(id); }

std::string CodeGenerator::get_var_type(std::string id) {
  return table_ctrl.get_type(id, state_stack.top());
}

std::string CodeGenerator::get_exp_type(int node_id) {
  return table_anal.get_exp_type(node_id, state_stack.top());
}

std::vector<int> CodeGenerator::get_bound(std::string id) {
  std::vector<Argument> arguments_lists =
      table_ctrl.get_range(id, state_stack.top());
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
    // id_list(son[3]);    // 程序头的id_list参数在C语言中不需要
  } else if (son_num == 2) {
  }
}

// program_body -> const_declarations var_declarations subprogram_declarations
// compound_statement
void CodeGenerator::program_body(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();

  if (son_num == 4) {
    state_stack.push("main");
    const_declarations(son[0]);
    var_declarations(son[1]);
    subprogram_declarations(son[2]);
    compound_statement(son[3]);
  }
}

// idlist -> idlist , id | id
void CodeGenerator::idlist(int node_id, vector<int> id_num, string id_type,
                           bool id_addr) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();

  if (son_num == 3) {
    idlist(son[0], id_num, id_type, id_addr);

    string value = get_str_value(son[2]);
    if (id_addr) value = "*" + value;
    if (id_type == "")  // 声明在变量区
      target_append(", " + value);
    else  // 在参数区
      target_append(", " + id_type + " " + value);
    if (!id_num.empty())
      for (auto num : id_num) target_append("[" + to_string(num) + "]");
  } else if (son_num == 1) {
    string value = get_str_value(son[0]);
    if (id_addr) value = "*" + value;
    target_append(" " + value);
    if (!id_num.empty())
      for (auto num : id_num) target_append("[" + to_string(num) + "]");
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

    target_append("const " + id_type + " ");  // 输出类型，例如 const int
    target_append(get_str_value(son[2]) + " = " + id_value);
  } else if (son_num == 3) {
    vector<string> type_value = const_value(son[2]);
    string id_type = type_value[0];
    string id_value = type_value[1];
    target_append("const " + id_type + " ");  // 输出类型，例如 const int
    target_append(get_str_value(son[0]) + " = " + id_value);
  }
}

// const_value -> + num | - num | num | "letter"
std::vector<std::string> CodeGenerator::const_value(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();

  string num_type_str[] = {"None", "int", "float", "bool"};
  std::vector<std::string> type_value;
  if (son_num == 2) {
    string op = get_str_value(son[0]);              // + or -
    string num = to_string(get_num_value(son[1]));  // 数字，包括整数和浮点数
    NumType num_type = get_num_type(son[1]);
    string id_type = num_type_str[num_type];

    type_value.push_back(id_type);
    type_value.push_back(op + num);
    return type_value;
  } else if (son_num == 1) {
    double val = get_num_value(son[0]);
    NumType num_type = get_num_type(son[0]);
    string id_type = num_type_str[num_type];
    string num;
    if (id_type == "int")
      num = to_string(int(val));
    else if (id_type == "float")
      num = to_string(val);
    else if (id_type == "bool")
      num = val ? "true" : "false";

    type_value.push_back(id_type);
    type_value.push_back(num);
    return type_value;
  } else if (son_num == 3) {
    string id_type = "char";
    string id_value = get_str_value(son[1]);

    type_value.push_back(id_type);
    type_value.push_back("'" + id_value + "'");
    return type_value;
  } else
    cerr << "Unexpected Expression" << endl;
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

    pair<vector<string>, vector<int>> type_nums = _type(son[4]);
    vector<string> id_type = type_nums.first;
    vector<int> id_num = type_nums.second;
    if (id_type[0] == "0") {      // 基本类型
      target_append(id_type[1]);  // int float bool char
      vector<int> none;
      idlist(son[2], none);
    } else {                      // 数组类型
      target_append(id_type[1]);  // int float bool char
      idlist(son[2], id_num);
    }
  } else if (son_num == 3) {  // array [ period ] of basic_type
    // idlist : type
    pair<vector<string>, vector<int>> type_nums = _type(son[2]);
    vector<string> id_type = type_nums.first;
    vector<int> id_num = type_nums.second;
    if (id_type[0] == "0") {      // 基本类型
      target_append(id_type[1]);  // int float bool char
      vector<int> none;
      idlist(son[0], none);
    } else {                      // 数组类型
      target_append(id_type[1]);  // int float bool char
      idlist(son[0], id_num);     // 需要传递数组的大小
    }
  }
}

// type -> basic_type | array [ period ] of basic_type
pair<vector<string>, vector<int>> CodeGenerator::_type(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  pair<vector<string>, vector<int>> ans;
  if (son_num == 1) {  // basic_type
    string id_type = basic_type(son[0]);
    ans.first.push_back("0");
    ans.first.push_back(id_type);
    return ans;
  } else if (son_num == 6) {              // array [ period ] of basic_type
    string id_type = basic_type(son[5]);  // 类型 int float bool char
    ans.first.push_back("1");
    ans.first.push_back(id_type);
    ans.second = period(son[2]);
    return ans;
  } else
    cerr << "Unexpected Expression" << endl;
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
  } else
    cerr << "Unexpected Expression" << endl;
}

// period -> period , digits .. digits | digits .. digits
std::vector<int> CodeGenerator::period(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    std::vector<int> nums;
    int left = get_num_value(son[0]);
    int right = get_num_value(son[2]);
    nums.push_back(right - left + 1);
    return nums;  // 1..10 代表10个元素
  } else if (son_num == 5) {
    std::vector<int> nums = period(son[0]);
    int left = get_num_value(son[2]);
    int right = get_num_value(son[4]);
    nums.push_back(right - left + 1);
    return nums;
  } else
    cerr << "Unexpected Expression" << endl;
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

// subprogram_head -> procedure id formal_parameter | function id
// formal_parameter : basic_type
void CodeGenerator::subprogram_head(
    int node_id) {  // 函数头 Pascal-S的过程为没有返回值的函数
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {       // procedure
    target_append("void");  // void
    string value = get_str_value(son[1]);
    state_stack.push(value);
    target_append(" " + value);  // 函数名 gcd

    formal_parameter(son[2]);
  } else if (son_num == 5) {              // function
    string id_type = basic_type(son[4]);  // int float bool char
    target_append(id_type);               // int
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
  } else if (son_num == 1) {
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
  } else if (son_num == 1) {
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
    target_append(id_type);               // int

    int father = get_father(node_id);
    string token = get_token(father);
    vector<int> none;
    if (token == "var_parameter")
      idlist(son[0], none, id_type, true);  // 参数 id,id,id
    else
      idlist(son[0], none, id_type);
  }
}

// subprogram_body -> const_declarations var_declarations compound_statement
void CodeGenerator::subprogram_body(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    // parameters
    target_append(" {\n");
    const_declaration(son[0]);
    var_declaration(son[1]);
    // function
    compound_statement(son[2]);
    target_append("}\n");
  } else
    cerr << "Unexpected Expression" << endl;
}

// compound_statement -> begin statement_list end
void CodeGenerator::compound_statement(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  int father = get_father(node_id);
  if (son_num == 3) {
    if (this->tree[father].type == "subprogram_body") {
      match(son[0], "begin");
      statement_list(son[1]);
      match(son[2], "end");
    } else if (this->tree[father].type == "program_body") {
      target_append("int main()");
      match(son[0], "begin");
      target_append(" {\n");
      statement_list(son[1]);
      match(son[2], "end");
      target_append("\nreturn 0; \n}\n");
    } else {  // 一般的符合语句要加大括号
      match(son[0], "begin");
      target_append(" {\n");
      statement_list(son[1]);
      match(son[2], "end");
      target_append("}\n");
    }
  } else
    cerr << "Unexpected Expression" << endl;
}
// statement_list -> statement_list ; statement
//                 | statement
void CodeGenerator::statement_list(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    statement_list(son[0]);
    match(son[1], ";");
    statement(son[2]);
  } else if (son_num == 1) {
    statement(son[0]);
  } else
    cerr << "Unexpected Expression" << endl;
}
// statement -> variable assignop expression
//            | procedure_call
//            | compound_statement
//            | if expression then statement else_part
//            | for id assignop expression to expression do statement
//            | read ( variable_list )
//            | write ( expression_list )
//            | e
void CodeGenerator::statement(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  // pascal的赋值语句比较复杂
  if (son_num == 3) {  // variable assignop expression
    pair<string, string> var_pair = variable(son[0]);
    string var_type = var_pair.first;
    string var = var_pair.second;
    // cout << var_type << " " << var << " " << cur_state[-1] << endl;

    match(son[1], "assignop");
    if (var == state_stack.top())  // 返回语句
      target_append("return ");
    else {
      target_append(var);
      target_append(" = ");
    }

    string exp = expression(son[2]);
    target_append(exp);
    target_append(";\n");
  }

  else if (son_num == 1) {
    if (tree[son[0]].type == "procedure_call") {
      procedure_call(son[0]);
      target_append(";\n");
    } else if (tree[son[0]].type == "compound_statement") {
      compound_statement(son[0]);
      target_append("\n");
    } else {                   // 空
      match(son[0], nullptr);  // TODO nullptr?
      // target_append(";\n")
    }
  }

  else if (son_num == 5) {  // if expression then statement else_part
    match(son[0], "if");
    target_append("if (");

    string exp = expression(son[1]);
    target_append(exp);
    target_append(")");

    match(son[2], "then");
    vector<int> c = get_son(son[3]);
    int cn = c.size();
    if (!(cn == 1 && tree[c[0]].type == "compound_statement")) c.clear();
    if (c.empty()) target_append(" {\n");
    statement(son[3]);
    if (c.empty()) target_append("}\n");
    else_part(son[4]);
  }

  else if (son_num == 8) {
    // for id assignop expression to expression do statement
    match(son[0], "for");
    target_append("for (");
    match(son[1], "id");

    if (is_addr(tree[son[1]].str_value))
      target_append("*" + tree[son[1]].str_value);
    else
      target_append(tree[son[1]].str_value);  // TODO num or str?

    match(son[2], "assignop");
    target_append("=");
    target_append(expression(son[3]));
    target_append(";");

    match(son[4], "to");

    target_append(tree[son[1]].str_value);
    target_append("<=");
    target_append(expression(son[5]));

    match(son[6], "do");
    target_append(";");

    target_append(tree[son[1]].str_value);
    target_append("++) {\n");
    statement(son[7]);
    target_append("}\n");
  } else if (son_num == 4) {
    //           | while expression do statement
    //           | read ( variable_list )
    //           | write ( expression_list )
    if (tree[son[0]].type == "read") {
      match(son[0], "read");
      target_append("scanf");

      match(son[1], "(");
      target_append("(");

      vector<pair<string, string>> vlist;
      variable_list(son[2], vlist);
      target_append("\"");
      for (auto p : vlist) {
        string var_type = p.first;
        if (var_type == "boolean") target_append("%d");
        if (var_type == "integer") target_append("%d");
        if (var_type == "char") target_append("%c");
        if (var_type == "read") target_append("%f");
      }
      target_append("\"");

      vector<string> value_list;
      target_append(", ");
      for (auto p : vlist) value_list.push_back("&" + p.second);
      target_append(join_vec(value_list, ", "));

      match(son[3], ")");
      target_append(");\n");
    }
    if (tree[son[0]].type == "write") {
      match(son[0], "write");
      target_append("printf");
      match(son[1], "(");

      target_append("(");
      target_append("\"");

      // TODO 这里要加入判断表达值类型的函数
      vector<string> elist;
      vector<string> tlist;
      expression_list(son[2], elist, tlist);

      vector<string> trans_tlist;
      for (auto etype : tlist) {
        if (etype == "integer" || etype == "boolean")
          trans_tlist.push_back("%d");
        if (etype == "real") trans_tlist.push_back("%f");
        if (etype == "char") trans_tlist.push_back("%c");
      }

      target_append(join_vec(trans_tlist, " "));
      target_append("\"");
      target_append(", ");

      target_append(join_vec(elist, ", "));
      match(son[3], ")");
      target_append(");\n");
    }

    if (tree[son[0]].type == "while") {
      // while expression do statement
      match(son[0], "while");
      target_append("while (");
      target_append(expression(son[1]));
      target_append(")");
      match(son[2], "do");
      statement(son[3]);
    }
  } else
    cerr << "Unexpected Expression" << endl;
}
// variable_list -> variable_list , variable
//                | variable
void CodeGenerator::variable_list(
    int node_id, std::vector<std::pair<string, string>>& vlist) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {  // variable_list , variable
    variable_list(son[0], vlist);
    match(son[1], ",");
    vlist.push_back(variable(son[2]));
  } else if (son_num == 1)  // variable
    vlist.push_back(variable(son[0]));
  else
    cerr << "Unexpected Expression" << endl;
}

// variable -> id id_varpart
std::pair<string, string> CodeGenerator::variable(int node_id, bool* is_bool) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 2) {  // id id_varpart
    match(son[0], "id");
    string var_type = get_var_type(tree[son[0]].str_value);
    if (var_type == "boolean") *is_bool = true;
    string var_part = id_varpart(son[1]);
    // 加入一个元组, (var的type, var) eg:(int, a[1][2])
    string has_ptr = is_addr(tree[son[0]].str_value) ? "*" : "";
    // is_func = "_re" if is_func(tree[son[0]].value) else ""
    string is_func = "";
    return {var_type, has_ptr + tree[son[0]].str_value + is_func + var_part};
  } else
    cerr << "Unexpected Expression" << endl;
}

// id_varpart -> [ expression_list ] | e
std::string CodeGenerator::id_varpart(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {  // [ expression_list ]
    match(son[0], "[");
    vector<string> elist, tlist;  // expression list
    expression_list(son[1], elist, tlist);
    match(son[2], "]");

    int father = get_father(node_id);
    int id = tree[father].son[0];
    string id_value = tree[id].str_value;
    vector<int> blist = get_bound(id_value);  // bound的list

    // elist_trans = ["[{}{}]".format(exp, "-" + str(bound)) for exp, bound in
    // zip(elist, blist) ]
    vector<string> elist_trans;
    for (int i = 0; i < elist.size(); i++)
      elist_trans.push_back(elist[i] + "-" + to_string(blist[i]));
    return join_vec(elist_trans, "");
  } else if (son_num == 1) {  // 空
    // TODO:更改match类型
    // match(son[0], nullptr);
    return "";
  }
}

// procedure_call -> id | id ( expression_list )
void CodeGenerator::procedure_call(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 1) {  // id 没有参数
    match(son[0], "id");
    target_append(tree[son[0]].str_value);
    target_append("()");
  } else if (son_num == 4) {  // id ( expression_list )
    // 有参数
    match(son[0], "id");
    target_append(tree[son[0]].str_value);

    if (is_func(tree[son[0]].str_value)) {
      vector<bool> args_list = get_args(tree[son[0]].str_value);
      // cout << args_list; // TODO
    }

    match(son[1], "(");
    target_append("(");

    vector<string> elist, tlist;
    expression_list(son[2], elist, tlist);
    target_append(join_vec(elist, ", "));

    match(son[3], ")");
    target_append(")");
  } else
    cerr << "Unexpected Expression" << endl;
}

// else_part -> else statement | e
void CodeGenerator::else_part(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 2) {  // else statement
    match(son[0], "else");
    target_append("else {\n");

    statement(son[1]);
    // target_append(";");
    target_append("}\n");
  } else if (son_num == 1)  // 空
    match(son[0], nullptr);
  else
    cerr << "Unexpected Expression" << endl;
}

// expression_list -> expression_list , expression
//                  | expression
void CodeGenerator::expression_list(int node_id,
                                    std::vector<std::string>& elist,
                                    std::vector<std::string>& tlist) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {  // expression_list , expression
    expression_list(son[0], elist, tlist);
    match(son[1], ",");
    elist.push_back(expression(son[2]));
    tlist.push_back(get_exp_type(son[2]));
  } else if (son_num == 1) {  // expression
    elist.push_back(expression(son[0]));
    tlist.push_back(get_exp_type(son[0]));
  } else
    cerr << "Unexpected Expression" << endl;
}

// expression -> simple_expression relop simple_expression
//             | simple_expression
std::string CodeGenerator::expression(
    int node_id, bool* is_bool) {  // TODO pass by reference
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (is_bool) *is_bool = false;
  if (son_num == 3) {  // simple_expression relop simple_expression
    if (is_bool) *is_bool = true;
    string front_exp = simple_expression(son[0]);
    match(son[1], "relop");
    string relop = tree[son[1]].str_value;
    if (relop == "<>") relop = "!=";
    if (relop == "=") relop = "==";
    string back_exp = simple_expression(son[2]);
    return front_exp + relop + back_exp;
  } else if (son_num == 1)  // simple_expression
    return simple_expression(son[0]);
  else
    cerr << "Unexpected Expression" << endl;
}

// simple_expression -> simple_expression addop term
//                    | term
string CodeGenerator::simple_expression(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {  // simple_expression addop term
    string exp = simple_expression(son[0]);
    match(son[1], "addop");
    string addop = tree[son[1]].str_value;
    if (addop == "or") addop = "|";
    string _term = term(son[2]);
    return exp + addop + _term;
  } else if (son_num == 1)  // term
    return term(son[0]);
  else
    cerr << "Unexpected Expression" << endl;
}

// term -> term mulop factor
//       | factor
string CodeGenerator::term(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {  // term mulop factor
    string _term = term(son[0]);

    match(son[1], "mulop");
    string mulop = tree[son[1]].str_value;
    if (mulop == "div") mulop = "/";
    if (mulop == "mod") mulop = "%";
    if (mulop == "and") mulop = "&&";
    string _factor = factor(son[2]);
    return _term + mulop + _factor;
  } else if (son_num == 1) {  // factor
    string _factor = factor(son[0]);
    return _factor;
  } else
    cerr << "Unexpected Expression" << endl;
}

// factor -> num
//         | variable
//         | id ( expression_list )
//         | ( expression )
//         | not factor
//         | uminus factor
string CodeGenerator::factor(int node_id, bool* is_bool) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 1) {  // num | variable
    if (tree[son[0]].type == "num") {
      double val = tree[son[0]].num_value;
      // TODO: add bool
      if (tree[son[0]].num_type == NumType::Integer)
        return to_string(int(val));
      else if (tree[son[0]].num_type == NumType::Real)
        return to_string(val);
    }
    if (tree[son[0]].type == "variable") {
      pair<string, string> var_pair = variable(son[0]);
      return var_pair.second;
    }
  }
  if (son_num == 4) {  // id ( expression_list )
    match(son[0], "id");
    match(son[1], "(");

    vector<bool> is_addr_list;
    if (is_func(tree[son[0]].str_value))
      is_addr_list = get_args(tree[son[0]].str_value);  // TODO

    vector<string> elist, tlist;
    expression_list(son[2], elist, tlist);
    match(son[3], ")");

    vector<string> args_list;
    for (int i = 0; i < elist.size(); i++) {
      string exp = elist[i];
      bool is_addr = false;
      if (is_addr_list.size()) is_addr_list[i];
      if (is_addr)
        args_list.push_back("&" + exp);
      else
        args_list.push_back(exp);
    }
    return tree[son[0]].str_value + "(" + join_vec(args_list, ", ") + ")";
  }
  if (son_num == 3) {  // ( expression )
    match(son[0], "(");
    string exp = expression(son[1], is_bool);
    match(son[2], ")");
    return "(" + exp + ")";
  }
  if (son_num == 2) {  // not factor | uminus factor
    if (tree[son[0]].type == "not") {
      bool is_bool_var = false;
      string _factor = factor(son[1], &is_bool_var);
      if (is_bool_var)
        return "!" + _factor;
      else
        return "~" + _factor;
    }
    if (tree[son[0]].type == "uminus") {
      match(son[0], "uminus");
      string _factor = factor(son[1]);
      return "-" + _factor;
    }
  } else
    cerr << "Unexpected Expression" << endl;
}