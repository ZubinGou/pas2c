#include "spdlog/spdlog.h"
#include "code_generator.h"
#include "assert.h"

using namespace std;

string join_vec(vector<string> vec, string sep) {
  string res;
  if (vec.size() > 0) res += vec[0];
  for (int i = 1; i < (int)vec.size(); i++) res += sep + vec[i];
  return res;
}

std::string CodeGenerator::run() {
  programstruct(1);
  add_head_file();
  add_indent();
  return this->target_code;
}

/*
  target_code manipulate
 */
void CodeGenerator::target_append(std::string code) {
  this->target_code += code;
}

void CodeGenerator::add_indent() {
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

void CodeGenerator::add_head_file() {
  for (auto item : head_file) {
    target_code = item + target_code;
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
  // spdlog::debug("match: {} <-> {}", this->tree[node_id].type, token);
  assert(this->tree[node_id].type == token);
}

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
    program_head(son[0]);
    program_body(son[2]);
    state_stack.pop();
  } else
    spdlog::error("Unexpected Expression[0]");
}

// program_head -> program id ( idlist ) | program id
void CodeGenerator::program_head(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  // pascal's program_head is not needed in C
  if (son_num == 5) {}
  else if (son_num == 2) {}
  else spdlog::error("Unexpected Expression[1]");
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
  } else
    spdlog::error("Unexpected Expression[2]");
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
    if (id_type == "")  // 变量声明
      target_append(", " + value);
    else  // 参数列表
      target_append(", " + id_type + " " + value);
    if (!id_num.empty())
      for (auto num : id_num) target_append("[" + to_string(num) + "]");
  } else if (son_num == 1) {
    string value = get_str_value(son[0]);
    if (id_addr) value = "*" + value;
    target_append(" " + value);
    if (!id_num.empty())
      for (auto num : id_num) target_append("[" + to_string(num) + "]");
  } else
    spdlog::error("Unexpected Expression[3]");
}

// const_declarations -> const const_declaration ; | e
void CodeGenerator::const_declarations(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    const_declaration(son[1]);
    target_append(";\n");
  } else if (son_num == 1) {
  } else
    spdlog::error("Unexpected Expression[4]");
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

    target_append("const " + id_type + " ");
    target_append(get_str_value(son[2]) + " = " + id_value);
  } else if (son_num == 3) {
    vector<string> type_value = const_value(son[2]);
    string id_type = type_value[0];
    string id_value = type_value[1];
    target_append("const " + id_type + " ");
    target_append(get_str_value(son[0]) + " = " + id_value);
  } else
    spdlog::error("Unexpected Expression[5]");
}

// const_value -> + num | - num | num | "letter"
std::vector<std::string> CodeGenerator::const_value(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  string num_type_str[] = {"None", "int", "float", "bool"};
  std::vector<std::string> type_value;
  if (son_num == 2) {
    string op = get_str_value(son[0]);  // + or -
    string num = to_string(get_num_value(son[1]));
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
    else if (id_type == "bool"){
      num = val ? "true" : "false";
    }

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
    spdlog::error("Unexpected Expression[6]");
}

// var_declarations -> var var_declaration ; | e
void CodeGenerator::var_declarations(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    var_declaration(son[1]);
    target_append(";\n");
  } 
    // spdlog::error("Unexpected Expression[7]");
}

// var_declaration -> var_declaration ; idlist : type | idlist : type
void CodeGenerator::var_declaration(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 5) {
    var_declaration(son[0]);
    target_append(";\n");

    pair<vector<string>, vector<int>> type_nums = _type(son[4]);
    vector<string> id_type = type_nums.first;
    vector<int> id_num = type_nums.second;
    if (id_type[0] == "0") {  // basic type
      target_append(id_type[1]);
      vector<int> none;
      idlist(son[2], none);
    } else if (id_type[0] == "1") {  // array type
      target_append(id_type[1]);
      idlist(son[2], id_num);
    } else if (id_type[0] == "2") {  // record type
      vector<int> none;
      idlist(son[0], none);
    } else
    spdlog::error("[Unexpected Expression[8]");
  } else if (son_num == 3) {  // idlist : type
    pair<vector<string>, vector<int>> type_nums = _type(son[2]);
    vector<string> id_type = type_nums.first;
    vector<int> id_num = type_nums.second;
    if (id_type[0] == "0") {  // basic type
      target_append(id_type[1]);
      vector<int> none;
      idlist(son[0], none);
    } else if (id_type[0] == "1") {  // array type
      target_append(id_type[1]);
      idlist(son[0], id_num);
    } else if (id_type[0] == "2") {  // record type
      vector<int> none;
      idlist(son[0], none);
    } else
    spdlog::error("[Unexpected Expression[8]");
  } else
    spdlog::error("Unexpected Expression[8]");
}

