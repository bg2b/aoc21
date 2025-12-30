// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <map>
#include <utility>
#include <cassert>

using namespace std;

int sgn(int d) { return d > 0 ? +1 : (d < 0 ? -1 : 0); }

bool is_horiz_or_vert(int x1, int y1, int x2, int y2) {
  return x1 == x2 || y1 == y2;
}

struct vents {
  // (x, y) => number of vents at that position
  map<pair<int, int>, int> num_vents;
  // Number of (x, y) with at least two vents
  int num_overlaps{0};

  // Add a vent at (x, y)
  void add(int x, int y);
  // Add a line of vents
  void add(int x1, int y1, int x2, int y2);
};

void vents::add(int x, int y) {
  if (++num_vents[{x, y}] == 2)
    ++num_overlaps;
}

void vents::add(int x1, int y1, int x2, int y2) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  assert(dx == 0 || dy == 0 || abs(dx) == abs(dy));
  dx = sgn(dx);
  dy = sgn(dy);
  while (x1 != x2 || y1 != y2) {
    add(x1, y1);
    x1 += dx;
    y1 += dy;
  }
  add(x2, y2);
}

void solve(bool horiz_vert_only) {
  vents ocean_floor;
  int x1, y1, x2, y2;
  char comma;
  string arrow;
  while (cin >> x1 >> comma >> y1 >> arrow >> x2 >> comma >> y2)
    if (!horiz_vert_only || is_horiz_or_vert(x1, y1, x2, y2))
      ocean_floor.add(x1, y1, x2, y2);
  cout << ocean_floor.num_overlaps << '\n';
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
