//============================================================================
// @Name : p2c.cpp
// @Author : Zubin Gou
// @Version : 1.0
// @Copyright : 2021, BUPT
// @Description : command line tool to interprete Pascal-S to C
//============================================================================
#include "code_generator.h"
#include "parser.h"
#include "semantic_analyzer.h"
// #include <getop
#include <cgetopt>

using namespace std;  // recommend only use it in .cpp

const string GRAMMAR_FILE = "../include/grammar.json";
const string INPUT_FILE = "../example/quicksort.pas";

static void show_usage(char *argv[]) {
  std::cerr << "Usage: " << argv[0] << " <option(s)> SOURCES"
            << "Options:\n"
            << "\t-h,--help\t\tShow this help message\n"
            << "\t-d,--destination DESTINATION\tSpecify the destination path"
            << std::endl;
  std::cerr << "Usage: " << argv[0] << "SOURCE [-o DESTINATION] [-l LOG_FILE]"
            << std::endl;
  exit(1);
}

void parsing_parameters(int argc, char *argv[]) {
  if (argc < 2) show_usage(argv);
  int o;
  const char *optstring = "o:p:s:";  // 有三个选项-abc，其中c选项后有冒号，所以后面必须有参数
  while ((o = getopt(argc, argv, optstring)) != -1) {
    switch (o) {
      case 'd':
        DEBUG = true;
        printf("DEBUG is on.\n");
        break;
      case 'l':
        LOG = true;
        printf("LOG is on.\n");
        break;
      case 'f':
        strcpy(DNS_TABLE_FILE, optarg);
        break;
      case 'p':
        PORT = atoi(optarg);
        break;
      case 's':
        strcpy(PUBLIC_DNS_IP, optarg);
        break;
      case 'h':
      default:
        printf(
            "Usage:  dnsrelay [-d] [-h] [-l] [-p <port>] [-f <db-file>] [-s "
            "<dns-server>]\n");
        printf("Where:  -d                     (print debug information)\n");
        printf("        -h                     (print help)\n");
        printf("        -l                     (print log)\n");
        printf("        -f db-file             (specify DNS table file)\n");
        printf("        -p port                (specify port number)\n");
        printf("        -s dns-server          (specify DNS server)\n");
        exit(-1);
    }
  }
}

int main(int argc, char *argv[]) {
  parsing_parameters(argc, argv);
  if (argc < 2) {  // We expect 3 arguments: the program name, the source path
                   // and the destination path
    return 1;
  }

  cout << "\n*** Section 0 ***\n";
  Parser parser(GRAMMAR_FILE, INPUT_FILE);
  SyntaxTree tree = parser.generate_tree();

  cout << "\n*** Section 1 ***\n";
  SemanticAnalyzer semantic_analyzer(tree);
  semantic_analyzer.print_table();

  cout << "\n*** Section 2 ***\n";
  CodeGenerator code_generator(tree.node_dic, semantic_analyzer);
  cout << code_generator.run() << endl;

  return 0;
}
