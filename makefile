output: main.o nfa.o reg_exp.o dfa.o
	g++ -std=c++20 dfa.o nfa.o reg_exp.o main.o -o output 
main.o: main.cpp
	g++ -std=c++20 -c main.cpp
dfa.o: dfa.cpp
	g++ -std=c++20 -c dfa.cpp
nfa.o: nfa.cpp
	g++ -std=c++20 -c nfa.cpp
reg_exp.o: reg_exp.cpp
	g++ -std=c++20 -c reg_exp.cpp
test: output
	./output -test | tee program_output.txt
clean:
	rm -f *.o output output_no_color





 