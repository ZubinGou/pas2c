#include "syntax_tree.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void SyntaxTree::insert_nodes(std::vector<std::pair<bool, Token>> nodes) {
  for (auto it : nodes) {
    node_dic.push_back(Node(cur_node, node_num, it.first, it.second.type,
                            it.second.num_value, it.second.str_value,
                            it.second.line, it.second.col, it.second.num_type));
    node_dic[cur_node].son.push_back(node_num);
    node_dic[cur_node].son_num++;
    node_num++;
  }
  int temp = node_num - 1;
  // cout << "temp: " << temp << endl;
  if (!node_dic[temp].is_terminal)
    cur_node = temp;
  else {
    while (true) {
      vector<int> list = node_dic[node_dic[temp].father].son;
      // cout << "list: ";
      // for (auto i : list) cout << i << " ";
      // cout << endl;
      vector<int>::iterator iter = find(list.begin(), list.end(), temp - 1);
      if (list.end() != iter) {
        temp--;
        if (!node_dic[temp].is_terminal) {
          cur_node = temp;
          break;
        }
      } else {
        temp = node_dic[temp].father;
        if (0 == temp) {
          cur_node = 0;
          break;
        }
      }
    }
  }
  // cout << "cur_node: " << cur_node << endl;
  // print_nodes().;
  // getchar();
}

void SyntaxTree::print_nodes() {
  spdlog::debug("Syntax Tree:");
  spdlog::set_pattern("\t\t%v");
  for (auto node : this->node_dic) {
    stringstream ss;
    ss << "node " << setw(3) << node.id << ": ";
    string sign = "[" + node.type + "," + to_string(node.num_value) + "," +
                  node.str_value + "," + to_string(node.line) + "," +
                  to_string(node.col) + "]";
    ss << setw(40) << sign;

    if (node.is_terminal)
      ss << setw(20) << "terminal";
    else
      ss << setw(20) << "non-terminal";
    if (node.num_type == Boolean)
      ss << setw(10) << "boolean";
    else if (node.num_type == Integer)
      ss << setw(10) << "integer";
    else if (node.num_type == Real)
      ss << setw(10) << "real";
    ss << setw(15) << "father: " << setw(3) << node.father << "   son: ";
    for (auto it : node.son) ss << it << " ";
    spdlog::debug(ss.str());
  }
  spdlog::set_pattern("%+");
}

Node SyntaxTree::find_inferior_node(const int& id, const int& inferior_pos) {
  return node_dic[node_dic[id].son[inferior_pos]];
}