#include "symbol_table.h"

#include <spdlog/spdlog.h>

#include <iomanip>
#include <iostream>
using namespace std;

bool SymbolTableController::create_table(const string& subtable_name,
                                         const bool& is_func,
                                         const bool& is_record,
                                         const string& return_type,
                                         vector<Argument>& arguments_lists,
                                         int& declaration) {
  if (current_table != "" && current_table != "main") {
    return false;
  }

  auto it = table_lists.find(subtable_name);
  if (it == table_lists.end()) {
    vector<SymbolTableElement> element_lists;
    SymbolTable new_table(current_table, element_lists, return_type,
                          arguments_lists, subtable_name, is_func, false,
                          is_record);
    // table_lists[subtable_name] = new_table;
    table_lists.insert(make_pair(subtable_name, new_table));
    if (arguments_lists.size()) {
      for (auto& argument : arguments_lists) {
        string identifier_type;
        if (argument.pass_value == true) {
          identifier_type = "addr";
        } else {
          identifier_type = "var";
        }
        vector<Argument> new_arguments_lists;
        vector<int> new_use;
        SymbolTableElement new_element(argument.name, identifier_type,
                                       argument.type, "", new_arguments_lists,
                                       argument.row, new_use);
        insert_element2table(new_element, subtable_name);
      }
    }

    string func_type;
    if (subtable_name == "main")
      func_type = "program";
    else if (is_func == true)
      func_type = "function";
    else
      func_type = "procedure";
    if (is_record == true) func_type = "record";
    vector<int> new_use;
    SymbolTableElement new_element(subtable_name, func_type, return_type, "",
                                   arguments_lists, declaration, new_use,
                                   (int)arguments_lists.size());
    // insert_element2table(new_element, subtable_name);
    if (subtable_name != "main") {
      insert_element2table(new_element, "main");
    }
    locate_table(subtable_name);
    return true;
  } else
    return false;
}

bool SymbolTableController::insert_element2table(const SymbolTableElement& item,
                                                 const string& table_name) {
  auto it = table_lists.find(table_name);
  if (it != table_lists.end()) {  // table exists
    bool redefined = false;
    for (auto& element : table_lists[table_name].element_lists) {
      if (element.name == item.name) {
        redefined = true;
        break;
      }
    }
    if (redefined)
      return false;
    else {  // insert the element into the table
      table_lists[table_name].element_lists.push_back(item);
      return true;
    }
  } else  // table not exists
    return false;
}

SymbolTableElement SymbolTableController::search_table(
    const std::string& id_name, const std::string& table_name) {
  SymbolTableElement empty_element;
  if (table_lists[table_name].element_lists.size()) {
    for (auto& element : table_lists[table_name].element_lists) {
      if (element.name == id_name) {  // find element
        return element;
      }
    }

    if (table_lists[table_name].parent ==
        "") {  // no element and the table has no parent
      return empty_element;
    } else {
      string new_table_name = table_lists[table_name].parent;
      SymbolTableElement result = search_table(id_name, new_table_name);
      if (result.empty() == false)  // find element in previous table
        return result;
      else
        return empty_element;
    }
  } else {
    return empty_element;
  }
}

void SymbolTableController::locate_table(const string& table_name) {
  table_lists[table_name].enabled = true;
  current_table = table_name;
}

void SymbolTableController::relocate_table() {
  table_lists[current_table].enabled = false;
  current_table = table_lists[current_table].parent;
}

string SymbolTableController::check_parameters(
    const std::string& function, const vector<string>& arg_lists) {
  int arg_total = arg_lists.size();
  if (table_lists.find(function) != table_lists.end()) {
    int para_total = table_lists[function].arguments.size();
    bool is_correct = true;
    vector<string> real_para;
    for (auto& element : table_lists[function].arguments) {
      real_para.push_back(element.type);
    }
    is_correct = (real_para == arg_lists);
    if (arg_total == para_total && is_correct) {
      return table_lists[function].return_type;
    } else
      return "ERROR";
  } else
    return "ERROR";
}

// vector<int> SymbolTableController::check_bound(const string& id_name, const
// string& table_name){

// }

void SymbolTableController::print_table() {
  spdlog::debug("Symbol Table");
  if (spdlog::get_level() == spdlog::level::debug)
    for (auto& table : table_lists) {
      cout << "---------------------------------table << " << table.second.name
           << " >>-------------------------------------------------" << endl;
      cout << setw(5) << "parent: " << setw(10) << table.second.parent << endl;
      cout << setw(5) << "return type: " << setw(10) << table.second.return_type
           << endl;
      if (table.second.is_record)
        cout << setw(5) << "type of table: " << setw(10) << "record" << endl;
      else if (table.second.is_func)
        cout << setw(5) << "type of table: " << setw(10) << "function" << endl;
      else
        cout << setw(5) << "type of table: " << setw(10) << "procedure" << endl;
      cout << "elements in table: " << endl;
      for (auto& ele : table.second.element_lists) {
        cout << setw(10) << "name: " << setw(8) << ele.name;
        cout << setw(20) << "element type: " << setw(10) << ele.element_type;
        cout << setw(15) << "value type: " << setw(8) << ele.value_type;
        cout << setw(15) << "value: " << setw(2) << ele.value;
        cout << setw(15) << "dimension: " << setw(3) << ele.dimension;
        cout << setw(15) << "declare: " << setw(3) << ele.declare << endl;
        if (ele.arguments_lists.empty() == false) {
          cout << setw(26) << "arguments in element: " << endl;
          for (auto& arg : ele.arguments_lists) {
            if (arg.period_element.second) {
              cout << setw(15) << "start: " << setw(3)
                   << arg.period_element.first;
              cout << setw(15) << "end: " << setw(3)
                   << arg.period_element.second << endl;
              continue;
            }
            cout << setw(15) << "name: " << setw(8) << arg.name;
            cout << setw(15) << "type: " << setw(8) << arg.type;
            cout << setw(15) << "row: " << setw(3) << arg.row;
            if (arg.pass_value)
              cout << setw(25) << "transfer type: address" << endl;
            else
              cout << setw(25) << "transfer type: value" << endl;
          }
        }
        if (ele.use.empty() == false) {
          cout << "use of element: " << endl;
          for (auto& u : ele.use) {
            cout << setw(20) << u;
          }
          cout << endl;
        }
      }
      if (table.second.arguments.empty() == false) {
        cout << "arguments in table: " << endl;
        for (auto& arg : table.second.arguments) {
          cout << setw(10) << "name: " << setw(8) << arg.name;
          cout << setw(15) << "type: " << setw(8) << arg.type;
          cout << setw(15) << "row: " << setw(8) << arg.row;
          if (arg.pass_value)
            cout << setw(25) << "transfer type: address" << endl;
          else
            cout << setw(25) << "transfer type: value" << endl;
        }
      }
    }
}

/*
  Interface for code_generator
*/
bool SymbolTableController::is_addr(std::string id_name,
                                    std::string table_name) {
  if (table_lists.find(table_name) != table_lists.end()) {
    vector<Argument> arguments = table_lists[table_name].arguments;
    for (auto argument : arguments) {
      if (id_name == argument.name) return argument.pass_value;
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

std::vector<Argument> SymbolTableController::get_range(std::string array_name,
                                                       std::string table_name) {
  SymbolTableElement result_item = search_table(array_name, table_name);
  std::vector<Argument> arguments_lists;
  if (!result_item.empty() && (result_item.element_type == "array"))
    arguments_lists = result_item.arguments_lists;
  return arguments_lists;
}

std::string SymbolTableController::get_type(std::string id_name,
                                            std::string table_name) {
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
