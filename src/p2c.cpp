#include <spdlog/sinks/basic_file_sink.h>     // support for basic file logging
#include <spdlog/sinks/rotating_file_sink.h>  // support for rotating file logging
#include <spdlog/spdlog.h>

#include <cstring>
#include <fstream>

#include "code_generator.h"
#include "config.h"
#include "getopt.h"
#include "parser.h"
#include "semantic_analyzer.h"

using namespace std;  // recommend only use it in .cpp

#ifdef _WIN32
string GRAMMAR_FILE = "..\\include\\grammar.json";
string INPUT_FILE;
string OUTPUT_FILE = "..\\output\\";
string LOG_FILE = ".\\log.txt";
#else
string GRAMMAR_FILE = "../include/grammar.json";
string INPUT_FILE;
string OUTPUT_FILE = "../output/";
string LOG_FILE = "./log.txt";
#endif

bool IS_DEBUG = false;
bool IS_LOG = false;

bool has_suffix(const string &str, const string &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

string get_base_name(string const &path) {
  return path.substr(path.find_last_of("/\\") + 1);
}

static void show_usage(char *argv[]) {
  cerr << "Usage: " << argv[0]
       << " src.pas [-d] [-g] [-h] [-l] [-o <file>] [-v]" << endl;
  cerr << "Where:  -d                     Display debug information." << endl;
  cerr << "        -g <file>              Set Pascal-S grammar file." << endl;
  cerr << "        -h                     Display help information." << endl;
  cerr << "        -l                     Output to log file." << endl;
  cerr << "        -o <file>              Place the output into <file>."
       << endl;
  cerr << "        -v                     Display compiler version information."
       << endl;
}

void parsing_parameters(int argc, char *argv[]) {
  // We expect 2 arguments: the program name, the source path
  if (argc < 2 || !has_suffix(string(argv[1]), ".pas")) {
    show_usage(argv);
    exit(0);
  }

  INPUT_FILE = string(argv[1]);
  spdlog::info("src: {}", INPUT_FILE);
  string base_name = get_base_name(INPUT_FILE);
  if (base_name.length() <= 4) {
    spdlog::error("src file must be xxx.pas");
    exit(-1);
  }
  OUTPUT_FILE += base_name.substr(0, base_name.length() - 4) + ".c";

  int opt;
  optind++;  // ignore the parameter: src.pas
  // ????????????????????????????????????????????????
  const char *optstring = "dg:hlo:v";
  while ((opt = getopt(argc, argv, optstring)) != -1) {
    switch (opt) {
      case 'd':
        IS_DEBUG = true;
        spdlog::info("DEBUG is on.");
        break;
      case 'g':
        GRAMMAR_FILE = string(optarg);
        spdlog::info("Set Pascal-S grammar to {}", GRAMMAR_FILE);
        break;
      case 'l':
        IS_LOG = true;
        spdlog::info("Log is output to {}", LOG_FILE);
        break;
      case 'o':
        OUTPUT_FILE = string(optarg);
        spdlog::info("Place the output to {}", OUTPUT_FILE);
        break;
      case 'v':
        cout << PROJECT_NAME << " version: " << PROJECT_VERSION << endl;
        exit(0);
      case 'h':
      default:
        show_usage(argv);
        exit(0);
    }
  }
}

int main(int argc, char *argv[]) {
  parsing_parameters(argc, argv);

  if (IS_LOG) {
    try {
      // auto file_logger = spdlog::basic_logger_mt("@", LOG_FILE);
      auto file_logger = spdlog::rotating_logger_mt("@", LOG_FILE, 1024 * 1024 * 5, 1, true);
      spdlog::set_default_logger(file_logger);
      // freopen(LOG_FILE.c_str(), "w", stdout);
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
  }

  if (IS_DEBUG)
    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug

  // change log pattern
  // spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
  spdlog::debug("Grammar file: {}", GRAMMAR_FILE);
  spdlog::debug("Input file: {}", INPUT_FILE);
  spdlog::debug("Output file: {}", OUTPUT_FILE);
  spdlog::debug("Log file: {}", LOG_FILE);

  spdlog::info("**** Section 0: Lexer & Parser ****");
  Parser parser(GRAMMAR_FILE, INPUT_FILE);
  SyntaxTree tree = parser.generate_tree();

  spdlog::info("**** Section 1: Semantic Analysis ****");
  SemanticAnalyzer semantic_analyzer(tree);
  semantic_analyzer.print_table();

  spdlog::info("**** Section 2: Code Generation ****");
  CodeGenerator code_generator(tree.node_dic, semantic_analyzer);
  string target_code = code_generator.run();
  spdlog::debug(target_code);

  spdlog::info("Compiled to {}.", OUTPUT_FILE);
  std::ofstream target_out(OUTPUT_FILE);
  target_out << target_code;
  target_out.close();

  return 0;
}