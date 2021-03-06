#include "lexer.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "assert.h"

using namespace std;

bool Lexer::is_digit(char c) {
  if('1' <= c && '9' >= c)
    return true;
  else return false;
}
bool Lexer::is_alpha(char c) {
  if('a' <= c && 'z' >= c || 'A' <= c && 'Z' >= c)
    return true;
  else return false;
}


Lexer::Lexer(const std::string& inputfile) {
  input = ifstream(inputfile, ios::in);
  cur_line = "";
  line_pos = 0;
  col_pos = 1;
  state = 0;
  spdlog::debug("Input File:");
  while (cur_line.length() == 0) {
    string tmp;
    if (!getline(input, tmp)) {
      spdlog::critical("the input file is empty!");
      exit(1);
    }
    cur_line = "";
    transform(tmp.begin(), tmp.end(), back_inserter(cur_line), ::tolower);
    line_pos++;
    spdlog::set_pattern("\t\t%v");
    spdlog::debug("\t\t line " + to_string(line_pos) + " : " + cur_line);
    spdlog::set_pattern("%+");
  }
}

std::vector<Token> Lexer::get_token_list() {
  Token token;
  while (get_token(token)) token_list.push_back(token);
  print_token_list();
  print_error_list();
  // assert(error_list.size() == 0);
  if (error_list.size() != 0) exit(1);
  return token_list;
}

