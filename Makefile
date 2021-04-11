default:
	$(MAKE) full

full: src/main.cpp src/parser.h src/parser.cpp src/lexer.h src/lexer.cpp src/syntaxtree.h src/syntaxtree.cpp include/json.hpp
	g++ src/parser.cpp src/lexer.cpp src/main.cpp src/syntaxtree.cpp -o bin/pas2c -Wall -std=c++17 && cd bin && ./pas2c

clean:
	$(RM) bin/pas2c
