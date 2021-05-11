#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <set>
#include <string>
#include <vector>

enum NumType { None = 0, Integer, Real, Boolean };

struct Token {
 public:
  inline void renew(std::string type, int line, int col, std::string str_value,
                    double num_value, NumType num_type = None) {
    this->type = type;
    this->num_value = num_value;
    this->str_value = str_value;
    this->line = line;
    this->col = col;
    this->num_type = num_type;
  };
  NumType num_type;
  std::string type;
  std::string str_value;
  double num_value;
  int line;  // line number
  int col;   // column number
};

struct Error {
 public:
  Error(std::string content, int line, int col)
      : content(content), line(line), col(col){};
  std::string content;
  int line;
  int col;
};

class Lexer {
 public:
  Lexer(const std::string& inputfile);
  std::vector<Token> get_token_list();

 private:
  bool get_new_line();
  bool get_token(Token& token);
  char get_char();
  void print_token_list();
  void print_error_list();

  std::ifstream input;
  std::vector<Token> token_list;
  std::vector<Error> error_list;
  int line_pos;          // current line
  int col_pos;           // current column
  int state;             // current state
  std::string cur_line;  // current line
  char temp_char;        // current character
  std::string cur_word;  // current word
  int last_col_pos;      // beginning column of the current word

  inline static const std::set<char> SEPARATION_CHAR = {'(', ')', '[', ']',
                                                        ',', ';', ':'};
  inline static const std::set<std::string> RESERVED_WORDS = {
      "array",    "begin", "const", "else", "do",        "end",     "for",
      "function", "if",    "not",   "of",   "procedure", "program", "record",
      "then",     "to",    "type",  "var",  "while",     "read",    "write",
      "uminus",   "and",   "div",   "mod",  "or"};
  inline static const std::set<std::string> IDENTIFIER = {
      "integer", "real", "char", "boolean", "text"};
};

#endif