bool Lexer::get_token(Token& token) {
  cur_word = "";
  last_col_pos = -1;
  if (!get_new_line()) return false;
  if (4 != state) {
    temp_char = get_char();  // get a new char
    while (temp_char == 0) {
      if (!get_new_line()) return false;
      temp_char = get_char();
    }
  }

  while (col_pos < 1 + int(cur_line.length())) {
    if (0 == state) {            // beginning state
      if (is_digit(temp_char)) {  // 0-9
        cur_word = temp_char;
        last_col_pos = col_pos;
        state = 2;
        col_pos++;
        if (col_pos == 1 + int(cur_line.length())) {
          token.renew("num", line_pos, last_col_pos, "", stoi(cur_word.c_str()),
                      Integer);
          state = 0;
          return true;
        }
      } else if (is_alpha(temp_char)) {  // letter
        cur_word = temp_char;
        last_col_pos = col_pos;
        state = 1;
        col_pos++;
        if (col_pos == 1 + int(cur_line.length())) {
          token.renew("id", line_pos, last_col_pos, cur_word, 0);
          state = 0;
          return true;
        }
      } else if ('=' == temp_char) {  // =
        col_pos++;
        token.renew("relop", line_pos, col_pos, "=", 0);
        return true;
      } else if ('<' == temp_char) {  // <, <>, <=
        if ('>' == cur_line[col_pos]) {
          token.renew("relop", line_pos, col_pos, "<>", 0);
          col_pos += 2;
        } else if ('=' == cur_line[col_pos]) {
          token.renew("relop", line_pos, col_pos, "<=", 0);
          col_pos += 2;
        } else {
          token.renew("relop", line_pos, col_pos, "<", 0);
          col_pos += 1;
        }
        return true;
      } else if ('>' == temp_char) {  // >, >=
        if ('=' == cur_line[col_pos]) {
          token.renew("relop", line_pos, col_pos, ">=", 0);
          col_pos += 2;
        } else {
          token.renew("relop", line_pos, col_pos, ">", 0);
          col_pos += 1;
        }
        return true;
      } else if ('+' == temp_char) {  // +
        token.renew("addop", line_pos, col_pos, "+", 0);
        col_pos += 1;
        return true;
      } else if ('-' == temp_char) {  // -
        token.renew("addop", line_pos, col_pos, "-", 0);
        col_pos += 1;
        return true;
      } else if ('*' == temp_char) {  // *
        token.renew("mulop", line_pos, col_pos, "*", 0);
        col_pos += 1;
        return true;
      } else if ('/' == temp_char) {  // /
        token.renew("mulop", line_pos, col_pos, "/", 0);
        col_pos += 1;
        return true;
      } else if (':' == temp_char && '=' == cur_line[col_pos]) {  // :=
        token.renew("assignop", line_pos, col_pos, ":=", 0);
        col_pos += 2;
        return true;
      } else if ('{' == temp_char) {  // {annotation}
        state = 5;
        col_pos += 1;
      } else if ('.' == temp_char) {  // .., .
        if ('.' == cur_line[col_pos]) {
          token.renew("..", line_pos, col_pos, "..", 0);
          col_pos += 2;
        } else {
          token.renew(".", line_pos, col_pos, ".", 0);
          col_pos += 1;
        }
        return true;
      } else if ('\'' == temp_char) {  // '
        token.renew("'", line_pos, col_pos, "'", 0);
        state = 4;
        col_pos++;
        return true;
      } else if (SEPARATION_CHAR.count(temp_char) !=
                 0) {  // separation_character
        token.renew(string(1, temp_char), line_pos, col_pos,
                    string(1, temp_char), 0);
        col_pos++;
        return true;
      } else {  // illegal character
        error_list.push_back(Error("Illegal character.", line_pos, col_pos));
        col_pos++;
        if (col_pos == 1 + int(cur_line.length())) {
          if (!get_new_line()) return false;
          temp_char = get_char();  // get a new char
          while (temp_char == 0) {
            if (!get_new_line()) return false;
            temp_char = get_char();
          }
          cur_word = "";
          last_col_pos = -1;
        }
      }
    }

    else if (1 == state) {  // last character is a letter
      char t_char = cur_line[col_pos - 1];
      if (is_digit(t_char) || is_alpha(t_char)) {
        cur_word += t_char;
        col_pos++;
        if (col_pos == 1 + int(cur_line.length())) {
          if (RESERVED_WORDS.count(cur_word) != 0)
            token.renew(cur_word, line_pos, last_col_pos, cur_word, 0);
          else if (cur_word == "true")
            token.renew("num", line_pos, last_col_pos, cur_word, 1, Boolean);
          else if (cur_word == "false")
            token.renew("num", line_pos, last_col_pos, cur_word, 0, Boolean);
          else
            token.renew("id", line_pos, last_col_pos, cur_word, 0);
          state = 0;
          return true;
        }
      } else {
        if (RESERVED_WORDS.count(cur_word) != 0) {  // reserved words
          if ("program" == cur_word && col_pos < 1 + int(cur_line.length()) &&
              ' ' == cur_line[col_pos - 1]) {
            token.renew(cur_word, line_pos, last_col_pos, cur_word, 0);
            state = 0;
            return true;
          } else if ("end" == cur_word && last_col_pos >= 1 &&
                     (last_col_pos == 1 || ' ' == cur_line[last_col_pos - 2])) {
            token.renew(cur_word, line_pos, last_col_pos, cur_word, 0);
            state = 0;
            return true;
          } else if ("array" == cur_word && ' ' == cur_line[last_col_pos - 2] &&
                     col_pos < 1 + int(cur_line.length()) &&
                     '[' == cur_line[col_pos - 1]) {
            token.renew(cur_word, line_pos, last_col_pos, cur_word, 0);
            state = 0;
            return true;
          } else if ("read" == cur_word && last_col_pos >= 1 &&
                     (last_col_pos == 1 || ' ' == cur_line[last_col_pos - 2])) {
            token.renew(cur_word, line_pos, last_col_pos, cur_word, 0);
            state = 0;
            return true;
          } else if (("write" == cur_word || "writeln" == cur_word) &&
                     last_col_pos >= 1 &&
                     (last_col_pos == 1 || ' ' == cur_line[last_col_pos - 2])) {
            token.renew(cur_word, line_pos, last_col_pos, cur_word, 0);
            state = 0;
            return true;
          } else if (last_col_pos >= 1 &&
                     (last_col_pos == 1 || ' ' == cur_line[last_col_pos - 2]) &&
                     (col_pos == 1 + int(cur_line.length()) ||
                      (col_pos < 1 + int(cur_line.length()) &&
                       ' ' == cur_line[col_pos - 1]))) {
            if ("div" == cur_word || "and" == cur_word || "mod" == cur_word) {
              token.renew("mulop", line_pos, last_col_pos, cur_word, 0);
              state = 0;
            } else if ("or" == cur_word) {
              token.renew("addop", line_pos, last_col_pos, cur_word, 0);
              state = 0;
            } else {
              token.renew(cur_word, line_pos, last_col_pos, cur_word, 0);
              state = 0;
            }
            return true;
          } else {  // error
            error_list.push_back(
                Error("Missing space before/after reserved word.", line_pos,
                      col_pos));
            state = 0;
            token.renew(cur_word, line_pos, last_col_pos, cur_word, 0);
            return true;
          }
        } else {  // cur_word is normal word
          if (IDENTIFIER.count(cur_word) != 0)
            token.renew(cur_word, line_pos, last_col_pos, cur_word, 0);
          else if (cur_word == "true")
            token.renew("num", line_pos, last_col_pos, cur_word, 1, Boolean);
          else if (cur_word == "false")
            token.renew("num", line_pos, last_col_pos, cur_word, 0, Boolean);
          else
            token.renew("id", line_pos, last_col_pos, cur_word, 0);
          state = 0;
          return true;
        }
      }
    }

    else if (2 == state) {  // last character is a digit
      if (is_digit(cur_line[col_pos - 1])) {
        cur_word += cur_line[col_pos - 1];
        col_pos++;
        if (col_pos == 1 + int(cur_line.length())) {
          token.renew("num", line_pos, last_col_pos, "", stoi(cur_word.c_str()),
                      Integer);
          state = 0;
          return true;
        }
      } else if ('.' == cur_line[col_pos - 1] &&
                 col_pos + 1 < 1 + int(cur_line.length()) &&
                 cur_line[col_pos] != '.') {  // float number
        state = 3;
        cur_word += cur_line[col_pos - 1];
        col_pos++;
      } else {
        state = 0;
        token.renew("num", line_pos, last_col_pos, "", stoi(cur_word.c_str()),
                    Integer);
        return true;
      }
    }

    else if (3 == state) {  // last character is
      if (isdigit(cur_line[col_pos - 1])) {
        cur_word += cur_line[col_pos - 1];
        col_pos++;
        if (col_pos == 1 + int(cur_line.length())) {
          token.renew("num", line_pos, last_col_pos, "", atof(cur_word.c_str()),
                      Real);
          state = 0;
          return true;
        }
      } else {
        state = 0;
        token.renew("num", line_pos, last_col_pos, "", atof(cur_word.c_str()),
                    Real);
        return true;
      }
    }

    else if (4 == state) {  // in '...'
      if ('\'' != cur_line[col_pos - 1]) {
        if (is_alpha(cur_line[col_pos - 1])) {
          token.renew("letter", line_pos, col_pos,
                      string(1, cur_line[col_pos - 1]), 0);
          col_pos++;
          return true;
        } else {
          error_list.push_back(
              Error("Illegal character in ' '.", line_pos, col_pos));
          col_pos++;
        }
      } else {
        token.renew("'", line_pos, col_pos, "'", 0);
        state = 0;
        col_pos++;
        return true;
      }
    }

    else if (5 == state) {  // annotation state, in {...}
      if ('}' != cur_line[col_pos - 1]) {
        col_pos++;
        if (col_pos == 1 + int(cur_line.length())) {
          cur_word = "";
          last_col_pos = -1;
          if (!get_new_line()) {
            error_list.push_back(
                Error("Annotation is not complete, '}' is missing.", line_pos,
                      col_pos));
            return false;
          }
          temp_char = get_char();  // get a new char
          while (temp_char == 0) {
            if (!get_new_line()) return false;
            temp_char = get_char();
          }
        }
      } else {
        state = 0;
        col_pos++;
        if (col_pos == 1 + int(cur_line.length())) {
          if (!get_new_line()) return false;
          temp_char = get_char();  // get a new char
          while (temp_char == 0) {
            if (!get_new_line()) return false;
            temp_char = get_char();
          }
          cur_word = "";
          last_col_pos = -1;
        } else {
          temp_char = get_char();  // get a new char
          while (temp_char == 0) {
            if (!get_new_line()) return false;
            temp_char = get_char();
          }
        }
      }
    }
  }

  return false;
}

