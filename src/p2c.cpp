#include <unistd.h>

#include <cstring>

#include "code_generator.h"
#include "parser.h"
#include "semantic_analyzer.h"

using namespace std;  // recommend only use it in .cpp

string VERSION_INFO = "pas2c version 2.1.0";
string GRAMMAR_FILE = "../include/grammar.json";
string INPUT_FILE = "../example/quicksort.pas";
string OUTPUT_FILE = "./out.c";
string LOG_FILE = "./log.txt";

bool IS_DEBUG = false;
bool IS_LOG = false;

static void show_usage(char *argv[]) {
  cerr << "Usage: " << argv[0] << " [-d] [-h] [-l] [-o <file>] [-v]" << endl;
  cerr << "Where:  -d                     Display debug information" << endl;
  cerr << "        -h                     Display help information." << endl;
  cerr << "        -l                     Create log file" << endl;
  cerr << "        -o <file>              Place the output into <file>" << endl;
  cerr << "        -v                     Display compiler version information."
       << endl;
}

void parsing_parameters(int argc, char *argv[]) {
  if (argc < 2) {  // We expect 3 arguments: the program name, the source path
                   // and the destination path
    show_usage(argv);
    exit(-1);
  }
  int o;
  // 有三个选项-abc，其中c选项后有冒号，所以后面必须有参数
  const char *optstring = "dhlo:v";
  while ((o = getopt(argc, argv, optstring)) != -1) {
    switch (o) {
      case 'd':
        IS_DEBUG = true;
        cout << "DEBUG is on." << endl;
        break;
      case 'l':
        IS_LOG = true;
        cout << "LOG is on." << endl;
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

  // cout << "\n*** Section 0 ***\n";
  // Parser parser(GRAMMAR_FILE, INPUT_FILE);
  // SyntaxTree tree = parser.generate_tree();

  // cout << "\n*** Section 1 ***\n";
  // SemanticAnalyzer semantic_analyzer(tree);
  // semantic_analyzer.print_table();

  // cout << "\n*** Section 2 ***\n";
  // CodeGenerator code_generator(tree.node_dic, semantic_analyzer);
  // cout << code_generator.run() << endl;

  // return 0;
}
