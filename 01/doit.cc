// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>

using namespace std;

void solve(int offset) {
  vector<int> depths;
  int depth;
  while (cin >> depth)
    depths.push_back(depth);
  int increases = 0;
  for (size_t i = 0; i + offset < depths.size(); ++i)
    if (depths[i + offset] > depths[i])
      ++increases;
  cout << increases << '\n';
}

void part1() { solve(1); }
void part2() { solve(3); }

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
