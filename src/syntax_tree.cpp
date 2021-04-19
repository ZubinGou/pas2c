#include "syntax_tree.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

using namespace std;

void SyntaxTree::insert_nodes(std::vector<std::pair<bool, Token>> nodes) {
  for (auto it : nodes) {
    node_dic.push_back(Node(cur_node, node_num, it.first, it.second.type,
                            it.second.num_value, it.second.str_value,
                            it.second.line, it.second.col));
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
  cout << "Syntax Tree:" << endl;
  for (auto node : this->node_dic) {
    cout << "node " << setw(3) << node.id << ": ";
    string sign = "[" + node.type + "," + to_string(node.num_value) + "," +
                  node.str_value + "," + to_string(node.line) + "," +
                  to_string(node.col) + "]";
    cout << setw(40) << sign;

    if (node.is_terminal)
      cout << setw(20) << "terminal";
    else
      cout << setw(20) << "non-terminal";
    cout << setw(15) << "father: " << setw(3) << node.father << "   son: ";
    for (auto it : node.son) cout << it << " ";
    cout << endl;
  }
}

 Node SyntaxTree::find_inferior_node(const int& id, const int& inferior_pos){
    return node_dic[node_dic[id].son[inferior_pos]];
  }