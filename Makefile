<<<<<<< HEAD
default:
	$(MAKE) full

full: src/main.cpp src/parser.h src/parser.cpp src/lexer.h src/lexer.cpp src/syntax_tree.h src/syntax_tree.cpp include/json.hpp
	g++ src/parser.cpp src/lexer.cpp src/main.cpp src/syntax_tree.cpp -o bin/pas2c -Wall -std=c++17 && cd bin && ./pas2c

clean:
	$(RM) bin/pas2c
=======
default:
	$(MAKE) full

full: src/main.cpp src/parser.h src/parser.cpp src/lexer.h src/lexer.cpp src/syntax_tree.h src/syntax_tree.cpp include/json.hpp
	g++ src/parser.cpp \
			src/lexer.cpp \
			src/main.cpp \
			src/syntax_tree.cpp \
			src/semantic_analyzer.cpp \
			src/symbol_table.cpp \
			src/code_generator.cpp \
			-o bin/pas2c -Wall -std=c++17 \
			&& cd bin && ./pas2c

test: src/test.cpp src/parser.h src/parser.cpp src/lexer.h src/lexer.cpp src/syntax_tree.h src/syntax_tree.cpp include/json.hpp
	g++ src/parser.cpp \
			src/lexer.cpp \
			src/test.cpp \
			src/syntax_tree.cpp \
			src/semantic_analyzer.cpp \
			src/symbol_table.cpp \
			src/code_generator.cpp \
			-o bin/pas2c_test -Wall -std=c++17 \
			&& cd bin && ./pas2c_test

clean:
	$(RM) bin/pas2c
>>>>>>> 3bb52d9d02d4c0dd7ba50b046b91eb650f11dca0
