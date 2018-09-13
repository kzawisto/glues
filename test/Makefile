


.PHONY: clean all

all: test


%.o: %.cxx $(shell find ../include -type f)
	g++ -std=c++11 -c $< -o $@ -I../include


test: test_basic.o
	g++ -std=c++11 $^ -o $@ -lgtest -lpthread


clean: 
	rm test
	find .. -type -f -name "*.o" -exec echo {} \;
