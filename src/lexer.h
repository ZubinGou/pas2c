#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <set>
#include <string>
#include <vector>

struct Token {
 public:
  inline void renew(std::string type, int line, int col, std::string str_value,
                    double num_value) {
    this->type = type;
    this->num_value = num_value;
    this->str_value = str_value;
    this->line = line;
    this->col = col;
  };
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
  inline static const std::set<std::string> RESERVED_WORDS = {"and",
                                                              "array",
                                                              "begin",
                                                              "case",
                                                              "const",
                                                              "div",
                                                              "do",
                                                              "downto",
                                                              "else",
                                                              "end",
                                                              "file",
                                                              "for",
                                                              "function",
                                                              "goto",
                                                              "if",
                                                              "in",
                                                              "label",
                                                              "mod",
                                                              "nil",
                                                              "not",
                                                              "of",
                                                              "or",
                                                              "packed",
                                                              "procedure",
                                                              "program",
                                                              "record",
                                                              "repeat",
                                                              "set",
                                                              "then",
                                                              "to",
                                                              "type",
                                                              "until",
                                                              "var",
                                                              "while",
                                                              "with",
                                                              "wxports",
                                                              "shr",
                                                              "string",
                                                              "asm",
                                                              "object",
                                                              "unit",
                                                              "constructor",
                                                              "implementation",
                                                              "destructor",
                                                              "uses",
                                                              "inherited",
                                                              "inline",
                                                              "interface",
                                                              "library",
                                                              "xor",
                                                              "shl",
                                                              "operator",
                                                              "read",
                                                              "write",
                                                              "uminus"};
  inline static const std::set<std::string> IDENTIFIER = {
      "integer", "real", "char", "boolean", "text"};
};

#endif