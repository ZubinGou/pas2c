#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "lexer.h"
#include "syntax_tree.h"

struct Grammar {
  std::string start;
  std::set<std::string> non_terminals;
  std::set<std::string> terminals;
  // symbol -> symbol list
  std::map<std::string, std::vector<std::vector<std::string>>> productions_map;
  std::vector<std::pair<std::string, std::vector<std::string>>> productions;
};

struct ItemElement {
  std::pair<std::string, std::vector<std::string>> production;
  std::string lookahead;
  std::size_t dot_position;
  ItemElement(std::pair<std::string, std::vector<std::string>> p, std::string l,
              int d)
      : production(p), lookahead(l), dot_position(d) {}
  bool operator<(const ItemElement& rhs) const {
    if (production != rhs.production) {
      return production < rhs.production;
    } else if (dot_position != rhs.dot_position) {
      return dot_position < rhs.dot_position;
    }
    return lookahead < rhs.lookahead;
  }

  bool operator==(const ItemElement& rhs) const {
    return production == rhs.production && lookahead == rhs.lookahead &&
           dot_position == rhs.dot_position;
  }
};

struct AnalysisState {  // used for log
 public:
  AnalysisState(std::vector<std::pair<int, Token>> stack, int input_ip,
                std::pair<int, std::size_t> act)
      : stack(stack), input_ip(input_ip), action(act){};
  std::vector<std::pair<int, Token>> stack;
  int input_ip;
  std::pair<int, std::size_t> action;
};

class Parser {
 public:
  enum ActionType { ACC = 1, R, S };
  inline static const std::string EPSILON = "e";
  inline static const std::string START = "S'";

  Parser(const std::string& filename, const std::string& inputfile);
  SyntaxTree generate_tree();

 private:
  Grammar grammar;
  std::vector<std::set<ItemElement>> item_family;
  // symbol -> symbol set
  std::map<std::string, std::set<std::string>> first_set;
  // std::map<std::string, std::set<std::string>> follow_set;
  // (row_no, symbol) -> row_no
  std::map<std::pair<std::size_t, std::string>, std::size_t> goto_table;
  // (row_no, symbol) -> (type, prod_no)
  std::map<std::pair<int, std::string>, std::pair<int, std::size_t>>
      action_table;
  // for filling action and goto
  std::map<std::pair<std::size_t, std::string>, std::size_t> item_edges;
  // from lexer
  std::vector<Token> token_list;
  std::vector<AnalysisState> analysis_log;
  std::vector<std::vector<std::pair<bool, Token>>> nodes_list;

  void load_grammar(const std::string& filename);
  void print_grammar();

  void get_first_set();
  std::set<std::string> get_first_for_symbols(std::vector<std::string>&);
  void print_first_set();

  void get_closure(std::set<ItemElement>&);
  std::set<ItemElement> get_next_item(const std::set<ItemElement>&,
                                      const std::string&);
  int is_new_item(const std::set<ItemElement>&);  // return -1 if it's a new
                                                  // item, otherwise return idx.
  void get_item_family();
  void print_item_family();
  void print_item(std::set<ItemElement>&);

  void get_table();
  void print_table();

  bool is_terminal(const std::string&);
  bool is_non_terminal(const std::string&);
  bool is_epsilon_(const std::string&);  // if there's epsilon in first set

  void analyze();
  void print_analysis_log();
};

#endif