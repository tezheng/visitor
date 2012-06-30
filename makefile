.PHONY: all

all:
	rm -rf bin
	mkdir bin
	g++ -Iinclude/ src/visitor.cpp example/helloworld.cpp -o bin/helloworld
