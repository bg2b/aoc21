all: doit1 doit2

doit1: doit1.cc alu1.cc
	$(CXX) -O -g -o doit1 doit1.cc

doit2: doit2.cc alu1.cc
	$(CXX) -O -g -o doit2 doit2.cc

alu1.cc: doit1.cc
	rm -f alu1.cc
	touch alu1.cc
	$(CXX) -o doit1 doit1.cc
	./doit1 < input > alu1.cc

checker: compile.cc
	rm -f checker.cc
	$(CXX) -o compile compile.cc
	./compile < input > checker.cc
	$(CXX) -o checker checker.cc
