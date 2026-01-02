// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

// Alternative using inclusion-exclusion.  The idea is to keep a map
// of cube => int where cubes are allowed to be added or subtracted.
// When doing an op with a new cube, find the intersection with all
// the cubes in the map.  For each such intersection, subtract the
// value for the existing cube (basically cancel out the overlap
// portion).  Then if turning on the new cube, add 1 to that region in
// the map.  See the argument in solve() for more details.
//
// The main benefit of this is that cube intersection is easy, while
// the regular cube slicing is more finicky.  The drawback is that the
// representation is generally not so compact.  It's actually similar
// speed or maybe slightly faster on the actual input, but I tried
// some larger examples with more overlap, and eventually it bogs
// down.

#include <iostream>
#include <map>
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

  // Return the intersection of two cubes
  cube intersection(cube const &c) const;

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

cube cube::intersection(cube const &c) const {
  assert(overlaps(c));
  cube result(*this);
  for (int i = 0; i < dim; ++i) {
    result.l[i] = max(result.l[i], c.l[i]);
    result.h[i] = min(result.h[i], c.h[i]);
  }
  return result;
}

size_t cube::volume() const {
  size_t result = 1;
  for (int i = 0; i < dim; ++i)
    result *= h[i] - l[i] + 1;
  return result;
}

// Some total order
bool operator<(cube const &c1, cube const &c2) {
  for (int i = 0; i < dim; ++i) {
    if (c1.l[i] != c2.l[i])
      return c1.l[i] < c2.l[i];
    if (c1.h[i] != c2.h[i])
      return c1.h[i] < c2.h[i];
  }
  // Equal
  return false;
}

void solve(bool part1) {
  // Map from cube to number of times points in the cube appear (can
  // be negative for subtracting overlaps)
  map<cube, int> cubes;
  string cmd;
  while (cin >> cmd) {
    assert(cmd == "on" || cmd == "off");
    cube c;
    // Part 1 only considers cubes overlapping the [-50,50] range
    if (part1 && !c.overlaps(cube(50)))
      continue;
    map<cube, int> updates;
    for (auto const &existing : cubes)
      if (existing.first.overlaps(c)) {
        // Subtle... why does this work?  Consider some point that's
        // within the cube c.  Assume by induction that the sum of
        // existing cube * count for cubes containing the point is
        // either 1 or 0, depending on whether the point is on or not.
        //
        // 1. Suppose the point is on already.  The sum of existing
        //    counts is +1 by the induction hypothesis.  The negation
        //    here will then produce a -1.  That'll cancel the
        //    existing 1 to give 0.
        //
        // 2. Suppose the point is currently off.  The existing sum is
        //    0, so the negation won't produce a net change, and it'll
        //    stay 0.
        //
        // In both cases, the count of any point in c will be 0 after
        // this loop.  If the command is "off", that's the right
        // answer, and if the command is "on" then ++new_cubes[c] will
        // give 1.  QED
        cube overlap = existing.first.intersection(c);
        updates[overlap] -= existing.second;
      }
    if (cmd == "on")
      ++updates[c];
    for (auto const &[c, amount] : updates)
      cubes[c] += amount;
  }
  ssize_t total_set = 0;
  for (auto const &c : cubes)
    total_set += c.first.volume() * c.second;
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
