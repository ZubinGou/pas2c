#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include "lexer.h"

#include <string>
#include <vector>

struct Node {
 public:
  Node(int father, int id, bool is_terminal, std::string type = "",
           double num_value = 0, std::string str_value = "", int line = 0,
           int col = 0)
      : father(father),
        son_num(0),
        id(id),
        type(type),
        num_value(num_value),
        str_value(str_value),
        is_terminal(is_terminal),
        line(line),
        col(col){};

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

class SyntaxTree {
 public:
  SyntaxTree() : cur_node(0), node_num(1), root(0) {
    node_dic.push_back(Node(0, 0, false));
  };

  void insert_nodes(std::vector<std::pair<bool, Token>> nodes);
  void find_inferior_node(/* par: id, inferior_pos */);
  void print_nodes();          // used for debugging

  std::vector<Node> node_dic;  // 语法树的全部节点, 下标为编号
  int cur_node;                // 当前指向的节点编号
  int node_num;                // 语法树拥有的节点数量
  int root;                    // 根节点的编号
};

#endif