// type -> basic_type | array [ period ] of basic_type ｜ record_type
pair<vector<string>, vector<int>> CodeGenerator::_type(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  pair<vector<string>, vector<int>> ans;
  if (son_num == 1) { 
    string var_type = get_token(son[0]);
    if (var_type == "basic_type") {
      string id_type = basic_type(son[0]);
      ans.first.push_back("0");
      ans.first.push_back(id_type);
      return ans;
    } else if (var_type == "record_type") {
      record_type(son[0]);
      ans.first.push_back("2");
      return ans;
    } else{ 
      spdlog::error("Unexpected Expression[9]");
    }
  } else if (son_num == 6) { 
    string id_type = basic_type(son[5]);
    ans.first.push_back("1");
    ans.first.push_back(id_type);
    ans.second = period(son[2]);
    return ans;
  } else
    spdlog::error("Unexpected Expression[9]");
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
    if (res == "bool")
      head_file.insert("#include <stdbool.h>\n");
    return res;
  } else
    spdlog::error("Unexpected Expression[10]");
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
    return nums;
  } else if (son_num == 5) {
    std::vector<int> nums = period(son[0]);
    int left = get_num_value(son[2]);
    int right = get_num_value(son[4]);
    nums.push_back(right - left + 1);
    return nums;
  } else
    spdlog::error("Unexpected Expression[11]");
}

// record_type -> record field_list end
void CodeGenerator::record_type(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    field_list(son[1]);
  } else
    spdlog::error("Unexpected Expression[12]");
}

// field_list -> fixed_fields ;
void CodeGenerator::field_list(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 2) {
    target_append("struct {\n");
    fixed_fields(son[0]);
    target_append(";\n}");
  } else
    spdlog::error("Unexpected Expression[13]");
}

// fixed_fields -> idlist : type | fixed_fields ; idlist : type
void CodeGenerator::fixed_fields(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    pair<vector<string>, vector<int>> type_nums = _type(son[2]);
    vector<string> id_type = type_nums.first;
    vector<int> id_num = type_nums.second;
    if (id_type[0] == "0") {  // basic type
      target_append(id_type[1]);
      vector<int> none;
      idlist(son[0], none);
    } else if (id_type[0] == "1"){  // array type
      target_append(id_type[1]);
      idlist(son[0], id_num);
    } else if (id_type[0] == "2") {  // record type
      vector<int> none;
      idlist(son[0], none);
    } else
    spdlog::error("Unexpected Expression[14]");
  } else if (son_num == 5) {
    fixed_fields(son[0]);
    target_append(";\n");
    pair<vector<string>, vector<int>> type_nums = _type(son[4]);
    vector<string> id_type = type_nums.first;
    vector<int> id_num = type_nums.second;
    if (id_type[0] == "0") {  // basic type
      target_append(id_type[1]);
      vector<int> none;
      idlist(son[2], none);
    } else if (id_type[0] == "1") {  // array type
      target_append(id_type[1]);
      idlist(son[2], id_num);
    } else if (id_type[0] == "2") {  // record type
      vector<int> none;
      idlist(son[0], none);
    } else
    spdlog::error("Unexpected Expression[14]");
  } else
    spdlog::error("Unexpected Expression[14]");
}

// subprogram_declarations -> subprogram_declarations subprogram ; | e
void CodeGenerator::subprogram_declarations(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    subprogram_declarations(son[0]);
    subprogram(son[1]);
  } else if (son_num == 1) {
  } else
    spdlog::error("Unexpected Expression[15]");
}

// subprogram -> subprogram_head ; subprogram_body
void CodeGenerator::subprogram(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    subprogram_head(son[0]);
    subprogram_body(son[2]);
    state_stack.pop();
  } else
    spdlog::error("Unexpected Expression[16]");
}

