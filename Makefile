
build: src/main.cpp src/program.cpp src/tokenization/tokenize.cpp src/parsing/parser.cpp 
	g++ src/main.cpp src/program.cpp src/tokenization/tokenize.cpp src/parsing/parser.cpp -o parser
run:
	./parser example.txt ast.txt