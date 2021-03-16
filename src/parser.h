#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <set>
#include <string>
#include <vector>

struct Grammar {
  std::string start;
  std::set<std::string> not_terminals;
  std::set<std::string> terminals;
  // symbol -> symbol list
  std::map<std::string, std::vector<std::string>> productions;
};

struct Item {
  std::pair<std::string, std::vector<std::string>> prodcution;
  std::string look_ahead;
  int dot_position;
};

class Parser {
 public:
  Parser(std::string filename);

 private:
  Grammar grammar;
  std::vector<std::set<Item>> item_set;
  std::map<std::string, std::set<std::string>> first_set;  // symbol -> symbol set
  std::map<std::string, std::set<std::string>> follow_set;
  // (row_no, symbol) -> row_no
  std::map<std::pair<int, std::string>, int> goto_table;
  // (row_no, symbol) -> (type, row_no)
  std::map<std::pair<int, std::string>, std::pair<int, int>> action_table;
};

#endif