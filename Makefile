
all: src/main.cpp src/tokenization/tokenize.cpp src/parsing/parser.cpp 
	g++ src/main.cpp src/tokenization/tokenize.cpp src/parsing/parser.cpp -o parser
	./parser