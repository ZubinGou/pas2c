#include "parser.h"

#include <fstream>
#include <iostream>
#include <regex>

#include "../include/json.hpp"

using namespace std;
using json = nlohmann::json;

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
  regex reg("\\s+");
  for (auto& prod : grammar_json["P"].items()) {
    for (auto& symbols_list : prod.value()) {
      grammar.productions[prod.key()] = symbols_list.get<vector<string>>();
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
    for (auto& symbol : prod.second) {
      cout << symbol << " ";
    }
    cout << endl;
  }
}

Parser::Parser(const std::string& filename) {
  load_grammar(filename);
  print_grammar();
}