// #ifndef CODE_GENERATOR_H
// #define CODE_GENERATOR_H

// #include <stack>

// #include "semantic_analyzer.h"
// #include "syntax_tree.h"

// /*
// struct Node {
//  public:
//   Node(int father, int id, bool is_terminal, std::string type = "",
//            double num_value = 0, std::string str_value = "", int line = 0,
//            int col = 0)
//       : father(father),
//         son_num(0),
//         id(id),
//         type(type),
//         num_value(num_value),
//         str_value(str_value),
//         is_terminal(is_terminal),
//         line(line),
//         col(col){};

//   int father;             // 父节点的编号
//   int son_num;            // 子结点的个数
//   int id;                 // 节点编号
//   std::string type;       // 结点代表的符号类型
//   double num_value;       // 节点携带的数值
//   std::string str_value;  // 节点携带的字符串值
//   bool is_terminal;       // 当前节点是否为终结符
//   int line;                // 如果是终结符，终结符的行号
//   int col;                // 如果是终结符，终结符的列号
//   std::vector<int> son;   // 子节点的编号集

// };
// */

// class CodeGenerator {
//  public:
//   CodeGenerator(std::vector<Node> syntax_tree, SemanticAnalyzer semantic_analyzer) {
//     this->tree = syntax_tree;
//     this->table = semantic_analyzer.controller;
//   }
  
//   std::string run(); // 返回 target_code

//  private:
//   std::vector<Node> tree; // 语法树的根节点
//   SymbolTableController table; // 符号表控制器
//   std::stack<std::string> state_stack; // 状态栈，标记作用域
//   std::string target_code; // 目标代码

//   // target_code manipulate
//   void append(std::string code); // 部分代码到目标代码
//   void add_indent(); // 为目标代码添加缩进

//   // Parser interface
//   vector<int> get_child(int node_id); // return tree[node_id].son
//   int get_father(int node_id); // return tree[node_id].father
//   void match(int node_id, string token); // match current node and token

//   // Semantic intrface
//   bool id_addr(string id);
//   bool is_func(string id);
//   string get_var_type(string id);
//   string get_exp_type(int node_id);
//   vector<int> get_bound(string id);
//   vector<bool> get_args(string id);

//   // Production



//   // debug
//   void print_target_code(); // 打印目标代码
// };

// #endif