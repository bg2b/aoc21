// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <cassert>

using namespace std;

void solve(bool part2) {
  int horiz = 0;
  int depth = 0;
  int aim = 0;
  auto &updown = part2 ? aim : depth;
  string dir;
  int amnt;
  while (cin >> dir && cin >> amnt) {
    if (dir == "down")
      updown += amnt;
    else if (dir == "up")
      updown -= amnt;
    else {
      assert(dir == "forward");
      horiz += amnt;
      if (part2)
        depth += amnt * aim;
    }
  }
  cout << horiz * depth << '\n';
}

void part1() { solve(false); }
void part2() { solve(true); }

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