// subprogram_head -> procedure id formal_parameter | function id
// formal_parameter : basic_type
void CodeGenerator::subprogram_head(
    int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {  // procedure
    target_append("void");  // procedure equal to "void function()"
    string value = get_str_value(son[1]);  // function name
    state_stack.push(value);
    target_append(" " + value);

    formal_parameter(son[2]);
  } else if (son_num == 5) {  // function
    string id_type = basic_type(son[4]);
    target_append(id_type);
    string id_value = get_str_value(son[1]);
    state_stack.push(id_value);  // function name
    target_append(" " + id_value);

    formal_parameter(son[2]);
  } else
    spdlog::error("Unexpected Expression[17]");
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
  } else
    spdlog::error("Unexpected Expression[18]");
}

// parameter_list -> parameter_list ; parameter | parameter
void CodeGenerator::parameter_list(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    parameter_list(son[0]);
    target_append(", ");  // separated by "," in C
    parameter(son[2]);
  } else if (son_num == 1) {
    parameter(son[0]);
  } else
    spdlog::error("Unexpected Expression[19]");
}

// parameter -> var_parameter | value_parameter
void CodeGenerator::parameter(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 1) {
    string token = get_token(son[0]);
    if (token == "var_parameter")  // pass addr
      var_parameter(son[0]);
    else  // pass value
      value_parameter(son[0]);
  } else
    spdlog::error("Unexpected Expression[20]");
}

// var_parameter -> var value_parameter
void CodeGenerator::var_parameter(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 2) {
    value_parameter(son[1]);
  } else
    spdlog::error("Unexpected Expression[21]");
}

// value_parameter -> idlist : basic_type
void CodeGenerator::value_parameter(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    string id_type = basic_type(son[2]);
    target_append(id_type);

    int father = get_father(node_id);
    string token = get_token(father);
    vector<int> none;
    if (token == "var_parameter")
      idlist(son[0], none, id_type, true);
    else
      idlist(son[0], none, id_type);
  } else
    spdlog::error("Unexpected Expression[22]");
}

// subprogram_body -> const_declarations var_declarations compound_statement
void CodeGenerator::subprogram_body(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {
    target_append(" {\n");
    const_declarations(son[0]);
    var_declarations(son[1]);
    compound_statement(son[2]);
    target_append("}\n");
  } else
    spdlog::error("Unexpected Expression[23]");
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
    } else {
      match(son[0], "begin");
      target_append(" {\n");
      statement_list(son[1]);
      match(son[2], "end");
      target_append("}\n");
    }
  } else
    spdlog::error("Unexpected Expression[24]");
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
    spdlog::error("Unexpected Expression[25]");
}
// statement -> variable assignop expression
//            | procedure_call
//            | compound_statement
//            | if expression then statement else_part
//            | for id assignop expression to expression do statement
//            | read ( variable_list )
//            | write ( expression_list )
//            | writeln ( expression_list )
//            | e
void CodeGenerator::statement(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 3) {  // variable assignop expression
    pair<string, string> var_pair = variable(son[0]);
    string var_type = var_pair.first;
    string var = var_pair.second;

    match(son[1], "assignop");
    if (var == state_stack.top())
      target_append("return ");
    else {
      target_append(var);
      target_append(" = ");
    }

    string exp = expression(son[2]);
    target_append(exp);
    target_append(";\n");
  } else if (son_num == 1) {
    if (tree[son[0]].type == "procedure_call") {
      procedure_call(son[0]);
      target_append(";\n");
    } else if (tree[son[0]].type == "compound_statement") {
      compound_statement(son[0]);
      target_append("\n");
    } else {
      match(son[0], "e");
    }
  } else if (son_num == 5) {  // if expression then statement else_part
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
  } else if (son_num == 8) {
    // for id assignop expression to expression do statement
    match(son[0], "for");
    target_append("for (");
    match(son[1], "id");

    if (is_addr(tree[son[1]].str_value))
      target_append("*" + tree[son[1]].str_value);
    else
      target_append(tree[son[1]].str_value);

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
    //           | writeln ( expression_list )
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
      head_file.insert("#include <stdio.h>\n");
    }
    if (tree[son[0]].type == "write" || tree[son[0]].type == "writeln") {
      target_append("printf");
      match(son[1], "(");

      target_append("(");
      target_append("\"");

      // TODO check value type
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
      if (tree[son[0]].type == "writeln")
        target_append("\\n");
      target_append("\"");
      target_append(", ");

      target_append(join_vec(elist, ", "));
      match(son[3], ")");
      target_append(");\n");
      head_file.insert("#include <stdio.h>\n");
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
    spdlog::error("Unexpected Expression[26]");
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
    spdlog::error("Unexpected Expression[27]");
}

