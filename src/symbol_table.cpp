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
    table_lists[subtable_name] = SymbolTable(current_table, element_lists, subtable_name, 
                                              is_func, false, return_type, arguments_lists);

    if(arguments_lists.size()){
      for(auto argument : arguments_lists){
        string identifier_type;
        if(argument.pass_value == true){
          identifier_type = "addr";
        }
        else{
          identifier_type = "var";
        }
        vector<Argument> new_arguments_lists;
        vector<int> new_use;
        SymbolTableElement new_element = SymbolTableElement(argument.name, identifier_type, 
                                          argument.type, NULL, NULL, new_arguments_lists, argument.row, new_use);
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
    SymbolTableElement new_element = SymbolTableElement(subtable_name, func_type, return_type, 
                                          arguments_lists.size(), NULL, arguments_lists, NULL, new_use);
    insert_element2table(new_element, subtable_name);
    if(subtable_name != "main"){
      insert_element2table(new_element, "main");
    }
    locate_table(subtable_name);
  }
  else
    return false;
}

bool SymbolTableController::insert_element2table(const SymbolTableElement& item, const string& table_name){
  auto it = table_lists.find(table_name);
  if(it != table_lists.end()){  // table exists
    bool redefined = false;
    for(auto element : table_lists[table_name].element_lists){
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
  if(table_lists[table_name].element_lists.size()){
    for(auto element : table_lists[table_name].element_lists){
      if(element.name == id_name){  // find element
        return element;
      }    
    }

    if(table_lists[table_name].parent == ""){ // no element and the table has no parent
      return ;
    }
    else{
      string new_table_name = table_lists[table_name].parent;
      SymbolTableElement result = search_table(id_name, new_table_name);
      if(result.name != "") // find element in previous table
        return result;
      else 
        return ;
    }
  }
  else{
    return ;
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



