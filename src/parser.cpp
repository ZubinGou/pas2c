#include "parser.h"

#include <fstream>
#include <iostream>
#include <regex>

#include "../include/json.hpp"

using namespace std;
using json = nlohmann::json;

Parser::Parser(const std::string& filename, const std::string& inputfile) {
  
  load_grammar(filename);
  // print_grammar();

  get_first_set();
  // print_first_set();

  get_item_family();
  // print_item_family();

  get_table();
  // print_table();
  
  Lexer Lex(inputfile);
  token_list = Lex.get_token_list();
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
      grammar.productions_map[prod.key()].push_back(
          symbols_list.get<vector<string>>());
      grammar.productions.push_back(
          make_pair(prod.key(), symbols_list.get<vector<string>>()));
    }
  }
  grammar.non_terminals.insert(START);
  grammar.terminals.insert("$");
  grammar.productions_map[START].push_back(vector<string>{grammar.start});
  grammar.productions.push_back(
      make_pair(START, vector<string>{grammar.start}));
  grammar.start = START;
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
  for (auto& prod : grammar.productions_map) {
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
  for (auto& t : grammar.terminals) {  // for terminal
    first_set[t].insert(t);
  }
  bool is_update = true;
  while (is_update) {
    is_update = false;
    for (auto& lhs : grammar.non_terminals) {
      for (auto& rhs : grammar.productions_map[lhs]) {
        if (first_set[rhs[0]].find(EPSILON) !=
            first_set[rhs[0]].end()) {  // got epsilon
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

set<string> Parser::get_first_for_symbols(vector<string>& symbols) {
  set<string> ret;
  for (auto& symbol : symbols) {
    if (is_terminal(symbol) && symbol != EPSILON) {
      ret.insert(symbol);
      return ret;
    } else if (first_set[symbol].find(EPSILON) == first_set[symbol].end()) {
      // there isn't epsilon in first set
      ret.insert(first_set[symbol].begin(), first_set[symbol].end());
      return ret;
    } else {
      ret.insert(first_set[symbol].begin(), first_set[symbol].end());
    }
  }
  return ret;
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

void Parser::get_closure(set<ItemElement>& item) {
  bool is_update = true;
  while (is_update) {
    is_update = false;
    set<ItemElement> last_item(item);
    for (auto& element : last_item) {
      if (element.dot_position >=
          element.production.second.size()) {  // reach the end
        continue;
      }
      auto& rhs = element.production.second;
      if (!is_terminal(rhs[element.dot_position])) {  // non terminal
        vector<string> slice_symbols(rhs.begin() + element.dot_position + 1,
                                     rhs.end());
        auto lookaheads = get_first_for_symbols(slice_symbols);
        if (lookaheads.find(EPSILON) != lookaheads.end()) {
          lookaheads.insert(element.lookahead);
          lookaheads.erase(EPSILON);
        } else if (lookaheads.empty()) {
          lookaheads.insert(element.lookahead);
        }
        for (auto& lhs : grammar.non_terminals) {
          for (auto& prod : grammar.productions_map[lhs]) {
            if (lhs == rhs[element.dot_position]) {
              for (auto& lookahead : lookaheads) {
                auto result = item.insert(
                    ItemElement(make_pair(lhs, prod), lookahead, 0));
                is_update |= result.second;
              }
            }
          }
        }
      }
    }
  }
}

set<ItemElement> Parser::get_next_item(const set<ItemElement>& item,
                                       const string& symbol) {
  set<ItemElement> next_item;
  for (auto& ele : item) {
    if (ele.dot_position < ele.production.second.size() &&
        symbol == ele.production.second[ele.dot_position]) {
      next_item.insert(
          ItemElement(ele.production, ele.lookahead, ele.dot_position + 1));
    }
  }
  get_closure(next_item);
  return next_item;
}

bool Parser::is_new_item(const std::set<ItemElement>& new_item) {
  for (auto& item : item_family) {
    if (item == new_item) {
      return false;
    }
  }
  return true;
}

void Parser::get_item_family() {
  set<ItemElement> start_item;
  for (auto& rhs : grammar.productions_map[grammar.start]) {
    auto prod = make_pair(grammar.start, rhs);
    start_item.insert(ItemElement(prod, "$", 0));
  }
  get_closure(start_item);
  item_family.push_back(start_item);
  size_t item_idx = 0;
  while (item_idx < item_family.size()) {
    // cout << item_idx << endl;
    const auto item = item_family[item_idx];
    set<string> next_symbols;
    for (auto& ele : item) {
      if (ele.dot_position < ele.production.second.size()) {
        // there is symbols after .
        auto& next_symbol = ele.production.second[ele.dot_position];
        next_symbols.insert(next_symbol);
      }
    }
    for (auto& s : next_symbols) {
      auto next_item = get_next_item(item, s);
      if (!next_item.empty() && is_new_item(next_item)) {
        item_family.push_back(next_item);
        item_edges[make_pair(item_idx, s)] = item_family.size() - 1;
      }
    }
    item_idx++;
  }
}

void Parser::get_table() {
  // fill goto table
  for (auto& [key, to] : item_edges) {
    if (!is_terminal(key.second)) {
      goto_table[key] = to;
    }
  }
  // fill action table
  for (size_t i = 0; i < item_family.size(); i++) {
    for (auto& ele : item_family[i]) {
      if (ele.dot_position == ele.production.second.size()) {  // reduce
        if (ele.production.first == grammar.start) {
          action_table[make_pair(i, ele.lookahead)] = make_pair(ACC, -1);
        } else {
          for (size_t j = 0; j < grammar.productions.size(); j++) {
            if (grammar.productions[j] == ele.production) {
              action_table[make_pair(i, ele.lookahead)] = make_pair(R, j);
              break;
            }
          }
        }
      } else {  // shift
        auto& symbol = ele.production.second[ele.dot_position];
        if (is_terminal(symbol)) {
          action_table[make_pair(i, symbol)] =
              make_pair(S, item_edges[make_pair(i, symbol)]);
        }
      }
    }
  }
}

void Parser::print_table() {
  for (size_t i = 0; i < grammar.terminals.size() / 2; i++) printf("\t");
  printf("action");
  for (size_t i = 0;
       i < grammar.non_terminals.size() / 2 + grammar.terminals.size() / 2 - 1; i++)
    printf("\t");
  printf("|\tgoto\n");

  printf("\t");
  for (auto const& t : grammar.terminals) {
    cout << t << "\t";
  }
  printf("| ");
  for (auto const& n : grammar.non_terminals) {
    cout << n << "\t";
  }
  printf("\n");

  for (size_t i = 0; i < item_family.size(); i++) {
    cout << i << "\t";
    for (auto& t : grammar.terminals) {
      auto action = action_table[make_pair(i, t)].first;
      if (action == R) {
        cout << 'R' << action_table[make_pair(i, t)].second << "\t";
      } else if (action == S) {
        cout << 'S' << action_table[make_pair(i, t)].second << "\t";
      } else if (action == ACC) {
        cout << "ACC\t";
      } else {
        cout << "\t";
      }
    }
    cout << "| ";
    for (const auto& n : grammar.non_terminals) {
      if (goto_table.find(make_pair(i, n)) != goto_table.end())
        cout << goto_table[make_pair(i, n)] << "\t";
      else
        printf("\t");
    }
    cout << endl;
  }
  cout << endl;
}

void Parser::print_item(set<ItemElement>& item) {
  for (auto& ele : item) {
    cout << ele.production.first << " ->";
    auto& rhs = ele.production.second;
    for (size_t i = 0; i < rhs.size(); i++) {
      cout << " ";
      if (i == ele.dot_position) {
        cout << ".";
      }
      cout << rhs[i];
    }
    if (ele.dot_position == rhs.size()) {
      cout << " .";
    }
    cout << ", " << ele.lookahead << endl;
  }
}

void Parser::print_item_family() {
  for (size_t i = 0; i < item_family.size(); i++) {
    printf("I%lu\n", i);
    print_item(item_family[i]);
  }
}

bool Parser::is_terminal(const string& symbol) {
  return grammar.terminals.find(symbol) != grammar.terminals.end();
}

