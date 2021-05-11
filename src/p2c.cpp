#include <spdlog/spdlog.h>
#include <unistd.h>

#include <cstring>

#include "code_generator.h"
#include "parser.h"
#include "semantic_analyzer.h"

using namespace std;  // recommend only use it in .cpp

string VERSION_INFO = "Pas2c version 2.1.0";
string GRAMMAR_FILE = "../include/grammar.json";
string INPUT_FILE = "../example/quicksort.pas";
string OUTPUT_FILE = "./out.c";
string LOG_FILE = "./log.txt";

bool IS_DEBUG = false;
bool IS_LOG = false;

static void show_usage(char *argv[]) {
  cerr << "Usage: " << argv[0] << " [-d] [-h] [-l] [-o <file>] [-v]" << endl;
  cerr << "Where:  -d                     Display debug information." << endl;
  cerr << "        -g                     Set Pascal-S grammar file." << endl;
  cerr << "        -h                     Display help information." << endl;
  cerr << "        -l                     Create log file." << endl;
  cerr << "        -o <file>              Place the output into <file>." << endl;
  cerr << "        -v                     Display compiler version information."
       << endl;
}

void parsing_parameters(int argc, char *argv[]) {
  if (argc < 2) {  // We expect 3 arguments: the program name, the source path
                   // and the destination path
    show_usage(argv);
    exit(-1);
  }
  INPUT_FILE = string(argv[1]);
  spdlog::info("Input: {}", INPUT_FILE);

  int opt;
  // 选项后有冒号，表示必须后面有参数
  const char *optstring = "dghlo:v";
  while ((opt = getopt(argc, argv, optstring)) != -1) {
    switch (opt) {
      case 'd':
        IS_DEBUG = true;
        cout << "DEBUG is on." << endl;
        break;
      case 'g':
        GRAMMAR_FILE = string(optarg);
        cout << "Set Pascal-S grammar to " << GRAMMAR_FILE << endl;
        break;
      case 'l':
        IS_LOG = true;
        cout << "Log is output to log.txt" << endl;
        break;
      case 'o':
        OUTPUT_FILE = string(optarg);
        cout << "Place the output to " << OUTPUT_FILE << endl;
        break;
      case 'v':
        cout << VERSION_INFO << endl;
        exit(-1);
      case 'h':
      default:
        show_usage(argv);
        exit(-1);
    }
  }
}

int main(int argc, char *argv[]) {
  parsing_parameters(argc, argv);
  spdlog::info("Welcome to spdlog!");
  spdlog::error("Some error message with arg: {}", 1);

  spdlog::warn("Easy padding in numbers like {:08d}", 12);
  spdlog::critical(
      "Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
  spdlog::info("Support for floats {:03.2f}", 1.23456);
  spdlog::info("Positional args are {1} {0}..", "too", "supported");
  spdlog::info("{:<30}", "left aligned");

  spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
  spdlog::debug("This message should be displayed..");

  // change log pattern
  spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

  // Compile time log levels
  // define SPDLOG_ACTIVE_LEVEL to desired level
  SPDLOG_TRACE("Some trace message with param {}", 42);
  SPDLOG_DEBUG("Some debug message");

  // cout << "\n*** Section 0 ***\n";
  // Parser parser(GRAMMAR_FILE, INPUT_FILE);
  // SyntaxTree tree = parser.generate_tree();

  // cout << "\n*** Section 1 ***\n";
  // SemanticAnalyzer semantic_analyzer(tree);
  // semantic_analyzer.print_table();

  // cout << "\n*** Section 2 ***\n";
  // CodeGenerator code_generator(tree.node_dic, semantic_analyzer);
  // cout << code_generator.run() << endl;

  return 0;
}
