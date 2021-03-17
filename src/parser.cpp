#include "parser.h"

#include <fstream>
#include <iostream>
#include <regex>

#include "../include/json.hpp"

using namespace std;
using json = nlohmann::json;

Parser::Parser(const std::string& filename) {
  load_grammar(filename);
  print_grammar();

  get_first_set();
  print_first_set();
}

void Parser::load_grammar(const std::string& filename) {
  std::ifstream grammar_config(filename);
  json grammar_json;
  grammar_config >> grammar_json;
  grammar.start = grammar_json["S"].get<string>();
  for (auto& symbol : grammar_json["N"]) {
    grammar.non_terminals.insert(symbol.get<string>());
  }
  for (auto& symbol : grammar_json["T"]) {
    grammar.terminals.insert(symbol.get<string>());
  }
  for (auto& prod : grammar_json["P"].items()) {
    for (auto& symbols_list : prod.value()) {
      grammar.productions[prod.key()].push_back(symbols_list.get<vector<string>>());
    }
  }
}

void Parser::print_grammar() {
  cout << "\n---------- Grammar ----------\n" << endl;
  cout << "Start symbol:\t" << grammar.start << endl;
  cout << "Terminal symbol:" << endl << "\t";
  for (auto& symbol : grammar.terminals) {
    cout << symbol << " ";
  }
  cout << "\n" << endl;
  cout << "Non terminal symbol:" << endl << "\t";
  for (auto& symbol : grammar.non_terminals) {
    cout << symbol << " ";
  }
  cout << "\n" << endl;
  cout << "Productions" << endl;
  for (auto& prod : grammar.productions) {
    cout << "\t" << prod.first << " -> ";
    for (auto it = prod.second.begin(); it != prod.second.end(); it++) {
      auto& symbols = *it;
      if (it != prod.second.begin()) {
        cout << "| ";
      }
      for (auto& s : symbols) {
        cout << s << " ";
      }
    }
    cout << endl;
  }
}

void Parser::get_first_set() {
  for (auto& t : grammar.terminals) { // for terminal
    first_set[t].insert(t);
  }
  bool is_update = true;
  while (is_update) {
    is_update = false;
    for (auto& lhs : grammar.non_terminals) {
      for (auto& rhs : grammar.productions[lhs]) {
        if (first_set[rhs[0]].find(EPSILON) != first_set[rhs[0]].end()) { // got epsilon
          auto result = first_set[lhs].insert(EPSILON);
          is_update |= result.second;
        }
        for (auto& symbol : rhs) {
          const auto& now_set = first_set[symbol];
          for (auto& first_symbol : now_set) {
            if (first_symbol != EPSILON && is_terminal(first_symbol)) {
              auto result = first_set[lhs].insert(first_symbol);
              is_update |= result.second;
            }
          }
          if (now_set.find(EPSILON) == now_set.end()) {
            break;
          }
        }
      }
    }
  }
}

void Parser::print_first_set() {
  cout << "\nFirst set" << endl;
  for (auto& lhs : grammar.non_terminals) {
    cout << "\t" << lhs << ":";
    for (auto& rhs : first_set[lhs]) {
      cout << " " << rhs;
    }
    cout << endl;
  }
}

bool Parser::is_terminal(const string& symbol) {
  return grammar.terminals.find(symbol) != grammar.terminals.end();
}
