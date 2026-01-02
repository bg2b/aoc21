// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

struct cucumbers {
  vector<string> sea_floor;

  int H() const { return sea_floor.size(); }
  int W() const { return sea_floor[0].size(); }

  // Construct from cin
  cucumbers();

  // Wrap coordinates beyond H() or W()
  void maybe_wrap(int &i, int &j) const;

  // Does wrapping, so access at H() or W() is OK
  bool is_empty(int i, int j) const;

  // Try to do one step of cucumbers of the given type, return true if
  // any moved
  bool step(char which);

  // Note | not ||
  bool step() { return step('>') | step('v'); }
};

cucumbers::cucumbers() {
  string line;
  while (getline(cin, line)) {
    sea_floor.push_back(line);
    assert(sea_floor.back().size() == sea_floor.front().size());
  }
}

void cucumbers::maybe_wrap(int &i, int &j) const {
  if (i >= H())
    i -= H();
  if (j >= W())
    j -= W();
}

bool cucumbers::is_empty(int i, int j) const {
  maybe_wrap(i, j);
  return sea_floor[i][j] == '.';
}

bool cucumbers::step(char type) {
  int delta_i = type == 'v' ? +1 : 0;
  int delta_j = type == '>' ? +1 : 0;
  vector<pair<int, int>> moving;
  for (int i = 0; i < H(); ++i)
    for (int j = 0; j < W(); ++j)
      if (sea_floor[i][j] == type && is_empty(i + delta_i, j + delta_j))
        moving.emplace_back(i, j);
  for (auto [i, j] : moving) {
    sea_floor[i][j] = '.';
    i += delta_i;
    j += delta_j;
    maybe_wrap(i, j);
    sea_floor[i][j] = type;
  }
  return !moving.empty();
}

void part1() {
  cucumbers cucs;
  int steps = 1;
  while (cucs.step())
    ++steps;
  cout << steps << '\n';
}

void part2() { cout << "Remotely Start The Sleigh!\n"; }

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
