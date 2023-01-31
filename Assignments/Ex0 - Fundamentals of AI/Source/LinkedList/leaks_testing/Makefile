

tests: main.o
	g++ -std=c++17 -O0 -g -Wall -o tests main.o

tests.o: main.cpp
	g++ -std=c++17 -O0 -g -Wall main.cpp

clean:
	rm tests main.o

memcheck:
	valgrind --leak-check=yes ./tests