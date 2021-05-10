default:
	$(MAKE) full

full: src/main.cpp src/parser.h src/parser.cpp src/lexer.h src/lexer.cpp src/syntax_tree.h src/syntax_tree.cpp include/json.hpp
	g++ src/parser.cpp src/lexer.cpp src/main.cpp src/syntax_tree.cpp -o bin/pas2c -Wall -std=c++17 && cd bin && ./pas2c

clean:
	$(RM) bin/pas2c
