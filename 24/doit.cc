// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

// This uses direct knowledge of the function of each step in the
// program.  The number being checked is in z.  The key search
// observation is that to eventually reach z == 0, every instruction
// where z could be multiplied by something has to be skipped.  Or
// equivalently, every step where z has a nontrivial divisor has to
// decrease z by a significant amount.

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

  // Read from cin
  step();

  // Return final z
  int execute(int z, char digit) const;

  // Should z have decreased?
  bool should_decrease() const { return zdiv != 1; }
};

step::step() {
  auto eat = [&](string const &tokens) {
    stringstream ss(tokens);
    string expected;
    while (ss >> expected) {
      string s;
      cin >> s;
      assert(s == expected);
    }
  };
  eat("inp w");
  eat("mul x 0");
  eat("add x z");
  eat("mod x 26");
  eat("div z");
  cin >> zdiv;
  assert(zdiv == 1 || zdiv == 26);
  eat("add x");
  cin >> xadd;
  eat("eql x w");
  eat("eql x 0");
  eat("mul y 0");
  eat("add y 25");
  eat("mul y x");
  eat("add y 1");
  eat("mul z y");
  eat("mul y 0");
  eat("add y w");
  eat("add");
  eat("y");
  cin >> yadd;
  eat("mul y x");
  eat("add z y");
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

// find_max: true for part 1 (max), false for part 2 (min)
bool search(int z, unsigned index, string &model_num, bool find_max) {
  if (index == program.size())
    return z == 0;
  char start = find_max ? '9' : '1';
  int dir = find_max ? -1 : 1;
  char end = start + 10 * dir;
  for (char digit = start; digit != end; digit += dir) {
    model_num[index] = digit;
    int znext = program[index].execute(z, digit);
    if (program[index].should_decrease() && znext > z / 2)
      continue;
    if (search(znext, index + 1, model_num, find_max))
      return true;
  }
  return false;
}

void solve(bool find_max) {
  for (int i = 0; i < 14; ++i)
    program.emplace_back(step());
  string model_num(program.size(), '0');
  bool found = search(0, 0, model_num, find_max);
  assert(found);
  cout << model_num << '\n';
}

void part1() { solve(true); }
void part2() { solve(false); }

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}
