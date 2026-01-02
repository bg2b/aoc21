// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <list>
#include <cassert>

using namespace std;

int const dim = 3;

struct cube {
  int l[dim];
  int h[dim];

  // Construct from cin
  cube();
  // Cube covering +/-max_coord in all dimensions
  cube(int max_coord);

  // Do two cubes overlap?
  bool overlaps(cube const &c) const;

  // Cut a cube along dimension i at position split (between l[i] and
  // h[i])
  cube low_part(int i, int split) const;
  cube high_part(int i, int split) const;

  // Return a disjoint list of cubes that make up whatever's outside
  // the overlapping cube c
  list<cube> subtract(cube const &c) const;

  size_t volume() const;
};

cube::cube() {
  for (int i = 0; i < dim; ++i) {
    char axis, equals, dot;
    cin >> axis >> equals >> l[i] >> dot >> dot >> h[i];
    if (i < dim - 1) {
      char comma;
      cin >> comma;
    }
    assert(axis == 'x' + i && l[i] <= h[i]);
  }
}

cube::cube(int max_coord) {
  for (int i = 0; i < dim; ++i) {
    l[i] = -max_coord;
    h[i] = max_coord;
  }
}

bool cube::overlaps(cube const &c) const {
  for (int i = 0; i < dim; ++i)
    if (h[i] < c.l[i] || l[i] > c.h[i])
      return false;
  return true;
}

cube cube::low_part(int i, int split) const {
  assert(l[i] <= split);
  cube result(*this);
  result.h[i] = split;
  return result;
}

cube cube::high_part(int i, int split) const {
  assert(split <= h[i]);
  cube result(*this);
  result.l[i] = split;
  return result;
}

list<cube> cube::subtract(cube const &c) const {
  assert(overlaps(c));
  list<cube> result;
  cube remainder(*this);
  for (int i = 0; i < dim; ++i) {
    // You are lost in a maze of twisty little <, =, >, l, h, all
    // different
    assert(remainder.h[i] >= c.l[i] && remainder.l[i] <= c.h[i]);
    if (remainder.l[i] < c.l[i]) {
      result.push_back(remainder.low_part(i, c.l[i] - 1));
      remainder = remainder.high_part(i, c.l[i]);
    }
    if (remainder.h[i] > c.h[i]) {
      result.push_back(remainder.high_part(i, c.h[i] + 1));
      remainder = remainder.low_part(i, c.h[i]);
    }
  }
  assert(result.size() <= 2 * dim);
  return result;
}

size_t cube::volume() const {
  size_t result = 1;
  for (int i = 0; i < dim; ++i)
    result *= h[i] - l[i] + 1;
  return result;
}

void solve(bool part1) {
  list<cube> cubes;
  string cmd;
  while (cin >> cmd) {
    assert(cmd == "on" || cmd == "off");
    cube c;
    // Part 1 only considers cubes overlapping the [-50,50] range
    if (part1 && !c.overlaps(cube(50)))
      continue;
    list<cube> minus_c;
    for (auto const &existing : cubes)
      if (existing.overlaps(c)) {
        auto diff = existing.subtract(c);
        minus_c.insert(minus_c.end(), diff.begin(), diff.end());
      } else
        minus_c.push_back(existing);
    cubes = minus_c;
    if (cmd == "on")
      cubes.push_back(c);
  }
  size_t total_set = 0;
  for (auto const &c : cubes)
    total_set += c.volume();
  cout << total_set << '\n';
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
