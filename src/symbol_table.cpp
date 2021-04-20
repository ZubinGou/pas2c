#include "symbol_table.h"
using namespace std;

bool SymbolTableController::create_table(const string& subtable_name, const bool& is_func,
                                         const string& return_type, vector<Argument >& arguments_lists){
  if(current_table != "" && current_table != "main"){
    return false;
  }

  auto it = table_lists.find(subtable_name);
  if(it == table_lists.end()){
    vector<SymbolTableElement> element_lists;
    SymbolTable new_table(current_table, element_lists, return_type, arguments_lists, subtable_name, 
                                              is_func, false);
    //table_lists[subtable_name] = new_table;
    table_lists.insert(make_pair(subtable_name, new_table));
    if(arguments_lists.size()){
      for(auto& argument : arguments_lists){
        string identifier_type;
        if(argument.pass_value == true){
          identifier_type = "addr";
        }
        else{
          identifier_type = "var";
        }
        vector<Argument> new_arguments_lists;
        vector<int> new_use;
        SymbolTableElement new_element(argument.name, identifier_type, 
                                          argument.type, NULL, new_arguments_lists, argument.row, new_use);
        insert_element2table(new_element, subtable_name);
      }
    }

    string func_type;
    if(subtable_name == "main")
      func_type = "program";
    else if(is_func == true)
      func_type = "function";
    else
      func_type = "procedure";
    vector<int> new_use;
    SymbolTableElement new_element(subtable_name, func_type, return_type, NULL, arguments_lists, 0, new_use, 
                                          arguments_lists.size());
    insert_element2table(new_element, subtable_name);
    if(subtable_name != "main"){
      insert_element2table(new_element, "main");
    }
    locate_table(subtable_name);
    return true;
  }
  else
    return false;
}

bool SymbolTableController::insert_element2table(const SymbolTableElement& item, const string& table_name){
  auto it = table_lists.find(table_name);
  if(it != table_lists.end()){  // table exists
    bool redefined = false;
    for(auto& element : table_lists[table_name].element_lists){
      if(element.name == item.name){
        redefined = true;
        break;
      }
    }
    if(redefined)
      return false;
    else{ // insert the element into the table
      table_lists[table_name].element_lists.push_back(item);
      return true;
    }
  }
  else  // table not exists
    return false;
}

SymbolTableElement SymbolTableController::search_table(const std::string& id_name, const std::string& table_name){
  SymbolTableElement empty_element;
  if(table_lists[table_name].element_lists.size()){
    for(auto& element : table_lists[table_name].element_lists){
      if(element.name == id_name){  // find element
        return element;
      }    
    }

    if(table_lists[table_name].parent == ""){ // no element and the table has no parent
      return empty_element;
    }
    else{
      string new_table_name = table_lists[table_name].parent;
      SymbolTableElement result = search_table(id_name, new_table_name);
      if(result.empty() == false) // find element in previous table
        return result;
      else 
        return empty_element;
    }
  }
  else{
    return empty_element;
  }
}

void SymbolTableController::locate_table(const string& table_name){
  table_lists[table_name].enabled = true;
  current_table = table_name;
}

void SymbolTableController::relocate_table(){
  table_lists[current_table].enabled = false;
  current_table = table_lists[current_table].parent;
}


/*
  Interface for code_generator
*/
bool SymbolTableController::is_addr(std::string id_name, std::string table_name) {
  if (table_lists.find(table_name) != table_lists.end()) {
    vector<Argument> arguments = table_lists[table_name].arguments;
    for (auto argument : arguments) {
      if (id_name == argument.name)
        return argument.pass_value;
    }
  }
  return false;
  // else
  //     "函数未声明"
}

bool SymbolTableController::is_func(std::string id_name) {
  SymbolTableElement result_item = search_table(id_name, "main");
  return (result_item.empty() && (result_item.element_type == "function"));
}

std::vector<Argument> SymbolTableController::get_range(std::string array_name, std::string table_name) {
  SymbolTableElement result_item = search_table(array_name, table_name);
  std::vector<Argument> arguments_lists;
  if (!result_item.empty() && (result_item.element_type == "array"))
    arguments_lists = result_item.arguments_lists;
  return arguments_lists;
}

std::string SymbolTableController::get_type(std::string id_name, std::string table_name) {
    SymbolTableElement result_item = search_table(id_name, table_name);
    // if (result_item.empty())
    //     '未定义'
    //     return;
    return result_item.value_type;
}

std::vector<bool> SymbolTableController::get_args(std::string table_name) {
  std::vector<bool> args;
  if (table_lists.find(table_name) != table_lists.end())
    for (auto item : table_lists[table_name].arguments)
      args.push_back(item.pass_value);
  return args;
}