// variable -> id id_varpart | id . variable
std::pair<string, string> CodeGenerator::variable(int node_id, bool* is_bool) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 2) {  // id id_varpart
    match(son[0], "id");
    string var_type = get_var_type(tree[son[0]].str_value);
    if (var_type == "boolean") *is_bool = true;
    string var_part = id_varpart(son[1]);
    string has_ptr = is_addr(tree[son[0]].str_value) ? "*" : "";
    string is_func = "";
    return {var_type, has_ptr + tree[son[0]].str_value + is_func + var_part};
  } if (son_num == 3) {  // id . variable
    string var_type = get_var_type(tree[son[0]].str_value);
    pair<string, string> var_part = variable(son[2], is_bool);
    return {var_type, tree[son[0]].str_value + "." + var_part.second};
  } else
    spdlog::error("Unexpected Expression[28]");
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
    vector<int> blist = get_bound(id_value);

    vector<string> elist_trans;
    for (int i = 0; i < (int)elist.size(); i++){
      elist_trans.push_back("[" + elist[i] + "]");
    }
    return join_vec(elist_trans, "");
  } else if (son_num == 1) {
    match(son[0], "e");
    return "";
  } else
    spdlog::error("Unexpected Expression[29]");
}

// procedure_call -> id | id ( expression_list )
void CodeGenerator::procedure_call(int node_id) {
  vector<int> son = get_son(node_id);
  int son_num = son.size();
  if (son_num == 1) {  // id
    match(son[0], "id");
    target_append(tree[son[0]].str_value);
    target_append("()");
  } else if (son_num == 4) {  // id ( expression_list )
    match(son[0], "id");
    target_append(tree[son[0]].str_value);

    if (is_func(tree[son[0]].str_value)) {
      vector<bool> args_list = get_args(tree[son[0]].str_value);
    }

    match(son[1], "(");
    target_append("(");

    vector<string> elist, tlist;
    expression_list(son[2], elist, tlist);
    target_append(join_vec(elist, ", "));

    match(son[3], ")");
    target_append(")");
  } else
    spdlog::error("Unexpected Expression[30]");
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
  } 
  else if (son_num == 1){
    match(son[0], "e");
  }
  else
    spdlog::error("Unexpected Expression[31]");
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
    spdlog::error("Unexpected Expression[32]");
}

// expression -> simple_expression relop simple_expression
//             | simple_expression
std::string CodeGenerator::expression(
    int node_id, bool* is_bool) {
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
    spdlog::error("Unexpected Expression[33]");
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
    if (addop == "or") addop = "||";
    string _term = term(son[2]);
    return exp + addop + _term;
  } else if (son_num == 1)  // term
    return term(son[0]);
  else
    spdlog::error("Unexpected Expression[34]");
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
    spdlog::error("Unexpected Expression[35]");
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
      if (tree[son[0]].num_type == NumType::Integer)
        return to_string(int(val));
      else if (tree[son[0]].num_type == NumType::Real)
        return to_string(val);
    }
    if (tree[son[0]].type == "variable") {
      pair<string, string> var_pair = variable(son[0]);
      return var_pair.second;
    }
  } else if (son_num == 4) {  // id ( expression_list )
    match(son[0], "id");
    match(son[1], "(");

    vector<bool> is_addr_list;
    // if (is_func(tree[son[0]].str_value))
    is_addr_list = get_args(tree[son[0]].str_value);

    vector<string> elist, tlist;
    expression_list(son[2], elist, tlist);
    match(son[3], ")");

    assert(is_addr_list.size() == elist.size());

    vector<string> args_list;
    for (int i = 0; i < (int)elist.size(); i++) {
      string exp = elist[i];
      bool is_addr = is_addr_list[i];
      if (is_addr)
        args_list.push_back("&" + exp);
      else
        args_list.push_back(exp);
    }
    return tree[son[0]].str_value + "(" + join_vec(args_list, ", ") + ")";
  } else if (son_num == 3) {  // ( expression )
    match(son[0], "(");
    string exp = expression(son[1], is_bool);
    match(son[2], ")");
    return "(" + exp + ")";
  } else if (son_num == 2) {  // not factor | uminus factor
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
    spdlog::error("Unexpected Expression[36]");
}