bool Lexer::get_new_line() {
  while (0 >= col_pos || 1 + int(cur_line.length()) == col_pos) {
    // current line is empty or reach the end of line
    cur_line = "";
    while (cur_line.length() == 0) {
      string tmp;
      if (!getline(input, tmp)) return false;  // reach the end of file
      cur_line = "";
      transform(tmp.begin(), tmp.end(), back_inserter(cur_line), ::tolower);
      line_pos++;
      col_pos = 1;
      spdlog::set_pattern("\t\t%v");
      spdlog::debug("\t\t line " + to_string(line_pos) + " : " + cur_line);
      spdlog::set_pattern("%+");
    }
    return true;

    //   string tmp;
    //   if (!getline(input, tmp)) return false;
    //   cur_line = "";
    //   transform(tmp.begin(), tmp.end(), back_inserter(cur_line), ::tolower);
    //   line_pos++;
    //   col_pos = 1;
    //   if (spdlog::get_level() == spdlog::level::debug)
    //     cout << "\t\t line " + to_string(line_pos) + " : " << cur_line <<
    //     endl;
    //   if (int(cur_line[cur_line.length() - 1]) != 13) {
    //     cur_line.append("\r");
    //     return true;
    //   }
  }
  return true;
}

char Lexer::get_char() {
  if (cur_line[col_pos - 1] != ' ') return cur_line[col_pos - 1];
  while (col_pos < int(cur_line.length())) {
    if (cur_line[col_pos - 1] == '\t' || cur_line[col_pos - 1] == ' ')
      col_pos += 1;
    else
      return cur_line[col_pos - 1];
  }
  return 0;
}

// bool Lexer::is_illegal(char c) {
//   while (!(c > 1 || c < 255)) {
//     col_pos++;
//     c = 
//   }
// }

void Lexer::print_token_list() {
  // type, value, line, col
  spdlog::debug("Token List:");
  spdlog::set_pattern("\t\t%v");
  int i = 1;
  // if (spdlog::get_level() == spdlog::level::debug)
  for (auto it : token_list) {
    stringstream ss;
    ss << "\t\tToken " << i << ": [" << it.type << " ";
    if (it.str_value == "")
      ss << it.num_value << " ";
    else
      ss << it.str_value << " ";
    ss << it.line << " " << it.col << "]";
    spdlog::debug(ss.str());
    i++;
  }
  spdlog::set_pattern("%+");
}

void Lexer::print_error_list() {
  if (error_list.size() == 0)
    spdlog::debug("[Lexer] No Errors.");
  else {
    spdlog::error("[Lexer] Have Errors:");
    spdlog::set_pattern("\t\t%v");
    for (auto it : error_list) {
       spdlog::error("\t\tline " + to_string(it.line) + ", column " + to_string(it.col) + ": " + it.content);
    }
    spdlog::set_pattern("%+");
  }
}