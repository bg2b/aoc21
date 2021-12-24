#include <iostream>
#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;

// Some inspection shows that program consists of 14 steps of two
// types:
//
// Increasing   Decreasing
//  inp w        inp w   
//  mul x 0      mul x 0 
//  add x z      add x z 
//  mod x 26     mod x 26
//  div z 1      div z 26
//  add x 13     add x -2
//  eql x w      eql x w 
//  eql x 0      eql x 0 
//  mul y 0      mul y 0 
//  add y 25     add y 25
//  mul y x      mul y x 
//  add y 1      add y 1 
//  mul z y      mul z y 
//  mul y 0      mul y 0 
//  add y w      add y w 
//  add y 12     add y 3 
//  mul y x      mul y x 
//  add z y      add z y 
//
// The increasing steps are basically z = z * 26 + digit + ...
//
// The decreasing has a conditional effect, depending on the value of
// z % 26.  z will be updated to something like z / 26 if the mod
// result has a specific value.
//
// There are seven increasing steps and seven decreasing ones.  For
// the model number to be valid, each decreasing step must hit the
// correct mod 26 value.
//
// Perhaps one could be clever and work out exactly what's going on.
// But "computers are made of iron, let them sweat..."  All we need to
// do is cut down the search space by some reasonable amount.  That
// can be done by starting the ALU simulation to scan the digits, and
// as soon as one of the decreasing steps fails to decrease z
// significantly, we know all numbers with that prefix are invalid.
// E.g., say we're scanning
//   71856...
//       ^ first decreasing step uses this digit
//
// If z doesn't decrease by a lot, nothing in the ... can make things
// valid.  If we're trying to find the largest valid model number the
// search can skip from 71856... directly to 71855999...  (If trying
// to find the smallest valid number, go to 71857111...)
//
// The compile() function turns the input into C++ code to simulate
// the ALU and return the position of the first digit that makes a
// decreasing step fail.  That gets included in the program and used
// by search() to find the relevant model number.

void compile(istream &in) {
  printf("#define HAVE_ALU\n");
  printf("int alu(char const *model_num) {\n");
  printf("long w = 0, x = 0, y = 0, z = 0;\n");
  printf("long zin = 0;\n");
  int step = 0;
  bool must_decrease = false;
  string cmd;
  char reg;
  while (in >> cmd >> reg) {
    assert(reg == 'w' || reg == 'x' || reg == 'y' || reg == 'z');
    if (cmd == "inp") {
      if (must_decrease)
	// If there's an invalid model number then z could possibly
	// could possibly decrease by a little (we did something like
	// z / 26 * 26 + ...), but it won't change radically.  We need
	// a big drop to continue.
	printf("if (z >= zin / 2) return %d;\n", step - 1);
      must_decrease = false;
      printf("zin = z;\n");
      printf("%c = model_num[%d] - '0';\n", reg, step++);
      continue;
    }
    string op;
    cin >> op;
    char const *opstr = op.c_str();
    if (cmd == "add")
      printf("%c += %s;\n", reg, opstr);
    else if (cmd == "mul")
      printf("%c *= %s;\n", reg, opstr);
    else if (cmd == "div") {
      printf("%c /= %s;\n", reg, opstr);
      if (reg == 'z' && op == "26")
	must_decrease = true;
    }
    else if (cmd == "mod")
      printf("%c %%= %s;\n", reg, opstr);
    else {
      assert(cmd == "eql");
      printf("%c = (%c == %s);\n", reg, reg, opstr);
    }
  }
  printf("if (z != 0) return %d;\n", step - 1);
  printf("return -1;\n");
  printf("}\n");
}

#include "alu1.cc"

#ifdef HAVE_ALU
string search() {
  string model_num(14, '9');
  while (true) {
    int wrong = alu(model_num.c_str());
    if (wrong == -1)
      return model_num;
    assert(wrong >= 0 && wrong < int(model_num.size()));
    // Everything to the right of wrong should reset to 9
    for (unsigned reset = wrong + 1; reset < model_num.size(); ++reset)
      model_num[reset] = '9';
    while (wrong >= 0 && model_num[wrong] == '1')
      // Borrow
      model_num[wrong--] = '9';
    // Should not have borrowed off the end
    assert(wrong >= 0 && model_num[wrong] >= '2');
    --model_num[wrong];
  }
}
#endif

int main(int argc, char **argv) {
#ifdef HAVE_ALU
  printf("Max valid model number %s\n", search().c_str());
#else
  compile(cin);
#endif
  return 0;
}
