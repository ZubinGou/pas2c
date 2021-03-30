#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>

struct Argument{
  Argument(std::string name, std::string type, bool pass_value, int row){
    this->name = name;
    this->type = type;
    this->pass_value = pass_value;
    this->row = row;
  }
  std::string name;
  std::string type;
  bool pass_value = false; // true为传地址, false为传值，默认为传值
  int row; 
};

struct SymbolTableElement{
  SymbolTableElement(std::string name, std::string element_type, std::string value_type, 
                      int dimension, std::string value, std::vector<struct Argument > arguments,
                      int declare, std::vector<int > use){
                        this->name = name;
                        this->element_type = element_type;
                        this->value_type = value_type;
                        this->dimension = dimension;
                        this->value = value;
                        this->arguments = arguments;
                        this->declare = declare;
                        this->use = use;
                      }
  std::string name;
  std::string element_type;
  std::string value_type;
  int dimension;
  std::string value; // according to the value of value_type
  std::vector<struct Argument > arguments;
  int declare;
  std::vector<int > use;
};

struct SymbolTable{
  
};

#endif