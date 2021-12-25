#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>

using namespace std;

// More intelligent solution using some knowledge of the function of
// each block

struct step {
  // The function of a step is:
  // x = z % 26 + xadd
  // z /= zdiv
  // if (x != digit)
  //   z = 26 * z + digit + yadd
  //
  // What z is divided by (either 1 or 26)
  int zdiv;
  // Extra amount added to x
  int xadd;
  // Extra amount added to y
  int yadd;

  step(istream &in);

  // Return final z
  int execute(int z, char digit) const;

  // Should z have decreased?
  bool should_decrease() const { return zdiv != 1; }
};

step::step(istream &in) {
  string cmd;
  char reg;
  string op;
  in >> cmd >> reg; assert(cmd == "inp" && reg == 'w');
  in >> cmd >> reg >> op; assert(cmd == "mul" && reg == 'x' && op == "0");
  in >> cmd >> reg >> op; assert(cmd == "add" && reg == 'x' && op == "z");
  in >> cmd >> reg >> op; assert(cmd == "mod" && reg == 'x' && op == "26");
  in >> cmd >> reg >> zdiv; assert(cmd == "div" && reg == 'z');
  assert(zdiv == 1 || zdiv == 26);
  in >> cmd >> reg >> xadd; assert(cmd == "add" && reg == 'x');
  in >> cmd >> reg >> op; assert(cmd == "eql" && reg == 'x' && op == "w");
  in >> cmd >> reg >> op; assert(cmd == "eql" && reg == 'x' && op == "0");
  in >> cmd >> reg >> op; assert(cmd == "mul" && reg == 'y' && op == "0");
  in >> cmd >> reg >> op; assert(cmd == "add" && reg == 'y' && op == "25");
  in >> cmd >> reg >> op; assert(cmd == "mul" && reg == 'y' && op == "x");
  in >> cmd >> reg >> op; assert(cmd == "add" && reg == 'y' && op == "1");
  in >> cmd >> reg >> op; assert(cmd == "mul" && reg == 'z' && op == "y");
  in >> cmd >> reg >> op; assert(cmd == "mul" && reg == 'y' && op == "0");
  in >> cmd >> reg >> op; assert(cmd == "add" && reg == 'y' && op == "w");
  in >> cmd >> reg >> yadd; assert(cmd == "add" && reg == 'y');
  in >> cmd >> reg >> op; assert(cmd == "mul" && reg == 'y' && op == "x");
  in >> cmd >> reg >> op; assert(cmd == "add" && reg == 'z' && op == "y");
}

int step::execute(int z, char digit) const {
  digit -= '0';
  int x = z % 26 + xadd;
  z /= zdiv;
  if (x != digit)
    z = 26 * z + digit + yadd;
  return z;
}

vector<step> program;

bool search(int z, unsigned index, string &model_num) {
  if (index == program.size())
    return z == 0;
  for (char digit = '9'; digit >= '1'; --digit) {
    model_num[index] = digit;
    int znext = program[index].execute(z, digit);
    if (program[index].should_decrease() && znext > z / 2)
      continue;
    if (search(znext, index + 1, model_num))
      return true;
  }
  return false;
}

int main(int argc, char **argv) {
  for (int i = 0; i < 14; ++i)
    program.emplace_back(step(cin));
  string model_num(program.size(), '0');
  bool found = search(0, 0, model_num);
  assert(found);
  printf("Maximum model number %s\n", model_num.c_str());
  return 0;
}
