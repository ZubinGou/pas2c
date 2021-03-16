default:
	$(MAKE) full

full: src/main.cpp src/parser.h src/parser.cpp
	g++ src/parser.cpp src/main.cpp -o bin/pas2c -Wall -std=c++17

clean:
	$(RM) bin/pas2c
