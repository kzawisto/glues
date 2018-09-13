


.PHONY: clean all

all: test


%.o: %.cxx $(shell find ../include -type f)
	g++ -std=c++14 -c $< -o $@ -I../include


test: test_basic.o
	g++ -std=c++14 $^ -o $@ -lgtest -lpthread


clean: 
	rm test
	find .. -type -f -name "*.o" -exec echo {} \;
