default:
	$(MAKE) full

full: src/main.cpp src/parser.h src/parser.cpp include/json.hpp
	g++ src/parser.cpp src/main.cpp -o bin/pas2c -Wall -std=c++17 && cd bin && ./pas2c

clean:
	$(RM) bin/pas2c
