#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <map>
#include <string>
#include <vector>

struct Argument{
  Argument(std::string name, std::string type, int row, bool pass_value = false){
    this->name = name;
    this->type = type;
    this->pass_value = pass_value;
    this->row = row;
  }
  Argument(std::pair<int, int> period_element){
    this->period_element = period_element;
  }
  std::string name; // parameter name
  std::string type; // parameter type
  bool pass_value = false; // true for transfer address, false for transfer value，default is false
  int row;  // the row number of parameters
  std::pair<int, int> period_element;
};

struct SymbolTableElement{
  SymbolTableElement(std::string name, std::string element_type, std::string value_type, std::string value, std::vector<Argument > arguments_lists,
                      int declare, std::vector<int > use, int dimension = 0){
                        this->name = name;
                        this->element_type = element_type;
                        this->value_type = value_type;
                        this->dimension = dimension;
                        this->value = value;
                        this->arguments_lists = arguments_lists;
                        this->declare = declare;
                        this->use = use;
                      }
  SymbolTableElement(){
    this->name = "";
    this->element_type = "";
    this->value_type = "";
    this->value = "";
    this->dimension = 0;
  }
  bool empty(){
    if(this->name == "" && this->element_type == "" && this->value_type == "" && this->value == "" && this->dimension == 0)
      return true;
    else
      return false;
  }
  std::string name; // identifier name
  std::string element_type; // identifier type, e.g FUNCTION PROCEDURE
  std::string value_type; 
  int dimension;  // the dimension of the array, or the number of parameters of a function or procedure
  std::string value; // according to the value of value_type
  std::vector<Argument > arguments_lists;  // argument list
  int declare;  // the position of declaration
  std::vector<int > use;  // the position being used
};

struct SymbolTable{
  SymbolTable(std::string parent, std::vector<SymbolTableElement > element_lists,  
               std::string return_type, std::vector<Argument > arguments,
               std::string name = "", bool is_func = false, bool enabled = false){
                 this->parent = parent;
                 this->element_lists = element_lists;
                 this->name = name;
                 this->is_func = is_func;
                 this->enabled = enabled;
                 this->return_type = return_type;
                 this->arguments = arguments;
  }
  SymbolTable(){}
  std::string parent; // parent symbol table name
  std::vector<SymbolTableElement > element_lists; // lists of table elements
  std::string name; // symbol table name
  bool is_func = false;  // true for functoin, false for procedure
  bool enabled = false;  // flag for direction and redirection
  std::string return_type; 
  std::vector<Argument > arguments;  // argument list
};

class SymbolTableController
{
  private:
    std::map<std::string, SymbolTable> table_lists;  // using table name to find its table
    
  public:
    bool create_table(const std::string&, const bool&, const std::string&, std::vector<Argument >&);
    bool insert_element2table(const SymbolTableElement&, const std::string& );
    SymbolTableElement search_table(const std::string&, const std::string& );
    void locate_table(const std::string&);
    void relocate_table();
    std::string current_table;  // current table name

    /*
      Interface for code_generator
    */
    bool is_addr(std::string id_name, std::string table_name);  // 确定某一个标识符传参方式
    bool is_func(std::string id_name);  // 判断是函数还是过程
    std::vector<Argument> get_range(std::string array_name, std::string table_name);  // 确定数组的上下界
    std::string get_type(std::string id_name, std::string table_name);  // 获取值的类型
    std::vector<bool> get_args(std::string table_name);  // 获取参数列表
};

#endif