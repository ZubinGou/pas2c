#include "parser.h"

#include <fstream>
#include <iostream>
#include <regex>

#include "../include/json.hpp"

using namespace std;
using json = nlohmann::json;

Parser::Parser(const string& filename, const string& inputfile) {
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

  analyze();
  // print_analysis_log();
}

SyntaxTree Parser::generate_tree() {
  SyntaxTree tree;
  Token token;
  token.renew("programstruct", 0, 0, "", 0);
  vector<pair<bool, Token>> nodes;
  nodes.push_back(make_pair(false, token));
  tree.insert_nodes(nodes);
  for (auto nodes : nodes_list) tree.insert_nodes(nodes);
  tree.print_nodes();
  return tree;
}

void Parser::load_grammar(const string& filename) {
  ifstream grammar_config(filename);
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
            if (is_terminal(first_symbol)) {
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
    if (is_terminal(symbol)) {
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
      if (is_non_terminal(rhs[element.dot_position])) {  // non terminal
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

int Parser::is_new_item(const set<ItemElement>& new_item) {
  int cnt = 0;
  for (auto& item : item_family) {
    if (item == new_item) {
      return cnt;
    }
    cnt++;
  }
  return -1;
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
      if (ele.dot_position < ele.production.second.size() &&
          ele.production.second[ele.dot_position] != EPSILON) {
        // there is symbols after .
        auto& next_symbol = ele.production.second[ele.dot_position];
        next_symbols.insert(next_symbol);
      }
    }
    for (auto& s : next_symbols) {
      auto next_item = get_next_item(item, s);
      auto next_item_idx = is_new_item(next_item);
      if (!next_item.empty() && is_new_item(next_item)) {
        if (next_item_idx == -1) {
          item_family.push_back(next_item);
          next_item_idx = item_family.size() - 1;
        }
        item_edges[make_pair(item_idx, s)] = next_item_idx;
      }
    }
    item_idx++;
  }
}

void Parser::get_table() {
  // fill goto table
  for (auto& [key, to] : item_edges) {
    if (is_non_terminal(key.second)) {
      goto_table[key] = to;
    }
  }
  // fill action table
  for (size_t i = 0; i < item_family.size(); i++) {
    for (auto& ele : item_family[i]) {
      if (ele.dot_position == ele.production.second.size() ||
          ele.production.second[0] ==
              EPSILON) {  // reduce, especially when symbol is epsilon
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
  // freopen("debug.txt", "w", stdout);
  for (size_t i = 0; i < grammar.terminals.size() / 2; i++) printf("\t\t");
  printf("action");
  for (size_t i = 0;
       i < grammar.non_terminals.size() / 2 + grammar.terminals.size() / 2 - 1;
       i++)
    printf("\t\t");
  printf("|\t\tgoto\n");

  printf("\t\t");
  for (auto const& t : grammar.terminals) {
    cout << t << "\t\t";
  }
  printf("| ");
  for (auto const& n : grammar.non_terminals) {
    cout << n << "\t\t";
  }
  printf("\n");

  for (size_t i = 0; i < item_family.size(); i++) {
    cout << i << "\t";
    for (auto& t : grammar.terminals) {
      auto action = action_table[make_pair(i, t)].first;
      if (action == R) {
        cout << 'R' << action_table[make_pair(i, t)].second << "\t\t";
      } else if (action == S) {
        cout << 'S' << action_table[make_pair(i, t)].second << "\t\t";
      } else if (action == ACC) {
        cout << "ACC\t\t";
      } else {
        cout << "\t\t";
      }
    }
    cout << "| ";
    for (const auto& n : grammar.non_terminals) {
      if (goto_table.find(make_pair(i, n)) != goto_table.end())
        cout << goto_table[make_pair(i, n)] << "\t\t";
      else
        printf("\t\t");
    }
    cout << endl;
  }
  cout << endl;
  // fclose(stdout);
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

bool Parser::is_non_terminal(const string& symbol) {
  return grammar.non_terminals.find(symbol) != grammar.non_terminals.end();
}

void Parser::analyze() {
  bool pass_analyze = true;
  // for recover error
  StrategyType strategy = DROP;
  size_t drop_cnt = 0, lookahead_len = 0, lookahead_idx = 0;
  vector<pair<int, Token>> temp_stack;
  int temp_ip = 0;
  Token temp_token;
  // for record error
  int error_len = -1;
  pair<string, string> error_msg;
  pair<int, int> error_pos = make_pair(-1, -1);
  // for analysis
  vector<pair<int, Token>> analysis_stack;
  vector<string> lookahead_tokens;
  int ip = 0;
  Token tmp;
  tmp.renew("", 0, 0, "", 0);
  analysis_stack.push_back(make_pair(0, tmp));
  tmp.renew("$", 0, 0, "$", 0);
  token_list.push_back(tmp);

  while (true) {
    pair<int, Token> stack_top = analysis_stack.back();
    Token cur_token = token_list[ip];
    auto action_pair =
        action_table.find(make_pair(stack_top.first, cur_token.type));
    if (action_pair != action_table.end()) {
      // Got in table
      // cout << "stack:" << endl;
      // for (auto stk : analysis_stack)
      //   cout << "  " << stk.first << " " << stk.second.type << endl;
      // cout << "token: " << cur_token.type << endl;
      // cout << "action:" << action;
      // if (S == action) {
      //   cout << " S" << num << endl;
      // } else if (R == action) {
      //   cout << " R" << num << endl;
      // }
      // cout << endl;
      int action = action_pair->second.first;
      int num = action_pair->second.second;
      analysis_log.push_back(AnalysisState(
          analysis_stack, ip,
          action_table[make_pair(stack_top.first, cur_token.type)]));
      if (S == action) {  // Shift
        analysis_stack.push_back(make_pair(num, cur_token));
        ip++;
      } else if (R == action) {  // Reduce
        vector<pair<bool, Token>> nodes;
        if (grammar.productions[num].second[0] != EPSILON) {
          for (auto it : grammar.productions[num].second) {
            nodes.insert(
                nodes.begin(),
                make_pair(is_terminal(analysis_stack.back().second.type),
                          analysis_stack.back().second));
            analysis_stack.pop_back();
          }
        } else {
          Token eps;
          eps.renew("e", 0, 0, "", 0);
          nodes.insert(nodes.begin(), make_pair(true, eps));
        }
        nodes_list.insert(nodes_list.begin(), nodes);
        string tmp = grammar.productions[num].first;
        Token tmp_token;
        tmp_token.renew(tmp, 0, 0, "", 0);
        analysis_stack.push_back(
            make_pair(goto_table[make_pair(analysis_stack.back().first, tmp)],
                      tmp_token));
      } else if (ACC == action) {  // Accept
        cout << "[Syntax] Accept." << endl << endl;
        break;
      } else {
        // TODO: table is broken
        pass_analyze = false;
        cout << "[Syntax] Meet errors." << endl << endl;
        break;
      }
      if (error_len >= 0) {
        error_len++;
      }
      if (error_len >= 10 || ip == token_list.size() - 1) {
        drop_cnt = 0;
        lookahead_idx = 0;
        lookahead_len = 0;
        if (error_pos.first != -1) {
          cout << "[Syntax] Error in [" << error_pos.first << ", "
               << error_pos.second << "]: ";
          if (strategy == DROP) {
            cout << "Extra token " << error_msg.first << endl;
          } else if (strategy == INSERT) {
            cout << "Lack of " << error_msg.second << " before "
                 << error_msg.first << endl;
          } else {
            cout << "Suppose to be " << error_msg.second << " but "
                 << error_msg.first << endl;
          }
        }
      }
    } else {
      // cout << cur_token << endl;
      // Failed to get in action table
      error_len = 0;
      if (strategy == DROP) {
        if (ip < token_list.size() &&
            drop_cnt < 1) {  // not ent and did not drop
          if (error_pos.first == -1) {
            error_pos = make_pair(cur_token.line, cur_token.col);
            error_msg.first = cur_token.type;
          }
          drop_cnt++;
          ip++;
        } else {
          // change drop to insert
          strategy = INSERT;
          ip -= drop_cnt;
          drop_cnt = 0;
          error_pos = make_pair(-1, -1);
        }
      }
      if (strategy == INSERT) {
        if (error_pos.first == -1) {
          error_pos = make_pair(cur_token.line, cur_token.col);
          error_msg.first = cur_token.type;
        }
        // find possible tokens to insert
        if (lookahead_len == 0) {
          for (const auto& p : action_table) {
            if (p.first.first == stack_top.first &&
                is_terminal(p.first.second)) {
              lookahead_tokens.push_back(p.first.second);
            }
          }
          lookahead_len = lookahead_tokens.size();
          ip--;
          temp_token = token_list[ip];
          temp_ip = ip;
          temp_stack = analysis_stack;
        }
        // enumerate possible tokens to insert
        if (lookahead_idx < lookahead_len) {
          analysis_stack = temp_stack;
          ip = temp_ip;
          Token t;
          error_msg.second = t.type = lookahead_tokens[lookahead_idx];
          token_list[ip] = t;
          lookahead_idx++;
        } else {
          // change insert to replace
          strategy = REPLACE;
          analysis_stack = temp_stack;
          token_list[temp_ip] = temp_token;
          ip = temp_ip + 1;
          lookahead_idx = 0;
          lookahead_len = 0;
          error_msg = make_pair(-1, -1);
          stack_top = analysis_stack.back();
        }
      }
      if (strategy == REPLACE) {
        if (error_pos.first == -1) {
          error_pos = make_pair(cur_token.line, cur_token.col);
          error_msg.first = cur_token.type;
        }
        if (lookahead_len == 0) {
          for (const auto& p : action_table) {
            if (p.first.first == stack_top.first &&
                is_terminal(p.first.second)) {
              lookahead_tokens.push_back(p.first.second);
            }
          }
          lookahead_len = lookahead_tokens.size();
          temp_token = token_list[ip];
          temp_ip = ip;
          temp_stack = analysis_stack;
        }
        if (lookahead_idx < lookahead_len) {
          analysis_stack = temp_stack;
          ip = temp_ip;
          Token t;
          error_msg.second = t.type = lookahead_tokens[lookahead_idx];
          token_list[ip] = t;
          lookahead_idx++;
        } else {
          strategy = DROP;
          token_list[ip] = temp_token;
          ip++;
          lookahead_idx = 0;
          lookahead_len = 0;
          error_msg = make_pair(-1, -1);
          stack_top = analysis_stack.back();
        }
      }
    }
  }
}

void Parser::print_analysis_log() {
  for (auto it : analysis_log) {
    cout << "--------------------------------" << endl;
    cout << "Stack:" << endl;
    for (auto stk : it.stack)
      cout << "  " << stk.first << " " << stk.second.type << endl;
    cout << "Input:" << endl;
    vector<Token> input(token_list.begin() + it.input_ip, token_list.end());
    for (auto ipt : input) {
      cout << "  [" << ipt.type << " ";
      if (ipt.str_value == "")
        cout << ipt.num_value << " ";
      else
        cout << ipt.str_value << " ";
      cout << ipt.line << " " << ipt.col << "]" << endl;
    }
    cout << "Action:" << endl;
    if (it.action.first == R) {
      cout << "  Reduced by " << grammar.productions[it.action.second].first
           << "=>";
      for (auto rt : grammar.productions[it.action.second].second)
        cout << rt << " ";
      cout << endl;
    } else if (it.action.first == S)
      cout << " Shift to State " << it.action.second << endl;
    else if (it.action.first == ACC)
      cout << "Accept!" << endl;
    cout << endl;
  };
}